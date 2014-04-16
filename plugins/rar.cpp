#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(WIN32) || defined(WIN64)
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif
#include <stdlib.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"
#include <iostream>

#define BYTESWAP32(n) ( \
	(((n)&0x000000ff) << 24) | \
	(((n)&0x0000ff00) << 8 ) | \
	(((n)&0x00ff0000) >> 8 ) | \
	(((n)&0xff000000) >> 24) )

typedef struct bestfile_s
{
	unsigned int packedsize;
	unsigned int filepos;
	unsigned char filename[255];
} bestfile_t;
static bestfile_t bestfile[1024];


static char myfilename[255];
static long filepos;
static unsigned int packedsize;
static unsigned int unpackedsize;
static uint64_t packedsize64;
static uint64_t unpackedsize64;
static unsigned char filecrc[4];

static char salt[8];
static int issalt;
static int islarge;
static unsigned short namesize;
static char encname[256];

static unsigned char header[40];
static unsigned int headerenc=0;
static unsigned short flags;
static unsigned char savedbuf[128];

int rar_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!filename || !hash) 
		return ERR_INVALID_PARAM;
	FILE *fd;
	int ret,a,b,c;
	char buf[4096];
	unsigned int u321;
	unsigned short u161;
	unsigned char u81,htype;
	char signature[7];
	char *filebuf;
	unsigned short headersize;
	int goodtogo=0,best=0;

	issalt = islarge = 0;
	strcpy(myfilename, filename);

	fd = fopen(filename, "rb");
	if (fd==NULL)
	{
		fclose(fd);
		if (!hashline) printf("Cannot open file %s\n", filename);
		return 1;
	}
	fread(signature,1,7,fd);
	filepos = 7;

	if ( (signature[0]!=0x52) || (signature[1]!=0x61) || (signature[2]!=0x72) || 
		(signature[3]!=0x21) || (signature[4]!=0x1a) || (signature[5]!=0x07))
	{
		fclose(fd);
		if (!hashline) printf("Not a RAR3 archive: %s", filename);
		return 1;
	}
	ret=0;
	while (ret>=0)
	{
		/* header CRC (2) */
		fread(&u161,1,2,fd);
		flags=u161;
		filepos+=2;
		/* header type (1) */
		fread(&htype,1,1,fd);
		filepos++;
		//printf("htype=%02x\n",htype);
		if (htype==0x74)
		{
			/* flags (2) */
			fread(&u161,1,2,fd);

			filepos+=2;
			issalt=0;
			islarge=0;
			if (!(u161 & 0x4))
			{
				if (!hashline) printf("RAR archive %s is not password protected!\n",filename);
				//return 1;
			}
			if ((u161 & 0x400))
			{
				issalt = 1;
			}
			if ((u161 & 0x100))
			{
				islarge = 1;
			}
			/* header size (2) */
			fread(&u161,1,2,fd);
			filepos+=u161-8;
			headersize=u161;
			fread(&packedsize,1,4,fd);
			fread(&unpackedsize,1,4,fd);
			fread(&u81,1,1,fd);
			fread(filecrc,1,4,fd);
			/* read time,ver,method = 6 bytes */
			fread(buf,1, 6,fd);
			/* read namesize */
			fread(&namesize,1,2,fd);
			/* read attr */
			fread(&u321,1, 4,fd);
			/* read 64-bit size if used */
			if (islarge)
			{
				fread(&packedsize64,1,8,fd);
				fread(&unpackedsize64,1,8,fd);
			}
			fread(encname,1, namesize,fd);

			if (issalt)
			{
				fread(salt,1, 8,fd);
			}
			//printf("Found file: %s packedsize: %d headersize=%d\n",encname,packedsize,headersize);

			if (packedsize<(32*1024*1024))
			{
				fseek(fd,headersize-((issalt*8)+(islarge)*16+32+namesize),SEEK_CUR);
				filepos = fseek(fd,0,SEEK_CUR);
				bestfile[best].filepos = fseek(fd,0,SEEK_CUR);
				bestfile[best].packedsize=packedsize;
				strcpy((char *)bestfile[best].filename,encname);
				best++;
				fseek(fd,packedsize,SEEK_CUR);
			}
			else 
			{
				fseek(fd,headersize-((issalt*8)+(islarge)*16+32+namesize),SEEK_CUR);
				fseek(fd,packedsize,SEEK_CUR);
			}

		}
		else if (htype==0x73)
		{
			fread(&u161,1,2,fd);
			filepos+=2;

			if (((u161>>7)&255)!=0)
			{
				printf("Encrypted header found!\n%s","");
				fread(&u161,1,2,fd);
				fread(&u161,1,2,fd);
				fread(&u321,1,4,fd);
				filepos+=8;
				/* Read in the salt */
				fread(salt,1, 8,fd);
				filepos+=8;
				fread(header,1, 32,fd);
				/* Better idea: Marc Bevand's one :) */
				fseek(fd,-24,SEEK_END);
				fread(salt,1,8,fd);
				fread(header,1,16,fd);
				headerenc=1;
				goodtogo=1;
				goto out;
			}
			fread(&u161,1,2,fd);
			fread(&u161,1,2,fd);
			fread(&u321,1,4,fd);
			filepos+=8;
		}
		else 
		{
			ret=-1;
		}
	}
out:
	if ((goodtogo==0)&&(best<1))
	{
		fclose(fd);
		printf("No crackable archive files found, exiting...%s\n","");
		return 1;
	}

	if (headerenc==0)
	{
		b=0xfffffff;
		c=0;
		for (a=0;a<best;a++) if (b>bestfile[a].packedsize) {b=bestfile[a].packedsize;c=a;}
		fseek(fd,bestfile[c].filepos,SEEK_SET);
		packedsize=bestfile[c].packedsize;

		filebuf = (char*)malloc(packedsize);
		fread(filebuf,1,packedsize,fd);
		FILE *ofd=fopen("outfile","w+");
		fwrite(filebuf,1,packedsize,ofd);
		memcpy(savedbuf,filebuf,128);
		if (!hashline) /*printf("Best file chosen to attack: %s\n",bestfile[c].filename)*/;
		fclose(ofd);
		free(filebuf);
	}
	fclose(fd);

	strcpy(hash->hash, atoh((unsigned char*)filecrc,8));
	strcpy(hash->salt, atoh((unsigned char*)salt,16));
	strcpy(hash->salt2, "");

	return 0;
}

int rar_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s:%s", hash->hash,hash->salt);
	return 0;
}

int rar_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;

	if(ishex(hash->hash) && strlen(hash->hash)==8 && ishex(hash->salt) && strlen(hash->salt)==16)
		return 1;
	else
		return 0;
}

int rar_is_special()
{
	return 1;
}
