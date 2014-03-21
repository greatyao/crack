#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__


//#include <windows.h>
#include <stdio.h>

#include <math.h>

#include <string>
using namespace std;

#include "bigInt.h"

//切割算法
class CSplitChars
{
	unsigned m_len_min;//最小长度
	unsigned m_len_max;//最大长度

	string m_chars_set;//字符集
	char *m_pchars_set;//字符集指针
	unsigned m_nNumChars;//字符集字符数量

	BigInt::Rossi m_total_nums;	//密码空间总量
	BigInt::Rossi m_split_sub;	//一份密码的数量
	unsigned m_nSplit;//切割份数

public:
	//构造函数，初始化字符集
	CSplitChars();

	//初始化数据
	//s_chars_set 字符集
	// len_min    最小长度
	// len_max    最大长度
	// nSplit     切割份数
	// 初始化成功，返回TRUE
	// 如果长度，字符集等存在问题，返回FALSE
	BOOL Init(string s_chars_set,unsigned len_min,unsigned len_max,unsigned nSplit);
	
	//取一块数据范围
	// n 表示分块序号，从1开始计数
	// s_start 数据块的开始字符串
	// s_end 数据块的结束字符串
	// 出错，返回0
	UINT Split(unsigned n,string &s_start,string &s_end);

private:
	//n 字节长度的密码空间 = z
	void fun_calculate(unsigned len_chars_set,unsigned n,BigInt::Rossi &z);
	
	//x的y次方 = z
	void fun_pow(BigInt::Rossi &x,unsigned y,BigInt::Rossi &z);

	//数值x转字符串s_str
	void fun_value2string(BigInt::Rossi &x,string &s_str);	
};



#endif