#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;

#include "bigInt.h"

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

private:
public:
	BigInt::Rossi compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);//����ռ����

	BigInt::Rossi string_to_integer(string password);
	string integer_to_string(BigInt::Rossi );

public:
	csplit();

	bool init(unsigned len_min,unsigned len_max,char *character_set);//��ʼ��
	void run(void);
};



#endif