#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"
#include <iostream>

namespace msoffice{
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



	/* 
	Read stream from mini table - callee needs to free memory 
	TODO: what if stream is in FAT? Until now I haven't seen a case
	like that with EncryptionStream (it's usually around 1KB, far below 4KB)
	Anyway, this should be handled properly some day.
	*/
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

	static const char Pad64 = '=';
	static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static int b64_pton(char const *src, unsigned char *target, int targsize)
	{
		int tarindex, state, ch;
		char *pos;

		state = 0;
		tarindex = 0;

		while ((ch = *src++) != '\0') 
		{
			//if (isspace((char)ch)) continue;
			if ((ch>=-1 && ch<=255) && isspace(ch)) continue;
			if (ch == Pad64) break;

			pos = (char*)strchr(Base64, ch);
			if (pos == 0) return (-1);

			switch (state)
			{
			case 0:
				if (target)
				{
					if ((size_t)tarindex >= targsize) return (-1);
					target[tarindex] = (pos - Base64) << 2;
				}
				state = 1;
				break;

			case 1:
				if (target)
				{
					if ((size_t)tarindex + 1 >= targsize) return (-1);
					target[tarindex]   |=  (pos - Base64) >> 4;
					target[tarindex+1]  = ((pos - Base64) & 0x0f) << 4 ;
				}
				tarindex++;
				state = 2;
				break;

			case 2:
				if (target)
				{
					if ((size_t)tarindex + 1 >= targsize) return (-1);
					target[tarindex]   |=  (pos - Base64) >> 2;
					target[tarindex+1]  = ((pos - Base64) & 0x03) << 6;
				}
				tarindex++;
				state = 3;
				break;
			case 3:
				if (target)
				{
					if ((size_t)tarindex >= targsize) return (-1);
					target[tarindex] |= (pos - Base64);
				}
				tarindex++;
				state = 0;
				break;

			default:
				abort();
			}
		}

		if (ch == Pad64)
		{
			ch = *src++;
			switch (state)
			{
			case 0:
			case 1: return (-1);
			case 2:
				for ((void)NULL; ch != '\0'; ch = *src++) if (!isspace(ch)) break;
				if (ch != Pad64) return (-1);
				ch = *src++;
			case 3:
				for ((void)NULL; ch != '\0'; ch = *src++) if (!isspace(ch)) return (-1);
				if (target && target[tarindex] != 0) return (-1);
			}
		}
		else
		{
			if (state != 0) return (-1);
		}
		return (tarindex);
	}

#if defined(WIN32) || defined(WIN64)
	void* memmem(const void *l, size_t l_len, const void *s, size_t s_len)
	{
		register char *cur, *last;
		const char *cl = (const char *)l;
		const char *cs = (const char *)s;

		/* we need something to compare */
		if (l_len == 0 || s_len == 0)
			return NULL;

		/* "s" must be smaller or equal to "l" */
		if (l_len < s_len)
			return NULL;

		/* special case where s_len == 1 */
		if (s_len == 1)
			return (void*)memchr(l, (int)*cs, l_len);

		/* the last position where its possible to find "s" in "l" */
		last = (char *)cl + l_len - s_len;

		for (cur = (char *)cl; cur <= last; cur++)
			if (cur[0] == cs[0] && memcmp(cur, cs, s_len) == 0)
				return cur;

		return NULL;
	}
#endif
}

