#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#ifdef __cplusplus
extern "C"{
#endif


struct crack_hash;

//hash�ļ�������������md5���޹��ļ����ܵ����ͣ�ֱ�ӵ��ж�ȡ���벻Ҫ��filename������
//����word֮����ļ����ܵ����ͣ�ֱ�Ӵ��ļ���ȡ
//�������Ͷ�ֻ��һ��hash�������
typedef int (*hash_plugin_parse_hash)(char *hashline, char *filename, struct crack_hash* hash);

//������ļ���Ľ��ܷ���1�����򷵻�0
typedef int (*hash_plugin_is_special)();

//���hash���Ƿ�Ϸ�
typedef int (*hash_plugin_check_valid)(struct crack_hash* hash);

//��HASH����ԭ��ԭ���ĸ�ʽ
typedef int (*hash_plugin_recovery)(const struct crack_hash* hash, char* line, int size);

struct hash_support_plugins
{
	int algo;
	hash_plugin_parse_hash parse;
	hash_plugin_is_special special;
	hash_plugin_check_valid check;
	hash_plugin_recovery recovery;
};

extern struct hash_support_plugins all_plugins[];

//�����㷨��������all_plugins�������������ڷ���NULL
struct hash_support_plugins* locate_by_algorithm(int algo);


//
int md4_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md4_is_special();
int md4_check_valid(struct crack_hash* hash);
int md4_recovery(const struct crack_hash* hash, char* line, int size);

//md5(�������������ƣ�����������������������,Ȼ����Ӧ��ʵ�֣�����䵽all_plugins)
int md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md5_is_special();
int md5_check_valid(struct crack_hash* hash);
int md5_recovery(const struct crack_hash* hash, char* line, int size);

//
int md5md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md5md5_is_special();
int md5md5_check_valid(struct crack_hash* hash);
int md5md5_recovery(const struct crack_hash* hash, char* line, int size);

//
int md5unix_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md5unix_is_special();
int md5unix_check_valid(struct crack_hash* hash);
int md5unix_recovery(const struct crack_hash* hash, char* line, int size);

//
int oscommerce_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int oscommerce_is_special();
int oscommerce_check_valid(struct crack_hash* hash);
int oscommerce_recovery(const struct crack_hash* hash, char* line, int size);

//
int ipb2_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int ipb2_is_special();
int ipb2_check_valid(struct crack_hash* hash);
int ipb2_recovery(const struct crack_hash* hash, char* line, int size);

//
int joomla_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int joomla_is_special();
int joomla_check_valid(struct crack_hash* hash);
int joomla_recovery(const struct crack_hash* hash, char* line, int size);

//
int vbulletin_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int vbulletin_is_special();
int vbulletin_check_valid(struct crack_hash* hash);
int vbulletin_recovery(const struct crack_hash* hash, char* line, int size);

//
int desunix_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int desunix_is_special();
int desunix_check_valid(struct crack_hash* hash);
int desunix_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha1_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha1_is_special();
int sha1_check_valid(struct crack_hash* hash);
int sha1_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha1sha1_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha1sha1_is_special();
int sha1sha1_check_valid(struct crack_hash* hash);
int sha1sha1_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha256_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha256_is_special();
int sha256_check_valid(struct crack_hash* hash);
int sha256_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha512_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha512_is_special();
int sha512_check_valid(struct crack_hash* hash);
int sha512_recovery(const struct crack_hash* hash, char* line, int size);

//
int mysql5_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int mysql5_is_special();
int mysql5_check_valid(struct crack_hash* hash);
int mysql5_recovery(const struct crack_hash* hash, char* line, int size);

//
int ntlm_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int ntlm_is_special();
int ntlm_check_valid(struct crack_hash* hash);
int ntlm_recovery(const struct crack_hash* hash, char* line, int size);

//
int lm_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int lm_is_special();
int lm_check_valid(struct crack_hash* hash);
int lm_recovery(const struct crack_hash* hash, char* line, int size);

//
int wordpress_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int wordpress_is_special();
int wordpress_check_valid(struct crack_hash* hash);
int wordpress_recovery(const struct crack_hash* hash, char* line, int size);

//
int apr1_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int apr1_is_special();
int apr1_check_valid(struct crack_hash* hash);
int apr1_recovery(const struct crack_hash* hash, char* line, int size);

//
int mssql_2000_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int mssql_2000_is_special();
int mssql_2000_check_valid(struct crack_hash* hash);
int mssql_2000_recovery(const struct crack_hash* hash, char* line, int size);

//
int mssql_2005_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int mssql_2005_is_special();
int mssql_2005_check_valid(struct crack_hash* hash);
int mssql_2005_recovery(const struct crack_hash* hash, char* line, int size);

//
int mssql_2012_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int mssql_2012_is_special();
int mssql_2012_check_valid(struct crack_hash* hash);
int mssql_2012_recovery(const struct crack_hash* hash, char* line, int size);

//
int mediawiki_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int mediawiki_is_special();
int mediawiki_check_valid(struct crack_hash* hash);
int mediawiki_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha256unix_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha256unix_is_special();
int sha256unix_check_valid(struct crack_hash* hash);
int sha256unix_recovery(const struct crack_hash* hash, char* line, int size);

//
int sha512unix_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int sha512unix_is_special();
int sha512unix_check_valid(struct crack_hash* hash);
int sha512unix_recovery(const struct crack_hash* hash, char* line, int size);

//
int nsldap_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int nsldap_is_special();
int nsldap_check_valid(struct crack_hash* hash);
int nsldap_recovery(const struct crack_hash* hash, char* line, int size);

//
int nsldaps_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int nsldaps_is_special();
int nsldaps_check_valid(struct crack_hash* hash);
int nsldaps_recovery(const struct crack_hash* hash, char* line, int size);

//һЩ���ú���
//char* strupr(char* ioString);//*******************************mike
char* strlow(char* ioString);
void hex2str(char *str, char *hex, int len);
void _to64(char *s, unsigned long v, int n);

#if defined(WIN32) || defined(WIN64)
#define snprintf _snprintf
#define strtok_r strtok_s
#define bzero(a, s) memset(a, 0, s)
#endif

#ifdef __cplusplus
}
#endif

#endif