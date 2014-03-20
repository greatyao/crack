
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

//��ʼ������
//s_chars_set �ַ���
// len_min    ��С����
// len_max    ��󳤶�
// nSplit     �и����
// ��ʼ���ɹ�������TRUE
// ������ȣ��ַ����ȴ������⣬����FALSE
BOOL CSplitChars::Init(string s_chars_set,unsigned len_min,unsigned len_max,unsigned nSplit)
{
	//��С���Ȳ���Ϊ0
	if(len_min<1) return FALSE;
	if(len_max<len_min) return FALSE;
	if(s_chars_set.length()<1) return FALSE;
	if(nSplit<1)return FALSE;

	//��ʼ��ʼ��
	m_len_min = len_min;
	m_len_max = len_max;

	m_chars_set = s_chars_set;
	m_pchars_set = (char *)m_chars_set.c_str(); 
	m_nNumChars = m_chars_set.length();

	m_nSplit = nSplit;


	//�����������������һ������
	BigInt::Rossi total_max;
	BigInt::Rossi total_min(0);
	fun_calculate(m_nNumChars,m_len_max,total_max);
	if(m_len_min>1)  fun_calculate(m_nNumChars,m_len_min-1,total_min);

	//����ռ�����
	m_total_nums = total_max - total_min;
	#ifdef _DEBUG
	printf("����ռ� %s\n",m_total_nums.toStrDec().c_str());
	#endif

	//ƽ��ֵ
	BigInt::Rossi sp(m_nSplit);
	m_split_sub = m_total_nums/sp;
	#ifdef _DEBUG
	printf("�и��Ժ�ÿ�����������Լ %s\n",m_split_sub.toStrDec().c_str());
	#endif
	//

	//�и�
	return TRUE;
}	

UINT CSplitChars::Split(unsigned n,string &s_start,string &s_end)
{	
	//����Ǵ�0����1��ʼ
	BigInt::Rossi m_base_pwd(0);//0?
	if(m_len_min>1){
		fun_calculate(m_nNumChars,m_len_min-1,m_base_pwd);
		m_base_pwd = m_base_pwd+1;
	}
	
	#ifdef _DEBUG
	printf("��ʼ�ַ�λ�� %s\n",m_base_pwd.toStrDec().c_str());
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

	//��ֵת�ַ���
	fun_value2string(startVal,s_start);
	fun_value2string(endVal,s_end);
	
	return 1;
}

//n �ֽڳ��ȵ�����ռ� = z
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

//x��y�η� = z
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
	BigInt::Rossi nRadix(m_nNumChars);//�ַ�����
	BigInt::Rossi nBit;//���ݳ���
	BigInt::Rossi nDummy(0);//��ʱ����

	#ifdef _DEBUG
	//printf("fun_value2string %s\n",x.toStrDec().c_str());
	//printf("�ַ��� %s\n",nRadix.toStrDec().c_str());
	#endif

	unsigned vBit=1;//��������λ��
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

	//����
	s_str = "";
	char Buffer[100]={0};
	memset(Buffer,m_pchars_set[0],vBit);
	int k=0;
	do
	{
		//����һλ
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
		int m_nMaxLength;	//������󳤶�
		int m_nPasswordMin;	//��С���볤��
		int m_nNumCharacters;//�ַ�����
		string m_BruteForceChars;//�����ֵ�
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