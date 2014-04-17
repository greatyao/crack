#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;

#include "bigInt.h"
#include "algorithm_types.h"

//�и��㷨
class csplit
{
	BigInt::Rossi m_zero;	//0
	BigInt::Rossi m_one;	//1

private:
	BigInt::Rossi compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);
	BigInt::Rossi compute_power(BigInt::Rossi x,unsigned y);
	BigInt::Rossi compute_power(unsigned x,unsigned y);

	BigInt::Rossi string_to_integer(const string & s_charset,const string &password);
	string integer_to_string(const string & s_charset,BigInt::Rossi );

	BigInt::Rossi get_step_length(const string & s_charset,unsigned );
	
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