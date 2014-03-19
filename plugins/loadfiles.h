#ifndef LOADHASHES_H
#define LOADHASHES_H

#ifdef __cplusplus
extern "C"{
#endif

//HASH�ַ������ļ���д��������˵���

struct crack_hash;

//��hash�ļ��ж�ȡhash�ַ���������hashesΪ��Ž����countΪhashes�������С
//���������Ч��hash����������󲻳���count�����򷵻ظ���
int load_hashes_file(const char *filename, int algo, struct crack_hash* hashes, int count);

//����hash���Ķ�д
//�ɹ�����1�����򷵻ظ���
int load_single_hash(char *hash, int algo, struct crack_hash* hashes);

#ifdef __cplusplus
}
#endif

#endif
