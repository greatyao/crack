#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#ifdef __cplusplus
extern "C"{
#endif


struct crack_hash;

//hash文件加载器，对于md5等无关文件解密的类型，直接单行读取（请不要从filename解析）
//对于word之类的文件解密的类型，直接从文件读取
//两种类型都只有一个hash串结果，
typedef int (*hash_plugin_parse_hash)(char *hashline, char *filename, struct crack_hash* hash);

//如果是文件类的解密返回1，否则返回0
typedef int (*hash_plugin_is_special)();

//检测hash串是否合法
typedef int (*hash_plugin_check_valid)(struct crack_hash* hash);

struct hash_support_plugins
{
	int algo;
	hash_plugin_parse_hash parse;
	hash_plugin_is_special special;
	hash_plugin_check_valid check;
};

extern struct hash_support_plugins all_plugins[];

//根据算法查找其在all_plugins的索引，不存在返回NULL
struct hash_support_plugins* locate_by_algorithm(int algo);


//md5(其他的依次类推，请在这里做几个函数申明,然后相应的实现，并填充到all_plugins)
int md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md5_is_special();
int md5_check_valid(struct crack_hash* hash);



//一些常用函数
char* strupr(char* ioString);
char* strlow(char* ioString);
void hex2str(char *str, char *hex, int len);
void _to64(char *s, unsigned long v, int n);

#ifdef __cplusplus
}
#endif

#endif