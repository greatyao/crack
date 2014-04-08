#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"
#include <iostream>

/* File Buffer */
static char *buf;

/* Compound file binary format ones */
static int minifatsector;
static int minisectionstart;
static int minisectionsize;
static int *difat;
static int sectorsize;

/* Encryption-specific ones */
static int fileversion = 0;
static unsigned char docsalt[32];
static unsigned char verifier[32];
static unsigned char verifierhash[32];
static unsigned char verifierhashinput[64];
static unsigned char verifierhashvalue[72];
static int verifierhashsize;
static int spincount;
static int keybits;
unsigned int saltsize;
unsigned int type=0;

/* Office 2010/2013 */
static const unsigned char encryptedVerifierHashInputBlockKey[] = { 0xfe, 0xa7, 0xd2, 0x76, 0x3b, 0x4b, 0x9e, 0x79 };
static const unsigned char encryptedVerifierHashValueBlockKey[] = { 0xd7, 0xaa, 0x0f, 0x6d, 0x30, 0x61, 0x34, 0x4e };

/* Get buffer+offset for sector */
char* get_buf_offset(int sector)
{
	return (buf+(sector+1)*sectorsize);
}

/* Get sector offset for sector */
int get_offset(int sector)
{
	return ((sector+1)*sectorsize);
}



/* Get FAT table for a given sector */
int* get_fat(int sector)
{
	char *fat=NULL;
	int difatn=0;

	if (sector<(sectorsize/4))
	{
		fat=get_buf_offset(difat[0]);
		return (int*)fat;
	}
	while ((!fat)&&(difatn<109))
	{
		if (sector>(((difatn+2)*sectorsize)/4)) difatn++;
		else fat=get_buf_offset(difat[difatn]);
	}
	return (int*)fat;
}


/* Get mini FAT table for a given minisector */
int* get_mtab(int sector)
{
	int *fat=NULL;
	char *mtab=NULL;
	int mtabn=0;
	int nextsector;

	nextsector = minifatsector;

	while (mtabn<sector)
	{
		mtabn++;
		if (sector>((mtabn*sectorsize)/4))
		{
			/* Get fat entry for next table; */
			fat = get_fat(nextsector);
			nextsector = fat[nextsector];
			mtabn++;
		}
	}
	mtab=get_buf_offset(nextsector);
	return (int*)mtab;
}


/* Get minisection sector nr per given mini sector offset */
int get_minisection_sector(int sector)
{
	int *fat=NULL;
	int sectn=0;
	int sectb=0;
	int nextsector;


	nextsector = minisectionstart;
	fat = get_fat(nextsector);
	sectn=0;
	while (sector>sectn)
	{
		sectn++;
		sectb++;
		if (sectb>=(sectorsize/64))
		{
			sectb=0;
			/* Get fat entry for next table; */
			fat = get_fat(nextsector);
			nextsector = fat[nextsector];
		}
	}
	return nextsector;
}


/* Get minisection offset */
int get_mini_offset(int sector)
{
	return ((sector*64)%(sectorsize));
}


char* read_stream_mini(int start, int size)
{
	char *lbuf=(char*)malloc(4);
	int lsize=0;
	int *mtab=NULL;     // current minitab
	int sector;

	sector=start;
	while (lsize<size)
	{
		lbuf = (char*)realloc(lbuf,lsize+64);
		memcpy(lbuf + lsize,get_buf_offset(get_minisection_sector(sector)) + get_mini_offset(sector), 64);
		lsize += 64;
		mtab = get_mtab(sector);
		sector = mtab[sector];
	}
	return lbuf;
}

/* Read stream from table - callee needs to free memory */
char* read_stream(int start, int size)
{
	char *lbuf=(char*)malloc(4);
	int lsize=0;
	int *fat=NULL;      // current minitab
	int sector;

	sector=start;

	while ((lsize)<size)
	{
		lbuf = (char*)realloc(lbuf,lsize+sectorsize);
		memcpy(lbuf + lsize,get_buf_offset(sector), sectorsize);
		lsize += sectorsize;
		fat = get_fat(sector);
		sector = fat[sector];
	}
	return lbuf;
}


