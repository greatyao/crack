#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;

#include "bigInt.h"
#include "algorithm_types.h"

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

	struct crack_task m_crack_task;

private:
	BigInt::Rossi compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);
	BigInt::Rossi compute_power(BigInt::Rossi x,unsigned y);

	BigInt::Rossi string_to_integer(string password);
	string integer_to_string(BigInt::Rossi );

	string make_character_table(enum crack_charset);//生成字符集数组
	bool init_bf(unsigned len_min,unsigned len_max,char *character_set);//初始化
public:
	csplit();
	~csplit();
public:
	bool init(struct crack_task *);//初始化
	//nsplits保存切割以后的份数
	//返回指针数组,需要手工释放
	struct crack_block *split_default(unsigned &nsplits);

};



#endif