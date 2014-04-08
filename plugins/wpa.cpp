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

typedef struct
{
	char          essid[36];
	unsigned char mac1[6];
	unsigned char mac2[6];
	unsigned char nonce1[32];
	unsigned char nonce2[32];
	unsigned char eapol[256];
	int           eapol_size;
	int           keyver;
	unsigned char keymic[16];
} hccap_t;

char myfilename[255];
int vectorsize;
int erev=0;
hccap_t hccap;

std::string base64;

void code_block(unsigned char *in, unsigned char b)
{
	char itoa64[65] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	base64.append(1,itoa64[in[0] >> 2]);
	base64.append(1,itoa64[((in[0] & 0x03) << 4) | (in[1] >> 4)]);
	if (b) {
		base64.append(1,itoa64[((in[1] & 0x0f) << 2) | (in[2] >> 6)]);
		base64.append(1,itoa64[in[2] & 0x3f]);
	} else
		base64.append(1,itoa64[((in[1] & 0x0f) << 2)]);
}

int wpa_parse_hash(char *hashline, char *filename, struct crack_hash* hash)
{
	if (!filename || !hash) 
		return ERR_INVALID_PARAM;

	FILE *fd;
	int err;
	struct stat f_stat;

	err = stat(filename,&f_stat);
	if (err<0)
	{
		return 1;
	}
	if (f_stat.st_size!=392)
	{
		return 1;
	}

	fd = fopen(filename,"rb");
	if (fd<0)
	{
		return 1;
	}

	err = fread(&hccap,1,0x188,fd);
	if (hccap.eapol_size>256)
	{
		return 1;
	}
	if (hccap.keyver!=1)
	{
		return 1;
	}

	fclose(fd);
	strcpy(myfilename, filename);

	//$WPAPSK$essid#base64
	base64 = "$WPAPSK$";
	base64.append(hccap.essid);
	base64.append("#");

	int i=0;
	unsigned char *w = (unsigned char *) (&hccap);
	for (i = 36; i + 3 < sizeof(hccap_t); i += 3)
		code_block(&w[i], 1);
	code_block(&w[i], 0);
	strcpy(hash->hash, base64.c_str());

	strcpy(hash->salt, atoh(hccap.nonce1,sizeof(hccap.nonce1)*2));
	strcpy(hash->salt2, atoh(hccap.nonce2,sizeof(hccap.nonce2)*2));
	
	return 0;
}

int wpa_recovery(const struct crack_hash* hash, char* line, int size)
{
	if(!hash || !line || size <= 0)
		return ERR_INVALID_PARAM;

	snprintf(line, size, "%s", hash->hash);
	return 0;
}

int wpa_check_valid(struct crack_hash* hash)
{
	if(!hash)
		return ERR_INVALID_PARAM;
	
	if(isStartsWith(hash->hash,"$WPAPSK$") && countSpecChar(hash->hash,'#'))
		return 1;
	else
		return 0;
}

int wpa_is_special()
{
	return 1;
}