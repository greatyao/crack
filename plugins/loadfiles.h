#ifndef LOADHASHES_H
#define LOADHASHES_H

#ifdef __cplusplus
extern "C"{
#endif

//HASH�ַ������ļ���д��������˵���

struct crack_hash;
struct crack_task;

//��hash�ļ��ж�ȡhash�ַ���������hashesΪ��Ž����countΪhashes�������С
//���������Ч��hash����������󲻳���count�����򷵻ظ���
int load_hashes_file(const char *filename, int algo, struct crack_hash* hashes, int count, int* special = NULL);

int load_hashes_file2(const char *filename, struct crack_task* task);

//����hash���Ķ�д
//�ɹ�����1�����򷵻ظ���
int load_single_hash(char *hash, int algo, struct crack_hash* hashes);
int load_single_hash2(char *hash, struct crack_task* task);

//�ͷ�task�е�crack_hash�ڴ�
int release_hashes_from_load(struct crack_task* task);

#ifdef __cplusplus
}
#endif

#endif
