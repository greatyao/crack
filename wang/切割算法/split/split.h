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
	unsigned m_len_min;//��С����
	unsigned m_len_max;//��󳤶�

	char m_character_set[0x100];//�ַ���
	unsigned m_characters;		//�ַ���

	BigInt::Rossi m_total_combinations;	//����ռ�����

	BigInt::Rossi m_zero;	//0
	BigInt::Rossi m_one;	//1

	struct crack_task m_crack_task;

private:
	BigInt::Rossi compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);
	BigInt::Rossi compute_power(BigInt::Rossi x,unsigned y);

	BigInt::Rossi string_to_integer(string password);
	string integer_to_string(BigInt::Rossi );

	string make_character_table(enum crack_charset);//�����ַ�������
	bool init_bf(unsigned len_min,unsigned len_max,char *character_set);//��ʼ��
public:
	csplit();
	~csplit();
public:
	bool init(struct crack_task *);//��ʼ��
	//nsplits�����и��Ժ�ķ���
	//����ָ������,��Ҫ�ֹ��ͷ�
	struct crack_block *split_default(unsigned &nsplits);

};



#endif