using namespace msoffice;
int msoffice_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
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
	char *token,*token1;

	fd=fopen(filename,"rb");
	if (!fd)
	{
		return 1;
	}
	fseek(fd,0,SEEK_END);
	fsize=ftell(fd);
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
	while ((orig[0]!=0)&&(strcmp(orig,"EncryptionInfo")!=0))
	{
		memcpy(utf16,buf+index,64);
		for (a=0;a<64;a+=2) orig[a/2]=utf16[a];
		memcpy(&datasector,buf+index+116,4);
		if (strcmp(orig,"Root Entry")==0)
		{
			minisectionstart=datasector;
			memcpy(&minisectionsize,buf+index+120,4);
		}
		if (strcmp(orig,"EncryptionInfo")==0)
		{
			memcpy(&datasize,buf+index+120,4);
			stream = read_stream_mini(datasector,datasize);
		}
		index+=128;
	}

	if (!stream)
	{
		//printf("No stream found!\n");
		return 1;
	}

	index = 0;

	/* Now parse the encryption stream */
	/* The office 2007 case */
	if ((((short)*(stream))==0x03)&&(((short)*(stream+2))==0x02))
	{
		unsigned int headerlen;
		unsigned int skipflags;
		unsigned int extrasize;
		unsigned int algid;
		unsigned int alghashid;
		unsigned int keysize;
		unsigned int providertype;

		fileversion=2007;
		//printf("MSOffice 2007 format!\n");
		index+=4;
		if (((unsigned int)(*(stream+index))) == 16)
		{
			//printf("External provider not supported!\n");
			free(buf);
			free(stream);
			return 1;
		}
		index+=4;
		memcpy(&headerlen,stream+index,4);
		//printf("Header length: %d\n",headerlen);
		index+=4;
		memcpy(&skipflags,stream+index,4);
		index+=4;
		memcpy(&extrasize,stream+index,4);
		index+=4;
		memcpy(&algid,stream+index,4);
		//printf("Algo ID: %08x\n",algid);
		index+=4;
		memcpy(&alghashid,stream+index,4);
		//printf("Hash algo ID: %08x\n",alghashid);
		index+=4;
		memcpy(&keysize,stream+index,4);
		//printf("Keysize: %d\n",keysize);
		keybits=keysize;
		index+=4;
		memcpy(&providertype,stream+index,4);
		//printf("Providertype: %08x\n",providertype);
		index+=8;
		headerlen-=28;
		index+=headerlen;
		memcpy(&saltsize,stream+index,4);
		//printf("Saltsize: %d\n",saltsize);
		index+=4;
		memcpy(docsalt,stream+index,saltsize);
		index+=saltsize;
		memcpy(verifier,stream+index,16);
		index+=16;
		memcpy(&verifierhashsize,stream+index,4);
		//printf("Verifier hash size: %d\n",verifierhashsize);
		index+=4;
		/* Using RC4 encryption? */
		if (providertype == 1) memcpy(verifierhash,stream+index,20);
		else memcpy(verifierhash,stream+index,32);
	}
	else if ((((short)*(stream))==0x04)&&(((short)*(stream+2))==0x04))
	{
		char *startptr;

		fileversion=2010;
		//printf("MSOffice 2010/2013 format!\n");
		index+=4;
		//printf("Provider: %d\n",((unsigned int)(*(stream+index))));
		if (((unsigned int)(*(stream+index))) == 16)
		{
			//printf("External provider not supported!\n");
			free(buf);
			free(stream);
			return 1;
		}
		index+=4;

		//printf("%s\n",stream+index);
		/* clumsy XML parsing, better one would use libxml2 */
		if (strncmp(stream+index,"<?xml version=\"1.0\" ",20)!=0)
		{
			//printf("Expected XML data, got garbage!\n");
			free(buf);
			free(stream);
			return 1;
		}
		startptr = (char*)memmem(stream,strlen(stream+8),"<p:encryptedKey",15);
		if (!startptr)
		{
			//printf("no encryptedKey parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		startptr += 15;

		/* Get spinCount */
		token = (char*)memmem(startptr,strlen(stream+8),"spinCount=\"",11);
		if (!token)
		{
			//printf("no spinCount parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 11;
		a=0;
		token1=(char*)malloc(16);
		bzero(token1,16);
		while ((token[a]!='"')&&(a<16))
		{
			token1[a]=token[a];
			a++;
		}
		spincount=atoi(token1);
		//printf("spinCount=%d\n",spincount);
		free(token1);

		/* Get keyBits */
		token = (char*)memmem(startptr,strlen(stream+8),"keyBits=\"",9);
		if (!token)
		{
			//printf("no keyBits parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 9;
		a=0;
		token1=(char*)malloc(16);
		bzero(token1,16);
		while ((token[a]!='"')&&(a<16))
		{
			token1[a]=token[a];
			a++;
		}
		keybits=atoi(token1);
		//printf("keyBits=%d\n",keybits);
		free(token1);

		/* Get saltSize */
		token = (char*)memmem(startptr,strlen(stream+8),"saltSize=\"",10);
		if (!token)
		{
			//printf("no saltSize parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 10;
		a=0;
		token1=(char*)malloc(16);
		bzero(token1,16);
		while ((token[a]!='"')&&(a<16))
		{
			token1[a]=token[a];
			a++;
		}
		saltsize=atoi(token1);
		//printf("saltsize=%d\n",saltsize);
		free(token1);

		/* Get hashAlgorithm */
		token = (char*)memmem(startptr,strlen(stream+8),"hashAlgorithm=\"",15);
		if (!token)
		{
			//printf("no hashAlgorithm parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 15;
		a=0;
		token1=(char*)malloc(16);
		bzero(token1,16);
		while ((token[a]!='"')&&(a<16))
		{
			token1[a]=token[a];
			a++;
		}
		//printf("hashAlgorithm=%s\n",token1);
		if (strcmp(token1,"SHA1") == 0) fileversion = 2010;
		else if (strcmp(token1,"SHA512") == 0) fileversion = 2013;
		else 
		{
			//printf("Unknown hash algorithm used!\n");
			free(buf);
			free(stream);
			return 1;
		}
		free(token1);

		/* Get saltValue */
		token = (char*)memmem(startptr,strlen(stream+8),"saltValue=\"",11);
		if (!token)
		{
			//printf("no saltValue parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 11;
		a=0;
		token1=(char*)malloc(64);
		bzero(token1,64);
		while ((token[a]!='"')&&(a<64))
		{
			token1[a]=token[a];
			a++;
		}
		b64_pton(token1,docsalt,saltsize+4);
		//printf("saltValue=");
		free(token1);

		/* Get encryptedVerifierHashInput */
		token = (char*)memmem(startptr,strlen(stream+8),"encryptedVerifierHashInput=\"",28);
		if (!token)
		{
			//printf("no encryptedVerifierHashInput parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 28;
		a=0;
		token1=(char*)malloc(64);
		bzero(token1,64);
		while ((token[a]!='"')&&(a<64))
		{
			token1[a]=token[a];
			a++;
		}
		b64_pton(token1,verifierhashinput,32+4);
		//printf("encryptedVerifierHashInput=");
		free(token1);

		/* Get encryptedVerifierHashValue */
		token = (char*)memmem(startptr,strlen(stream+8),"encryptedVerifierHashValue=\"",28);
		if (!token)
		{
			//printf("no encryptedVerifierHashValue parameters in XML!\n");
			free(buf);
			free(stream);
			return 1;
		}
		token += 28;
		a=0;
		token1=(char*)malloc(64);
		bzero(token1,64);
		while ((token[a]!='"')&&(a<64))
		{
			token1[a]=token[a];
			a++;
		}
		b64_pton(token1,verifierhashvalue,64+4);
		//printf("encryptedVerifierHashValue=");
		free(token1);
	}

	fclose(fd);
	free(stream);
	free(buf);

	strcpy(hash->hash, atoh(verifierhashvalue,64));
	strcpy(hash->salt, atoh(docsalt,32));
	strcpy(hash->salt2, atoh(verifierhashinput,32));

	return 0;
}

int msoffice_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;

	if(ishex(hash->hash) && strlen(hash->hash)==64 && ishex(hash->salt) && strlen(hash->salt)==32 && ishex(hash->salt2) && strlen(hash->salt2)==32)
		return 1;
	else
		return 0;
}

int msoffice_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s:%s:%s", hash->hash,hash->salt,hash->salt2);
	return 0;
}

int msoffice_is_special()
{
	return 1;
}