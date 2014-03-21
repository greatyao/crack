#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;

#include "bigInt.h"

//切割算法
class csplit
{
	unsigned m_len_min;//最小长度
	unsigned m_len_max;//最大长度

	char m_character_set[0x100];//字符集
	unsigned m_characters;		//字符数

	BigInt::Rossi m_total_combinations;	//密码空间总量

	BigInt::Rossi m_zero;	//0
	BigInt::Rossi m_one;	//1

private:
public:
	BigInt::Rossi compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);//密码空间计算

	BigInt::Rossi string_to_integer(string password);
	string integer_to_string(BigInt::Rossi );

public:
	csplit();

	bool init(unsigned len_min,unsigned len_max,char *character_set);//初始化
	void run(void);
};



#endif