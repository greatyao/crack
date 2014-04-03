#ifndef LOADHASHES_H
#define LOADHASHES_H

#ifdef __cplusplus
extern "C"{
#endif

//HASH字符串的文件读写，供服务端调用

struct crack_hash;
struct crack_task;

//从hash文件中读取hash字符串，其中hashes为存放结果，count为hashes的输入大小
//结果返回有效的hash串个数，最大不超过count，否则返回负数
int load_hashes_file(const char *filename, int algo, struct crack_hash* hashes, int count, int* special = NULL);

int load_hashes_file2(const char *filename, struct crack_task* task);

//单个hash串的读写
//成功返回1，否则返回负数
int load_single_hash(char *hash, int algo, struct crack_hash* hashes);
int load_single_hash2(char *hash, struct crack_task* task);

//释放task中的crack_hash内存
int release_hashes_from_load(struct crack_task* task);

#ifdef __cplusplus
}
#endif

#endif
