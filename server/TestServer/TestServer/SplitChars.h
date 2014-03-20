#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__


//#include <windows.h>
#include <stdio.h>

#include <math.h>

#include <string>
using namespace std;

#include "bigInt.h"

//�и��㷨
class CSplitChars
{
	unsigned m_len_min;//��С����
	unsigned m_len_max;//��󳤶�

	string m_chars_set;//�ַ���
	char *m_pchars_set;//�ַ���ָ��
	unsigned m_nNumChars;//�ַ����ַ�����

	BigInt::Rossi m_total_nums;	//����ռ�����
	BigInt::Rossi m_split_sub;	//һ�����������
	unsigned m_nSplit;//�и����

public:
	//���캯������ʼ���ַ���
	CSplitChars();

	//��ʼ������
	//s_chars_set �ַ���
	// len_min    ��С����
	// len_max    ��󳤶�
	// nSplit     �и����
	// ��ʼ���ɹ�������TRUE
	// ������ȣ��ַ����ȴ������⣬����FALSE
	BOOL Init(string s_chars_set,unsigned len_min,unsigned len_max,unsigned nSplit);
	
	//ȡһ�����ݷ�Χ
	// n ��ʾ�ֿ���ţ���1��ʼ����
	// s_start ���ݿ�Ŀ�ʼ�ַ���
	// s_end ���ݿ�Ľ����ַ���
	// ��������0
	UINT Split(unsigned n,string &s_start,string &s_end);

private:
	//n �ֽڳ��ȵ�����ռ� = z
	void fun_calculate(unsigned len_chars_set,unsigned n,BigInt::Rossi &z);
	
	//x��y�η� = z
	void fun_pow(BigInt::Rossi &x,unsigned y,BigInt::Rossi &z);

	//��ֵxת�ַ���s_str
	void fun_value2string(BigInt::Rossi &x,string &s_str);	
};



#endif