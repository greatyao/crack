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

struct hash_support_plugins
{
	int algo;
	hash_plugin_parse_hash parse;
	hash_plugin_is_special special;
	hash_plugin_check_valid check;
};

extern struct hash_support_plugins all_plugins[];

//�����㷨��������all_plugins�������������ڷ���NULL
struct hash_support_plugins* locate_by_algorithm(int algo);


//md5(�������������ƣ�����������������������,Ȼ����Ӧ��ʵ�֣�����䵽all_plugins)
int md5_parse_hash(char *hashline, char *filename, struct crack_hash* hash);
int md5_is_special();
int md5_check_valid(struct crack_hash* hash);



//һЩ���ú���
char* strupr(char* ioString);
char* strlow(char* ioString);
void hex2str(char *str, char *hex, int len);
void _to64(char *s, unsigned long v, int n);

#ifdef __cplusplus
}
#endif

#endif