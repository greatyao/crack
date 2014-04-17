#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
#include "algorithm_types.h"

using namespace std;

//�и��㷨
class csplit
{
private:
	double compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);
	
	string make_character_table(enum crack_charset);//�����ַ�������
	bool init_bf(unsigned len_min,unsigned len_max,char *character_set);//��ʼ��
		
	struct crack_block *split_mask(struct crack_task *pct,unsigned &nsplits);
	struct crack_block *split_dic(struct crack_task *pct,unsigned &nsplits);
public:
	csplit();
	~csplit();
public:
	//nsplits�����и��Ժ�ķ���
	//����ָ������,��Ҫ�ֹ��ͷ�
	struct crack_block *split_easy(struct crack_task *pct,unsigned &nsplits);
	struct crack_block *split_intelligent(struct crack_task *pct,unsigned &nsplits);

	struct crack_block *split_task(struct crack_task *pct,unsigned &nsplits);

	void release_splits(char *p);//�ͷ�

};



#endif