int parse_xls(char *stream, int size)
{
	int offset=0;
	int headersize;
	char *headerutf16;
	char *header;
	int a;

	while (offset<size-4)
	{
		if (((short)*(stream+offset))!=0x2f) offset+=4;
		else 
		{
			offset+=4;
			if (memcmp(stream+offset,"\x00\x00",2)==0)
			{
				//printf("XOR encryption not supported");
				return 1;
			}
			else if (memcmp(stream+offset,"\x01\x00\x01\x00\x01\x00",6)==0)
			{
				//printf("RC4 encryption (40bit)\n");
				memcpy(docsalt,stream+offset+6,16);
				memcpy(verifier,stream+offset+22,16);
				memcpy(verifierhash,stream+offset+38,16);
				verifierhashsize=16;
				return 1;
			}
			else if ((memcmp(stream+offset,"\x01\x00\x02\x00",4)==0)||(memcmp(stream+offset,"\x01\x00\x03\x00",4)==0))
			{
				//printf("RC4 part (CryptoAPI)\n");
				offset+=10;
				memcpy(&headersize,stream+offset,4);
				//printf("headersize=%d\n",headersize);
				offset+=20;
				memcpy(&keybits,stream+offset,4);
				//printf("keybits=%d\n",keybits);
				offset+=16;
				headersize-=32;
				headerutf16=(char*)alloca(headersize);
				memcpy(headerutf16,stream+offset,headersize);
				header=(char*)alloca(headersize/2);
				for (a=0;a<headersize;a+=2) header[a/2]=headerutf16[a];
				if (strstr(header,"trong")) type=1;
				else type=0;
				//printf("header: %s\n",header);
				offset+=headersize;
				memcpy(&saltsize,stream+offset,4);
				offset+=4;
				//printf("saltsize=%d\n",saltsize);
				memcpy(docsalt,stream+offset,16);
				offset+=16;
				memcpy(verifier,stream+offset,16);
				offset+=16;
				memcpy(&verifierhashsize,stream+offset,4);
				offset+=4;
				//printf("verifierhashsize=%d\n",verifierhashsize);
				memcpy(verifierhash,stream+offset,20);
				return 0;
			}
		}
	}
}


int parse_doc(char *stream, int size)
{
	int offset=0;
	int headersize;
	char *headerutf16;
	char *header;
	int a;

	/* 40bit RC4 */
	if ((((short)*(stream))==1)||(((short)*(stream+2))==1))
	{
		//printf("40bit RC4\n");
		memcpy(docsalt,stream+4,16);
		memcpy(verifier,stream+20,16);
		memcpy(verifierhash,stream+36,16);
		verifierhashsize=16;
		return 1;
	}
	else if ((((short)*(stream))>=2)||(((short)*(stream+2))==2))
	{
		offset+=8;
		memcpy(&headersize,stream+offset,4);
		//printf("headersize=%d\n",headersize);
		offset+=20;
		memcpy(&keybits,stream+offset,4);
		//printf("keybits=%d\n",keybits);
		offset+=16;
		headersize-=32;
		headerutf16=(char*)alloca(headersize);
		memcpy(headerutf16,stream+offset,headersize);
		header=(char*)alloca(headersize/2);
		for (a=0;a<headersize;a+=2) header[a/2]=headerutf16[a];
		if (strstr(header,"trong")) type=1;
		else type=0;
		//printf("header: %s\n",header);
		offset+=headersize;
		memcpy(&saltsize,stream+offset,4);
		//printf("saltsize=%d\n",saltsize);
		offset+=4;
		memcpy(docsalt,stream+offset,16);
		offset+=16;
		memcpy(verifier,stream+offset,16);
		offset+=16;
		memcpy(&verifierhashsize,stream+offset,4);
		//printf("verifierhashsize=%d\n",saltsize);
		offset+=4;
		memcpy(verifierhash,stream+offset,20);
		return 0;
	}
	else
	{
		//printf("WTF is that word document?!?\n");
		return 1;
	}
}

