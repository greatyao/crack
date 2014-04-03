#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loadfiles.h"
#include "plugin.h"
#include "algorithm_types.h"
#include "err.h"
#include "CLog.h"

#define MAX_HASHES_PER_FILE 64

int load_hashes_file2(const char *filename, struct crack_task* task)
{
	struct crack_hash hashes[MAX_HASHES_PER_FILE];
	int n, special;
	
	n = load_hashes_file(filename, task->algo, hashes, MAX_HASHES_PER_FILE, &special);
	if(n <= 0) return n;
	
	task->count = n;
	task->hashes = (struct crack_hash*)malloc(sizeof(struct crack_hash)*n);
	memcpy(task->hashes, hashes, sizeof(struct crack_hash)*n);
	task->special = special;
	return n;
}


/* Load hashlist file */
int load_hashes_file(const char *filename,  int algo, struct crack_hash* hashes, int count, int* special)
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
	if(*special) *special = plugin->special();
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

int load_single_hash2(char *hash, struct crack_task* task)
{
	struct crack_hash hashes;
	int n = load_single_hash(hash, task->algo, &hashes);
	if(n <= 0) return n;
	
	task->special = 0;
	task->count = 1;
	task->hashes = (struct crack_hash*)malloc(sizeof(struct crack_hash));
	memcpy(task->hashes, &hashes, sizeof(struct crack_hash));
	return 1;
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

int release_hashes_from_load(struct crack_task* task)
{
	if(task->count > 0 && task->hashes)
	{
		free(task->hashes);
		task->hashes = NULL;
		task->count = 0;
	}
	return 0;
}
