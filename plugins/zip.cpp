#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "str_api.h"
#include <iostream>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

unsigned int g_CrcTable[256];

static unsigned char zip_normbuf[5][12];
static unsigned char zip_crc32[4];
static unsigned char zip_tim[2];
static long fileoffset;
static int filenamelen;
static int comprsize, ucomprsize;
static char zipbuf[1024*16];
static char verifiers[5];
static int cur=0;
static long offsets[5];
static int has_winzip_encryption, has_ext_flag, winzip_key_size, winzip_salt_size;
static unsigned char winzip_salt[16];
static unsigned char winzip_check[2];
static unsigned int fcrc;

#define kCrcPoly 0xEDB88320
static void  CrcGenerateTable(void)
{
	unsigned int i;
	for (i = 0; i < 256; i++)
	{
		unsigned int r = i;
		int j;
		for (j = 0; j < 8; j++)
			r = (r >> 1) ^ (kCrcPoly & ~((r & 1) - 1));
		g_CrcTable[i] = r;
	}
}


int zip_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	FILE *fd;
	int added;
	char buf[4096];
	unsigned int u321;
	unsigned short u161, genpurpose, extrafieldlen;
	int parsed=0,compmethod=0,fileissmall=0;
	int usizes[5],csizes[5];

	fileoffset = 0;
	CrcGenerateTable();
	comprsize=ucomprsize=0;
	fileoffset=0;
	memset(zipbuf,0,1024*16);
	memset(verifiers,0,5);
	memset(zip_crc32,0,4);
	memset(zip_tim,0,2);

	fd = fopen(filename, "rb");
	if (fd==NULL)
	{
		if (!hashline) printf("Cannot open file %s\n", filename);
		return 1;
	}
	fread(&u321,1,4,fd);
	fileoffset+=4;
	if (u321 != 0x04034b50)
	{
		if (!hashline) printf("Not a ZIP file: %s!\n", filename);
		return 1;
	}
	fclose(fd);
	fileoffset=0;
	fd = fopen(filename, "rb");

	while (!parsed)
	{
		has_winzip_encryption=0;
		has_ext_flag=0;
		compmethod=0;
		fileissmall=0;

		fread(&u321,1,4,fd);
		fileoffset+=4;
		if (u321 != 0x04034b50)
		{
			parsed=1;
			break;
		}

		/* version needed to extract */
		fread(&u161,1,2,fd);
		fileoffset+=2;
		/* general purpose bit flag */
		fread(&genpurpose,1, 2,fd);
		fileoffset+=2;
		/* compression method, last mod file time, last mod file date */
		fread(&u161,1,2,fd);
		fileoffset+=2;
		compmethod=u161;
		if (u161 == 99) 
		{
			has_winzip_encryption = 1;
		}
		fread(&zip_tim,1, 2,fd);
		fread(&u161,1, 2,fd);
		fileoffset+=4;

		/* crc32 */
		fread(zip_crc32,1, 4,fd);
		if (cur==0) memcpy(&fcrc,zip_crc32,4);
		fileoffset+=4;

		/* compressed size */
		fread(&comprsize,1, 4,fd);
		fileoffset+=4;

		/* uncompressed size */
		fread( &ucomprsize,1, 4,fd);
		fileoffset+=4;
		if (ucomprsize<100) fileissmall=1;

		/* file name length */
		fread(&filenamelen,1, 2,fd);
		fileoffset+=2;

		/* extra field length */
		fread(&extrafieldlen,1, 2,fd);
		fileoffset+=2;

		/* file name */
		bzero(buf,4096);
		fread(buf,1, filenamelen,fd);
		fileoffset+=filenamelen;
		//printf("File: %s verneeded=%d haswe=%d compmethod=%d eflen=%d\n",buf,verneeded,has_winzip_encryption,compmethod,extrafieldlen);

		/* extra field should be taken care if winzip encryption is used */
		fread(buf,1, extrafieldlen,fd);
		fileoffset+=extrafieldlen;

		if (CHECK_BIT(genpurpose, 3) == 1) has_ext_flag = 1;
		else has_ext_flag=0;
		//printf("has extra flag=%d\n",has_ext_flag);
		added=0;

		/* check if bit 0 in genpurpose are set => we've got encryption */
		if (CHECK_BIT(genpurpose, 0) == 0)
		{
		}
		else 
		{
			parsed=1;
			if (has_winzip_encryption == 1) 
			{
				switch (buf[8]&255)
				{
				case 1: winzip_key_size = 128;winzip_salt_size = 8;break;
				case 2: winzip_key_size = 192;winzip_salt_size = 12;break;
				case 3: winzip_key_size = 256;winzip_salt_size = 16;break;
				default: if (!hashline) printf("Unknown AES encryption key length (0x%02x) quitting...\n",buf[8]&255);return 1;
				}
				if (!hashline) printf("Encrypted using strong AES%d encryption\n",winzip_key_size);
			}
			// Parse the encryption header - the winzip way 
			if (has_winzip_encryption)
			{
				fread(winzip_salt,1, winzip_salt_size,fd);
				fread(winzip_check,1, 2,fd);
			}
			else if ((compmethod==8)&&(fileissmall==0))
			{
				//if (has_ext_flag==0) 
				//{
				verifiers[cur]=zip_tim[1]&255;
				//}
				//else 
				//{
				//    verifiers[cur]=(zip_crc32[3]&255);
				//}

				fread((char *)zip_normbuf[cur], 1,12,fd);
				fileoffset+=12;
				offsets[cur]=fileoffset;
				comprsize-=12;
				csizes[cur]=comprsize;
				usizes[cur]=ucomprsize;
				cur++;
				added=1;
			}
			else parsed=0;
		}

		if ((cur<5)&&(has_winzip_encryption==0)) parsed=0;
		if (parsed==0)
		{
			fseek(fd,comprsize,SEEK_CUR);
			fileoffset+=comprsize;
			fread(&u321,1,4,fd);
			if (u321==0x08074b50)
			{
				fileoffset+=4;
				fseek(fd,12,SEEK_CUR);
				fileoffset+=12;
			}
			else
			{
				if (added==1) verifiers[cur-1]=(zip_crc32[3]&255);
				fseek(fd,-4,SEEK_CUR);
			}
		}
	}


	if ((parsed==0)&&(cur!=0)&&(cur<5)&&(has_winzip_encryption==0))
	{
		parsed=1;
	}

	if ((!hashline) && (has_winzip_encryption==0)) /*printf("Found >= %d password-protected files in archive!\n",cur)*/;

	if ((cur==0)&&(has_winzip_encryption==0))
	{
		if (!hashline) printf("File %s is not a password-protected ZIP archive\n", filename);
		return 1;
	}

	if (has_winzip_encryption==0)
	{
		fseek(fd,offsets[0],SEEK_SET);
		fileoffset=offsets[0];
		fread(zipbuf,1,1024*16,fd);
		comprsize=csizes[0];
		ucomprsize=usizes[0];
	}

	fclose(fd);
	itoa(fcrc,hash->hash,16);
	strcpy(hash->salt, "");
	strcpy(hash->salt2, "");
	return 0;
}

int zip_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int zip_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;

	if(ishex(hash->hash) && strlen(hash->hash)==8)
		return 1;
	else
		return 0;
}

int zip_is_special()
{
	return 1;
}