int msoffice_old_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!filename || !hash) 
		return ERR_INVALID_PARAM;

	FILE *fd;
	int fsize;
	int index=0;
	int dirsector;
	char utf16[64];
	char orig[64];
	int datasector,datasize;
	int ministreamcutoff;
	int a;
	char *stream=NULL;

	fd=fopen(filename,"rb");
	if (!fd)
	{
		return 1;
	}
	fseek(fd,0,SEEK_END);
	fsize = ftell(fd);
	fseek(fd,0,SEEK_SET);
	buf=(char*)malloc(fsize+1);
	fread(buf,1,fsize,fd);
	if (memcmp(buf,"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",8)!=0) 
	{
		//printf("No header signature found!\n");
		free(buf);
		return 1;
	}
	index+=24;
	if (memcmp(buf+index,"\x3e\x00",2)!=0)
	{
		//printf("Minor version wrong!\n");
		free(buf);
		return 1;
	}
	index+=2;
	if ((memcmp(buf+index,"\x03\x00",2)!=0)&&(memcmp(buf+index,"\x04\x00",2)!=0))
	{
		//printf("Major version wrong!\n");
		free(buf);
		return 1;
	}
	else
	{
		if ((short)*(buf+index)==3) sectorsize=512;
		else if ((short)*(buf+index)==4) sectorsize=4096;
		else 
		{
			//printf("Bad sector size!\n");
			free(buf);
			return 1;
		}
	}

	index+=22;
	memcpy(&dirsector,(int*)(buf+index),4);
	dirsector+=1;
	dirsector*=sectorsize;
	index+=8;
	memcpy(&ministreamcutoff,(int*)(buf+index),4);
	memcpy(&minifatsector,(int*)(buf+index+4),4);
	difat=(int *)(buf+index+20);


	index=dirsector;
	orig[0]='M';
	while ((orig[0]!=0)&&((strcmp(orig,"Workbook")!=0)||(strcmp(orig,"1Table")!=0)))
	{
		memcpy(utf16,buf+index,64);
		for (a=0;a<64;a+=2) orig[a/2]=utf16[a];
		memcpy(&datasector,buf+index+116,4);
		//printf("%s \n",orig);
		if (strcmp(orig,"Root Entry")==0)
		{
			minisectionstart=datasector;
			memcpy(&minisectionsize,buf+index+120,4);
		}
		if (strcmp(orig,"Workbook")==0)
		{
			memcpy(&datasize,buf+index+120,4);
			stream = read_stream(datasector,datasize);
			if (1 == parse_xls(stream,datasize))
			{
				free(stream);
				return 1;
			}
			break;
		}
		if (strcmp(orig,"1Table")==0)
		{
			memcpy(&datasize,buf+index+120,4);
			stream = read_stream(datasector,datasize);
			if (1 == parse_doc(stream,datasize))
			{
				free(stream);
				return 1;
			}
			break;
		}
		index+=128;
	}

	if (!stream)
	{
		//printf("No stream found!\n");
		return 1;
	}


	fclose(fd);
	free(stream);
	free(buf);

	strcpy(hash->hash, atoh(verifierhash,sizeof(verifierhash)));
	strcpy(hash->salt, atoh(docsalt,sizeof(docsalt)));
	strcpy(hash->salt2, "");

	return 0;
}

int msoffice_old_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s:%s", hash->hash,hash->salt);
	return 0;
}

int msoffice_old_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;

	if(ishex(hash->hash) && strlen(hash->hash)==32 && ishex(hash->salt) && strlen(hash->salt)==32)
		return 1;
	else
		return 0;
}

int msoffice_old_is_special()
{
	return 1;
}