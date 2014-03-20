
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#include <math.h>

#include <string>
using namespace std;


#include "bigInt.h"
#include "SplitChars.h"

CSplitChars::CSplitChars()
{
}

//初始化数据
//s_chars_set 字符集
// len_min    最小长度
// len_max    最大长度
// nSplit     切割份数
// 初始化成功，返回TRUE
// 如果长度，字符集等存在问题，返回FALSE
BOOL CSplitChars::Init(string s_chars_set,unsigned len_min,unsigned len_max,unsigned nSplit)
{
	//最小长度不能为0
	if(len_min<1) return FALSE;
	if(len_max<len_min) return FALSE;
	if(s_chars_set.length()<1) return FALSE;
	if(nSplit<1)return FALSE;

	//开始初始化
	m_len_min = len_min;
	m_len_max = len_max;

	m_chars_set = s_chars_set;
	m_pchars_set = (char *)m_chars_set.c_str(); 
	m_nNumChars = m_chars_set.length();

	m_nSplit = nSplit;


	//参数无问题则进行下一步计算
	BigInt::Rossi total_max;
	BigInt::Rossi total_min(0);
	fun_calculate(m_nNumChars,m_len_max,total_max);
	if(m_len_min>1)  fun_calculate(m_nNumChars,m_len_min-1,total_min);

	//密码空间总量
	m_total_nums = total_max - total_min;
	#ifdef _DEBUG
	printf("密码空间 %s\n",m_total_nums.toStrDec().c_str());
	#endif

	//平均值
	BigInt::Rossi sp(m_nSplit);
	m_split_sub = m_total_nums/sp;
	#ifdef _DEBUG
	printf("切割以后每个块的数据量约 %s\n",m_split_sub.toStrDec().c_str());
	#endif
	//

	//切割
	return TRUE;
}	

UINT CSplitChars::Split(unsigned n,string &s_start,string &s_end)
{	
	//序号是从0还是1开始
	BigInt::Rossi m_base_pwd(0);//0?
	if(m_len_min>1){
		fun_calculate(m_nNumChars,m_len_min-1,m_base_pwd);
		m_base_pwd = m_base_pwd+1;
	}
	
	#ifdef _DEBUG
	printf("起始字符位置 %s\n",m_base_pwd.toStrDec().c_str());
	#endif

	if(n>m_nSplit)
		return 0;

	BigInt::Rossi startVal;
	BigInt::Rossi endVal;

	startVal = m_base_pwd+m_split_sub*(n-1);
	endVal   = startVal+m_split_sub;

	if(n>1) startVal++;

	if(n==m_nSplit) 
	{
		BigInt::Rossi temp;fun_calculate(m_nNumChars,m_len_min-1,temp);
		endVal = m_total_nums+temp;
		//endVal --;
	}

	//数值转字符串
	fun_value2string(startVal,s_start);
	fun_value2string(endVal,s_end);
	
	return 1;
}

//n 字节长度的密码空间 = z
void CSplitChars::fun_calculate(unsigned len_chars_set,unsigned n,BigInt::Rossi &z)
{
	BigInt::Rossi dummy(len_chars_set);
	BigInt::Rossi nPow(0);
	z = nPow;
	for(unsigned i=1; i<=n; i++)
	{
		fun_pow(dummy,i,nPow);
		z = z + nPow;			
	}
}

//x的y次方 = z
void CSplitChars::fun_pow(BigInt::Rossi &x,unsigned y,BigInt::Rossi &z)
{
	if(y==0)
	{
		BigInt::Rossi dummy(1);
		z = dummy;
	}
	else{
		z = x;
		for(unsigned i=1; i<y; i++)
		{
			z = z * x;
		}
	}
}

//x -> string
void CSplitChars::fun_value2string(BigInt::Rossi &x,string &s_str)
{
	BigInt::Rossi nRadix(m_nNumChars);//字符数量
	BigInt::Rossi nBit;//数据长度
	BigInt::Rossi nDummy(0);//临时变量

	#ifdef _DEBUG
	//printf("fun_value2string %s\n",x.toStrDec().c_str());
	//printf("字符数 %s\n",nRadix.toStrDec().c_str());
	#endif

	unsigned vBit=1;//计算数据位数
	while(1)
	{
		//x = 15;
		fun_calculate(m_nNumChars,vBit,nDummy);	
		if(nDummy>=x)
			break;	
		vBit++;
	}

	
	BigInt::Rossi xx;
	if(vBit>1)
	{
		fun_calculate(m_nNumChars,vBit-1,nDummy);
		xx = x - nDummy;
		xx--;
	}

	//计算
	s_str = "";
	char Buffer[100]={0};
	memset(Buffer,m_pchars_set[0],vBit);
	int k=0;
	do
	{
		//处理一位
		if(vBit==1) break;

		nBit = (xx) % nRadix;
		unsigned long v = nBit.toUlong();
		//s_str.append(&m_pchars_set[v],1);
		Buffer[vBit-k-1]=m_pchars_set[v];
		k++;

		//nBit = (x/nRadix)%nRadix;
		if(nRadix>xx) break;
		xx = xx/nRadix;		
	}
	while(1);

	s_str = Buffer;
}



class CCharExhaustive
{
	private:
		int m_nMaxLength;	//密码最大长度
		int m_nPasswordMin;	//最小密码长度
		int m_nNumCharacters;//字符数量
		string m_BruteForceChars;//密码字典
		char *m_pBruteForceChars;
		int m_nNumChars;
		int m_BFCharsVector[100];
		
	public:
		//
		CCharExhaustive(string &sBruteForceChars)
		{
			m_BruteForceChars = sBruteForceChars;
			m_nNumCharacters = m_BruteForceChars.length();
			m_pBruteForceChars = (char *)m_BruteForceChars.c_str();
			m_nNumChars = m_BruteForceChars.length();
		}
	
		//
		void InitBFCharsVector(string &sPassword)
		{	
			const char *pPwd = sPassword.c_str();
			m_nPasswordMin = sPassword.length();
			m_nMaxLength = m_nPasswordMin+1;
			for(unsigned i=0; i<m_nPasswordMin; i++)
			{
				char ch = pPwd[i];
				m_BFCharsVector[i]=m_BruteForceChars.find(ch,0);
			}
		}
		//
		void PasswordMinPP(void)
		{
			m_nPasswordMin++;
			m_nMaxLength = m_nPasswordMin+1;
		}
		
		//
		bool GetNextPassword (char * destionation)
		{	
			int index = 0;
			for (int i = 0; i < m_nPasswordMin; i++) 
			{
			destionation[index]=m_pBruteForceChars[m_BFCharsVector[i]];
			index += 1;
			}	
			for (int i = m_nPasswordMin - 1; i >= 0; i--) 
			{
				m_BFCharsVector[i] += 1;
				if( m_BFCharsVector[i] < m_nNumChars) 
				{
					break;
				}
				else 
				{
					m_BFCharsVector[i] = 0;
					if (i == 0)					
					{				
						return false;
					}
				}
			}
			destionation[m_nPasswordMin]=0;
			return true;
		}
	
};