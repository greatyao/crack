#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loadfiles.h"
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"


/* Load hashlist file */
int load_hashes_file(const char *filename,  int algo, struct crack_hash* hashes, int count)
{
    FILE *hashfile;
    char buf[HASHFILE_MAX_LINE_LENGTH*3];
    int ret;
    char filename_copy[512]; 
	struct hash_support_plugins* plugin;
	int total = 0;
	
	plugin = locate_by_algorithm(algo);
	if(!plugin)
		return ERR_NO_SUPPORT_ALGO;
    
    strcpy(filename_copy, filename);
    
	//文件类型解密的解析
    if (plugin->special())
    {
		ret = plugin->parse(NULL, (char *)filename, hashes);
		if (ret == 0) 
		{
			CLog::Log(LOG_LEVEL_NOMAL, "File %s loaded successfully\n",filename);
			return 1;
		}
		else
		{
			CLog::Log(LOG_LEVEL_WARNING, "Cannot load file: %s!\n", filename);
			return ret;
		}
    }

	//md5等非文件类型的解密
    hashfile = fopen(filename, "r");
    if (hashfile == NULL)
    {
		CLog::Log(LOG_LEVEL_WARNING, "Cannot open hashlist file: %s\n", filename);
		return ERR_FILENOEXIST;
    }
    
	while (!feof(hashfile))
	{
	    if (fgets((char *)&buf, HASHFILE_MAX_LINE_LENGTH, hashfile) != NULL)
	    {
			if ((strlen(buf) > 0) && (buf[strlen(buf)-1] == '\n')) 
				buf[strlen(buf)-1] = 0;
			if (buf[strlen(buf)-1] == '\r') 
				buf[strlen(buf)-1] = 0;
			if(total >= count)
			{
				CLog::Log(LOG_LEVEL_WARNING, "Not enough memory to insert new hash_list entry!\n");
				break;
			}
			if (plugin->parse(buf, NULL, hashes+total) == 0)
				total++;
	    }
	}
    
    fclose(hashfile);
    CLog::Log(LOG_LEVEL_NOMAL, "(%s): %d hashes loaded successfully\n", filename_copy, total);
    return total;
}


/* Load single hash (from command-line) */
int load_single_hash(char *hash, int algo, struct crack_hash* hashes)
{
	struct hash_support_plugins* plugin = locate_by_algorithm(algo);
	if(!plugin)
		return ERR_NO_SUPPORT_ALGO;
		
    if (plugin->special())
		return ERR_INVALID_PARAM;

    if (!hash || !hashes) 
		return ERR_INVALID_PARAM;
    
    int ret = plugin->parse(hash, NULL, hashes);
	
	if(ret == 0)
		return 1;
    return ret;
}

