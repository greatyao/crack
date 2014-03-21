#include "split.h"

const unsigned long split_combinations = 0xFFFFFFFF;
const unsigned long split_multiple     = 500;
const unsigned long max_password_length= 32; 

csplit::csplit()
{
	m_characters = 0;
	BigInt::Rossi zero(0);
	m_zero = zero;
	BigInt::Rossi one(1);
	m_one  = one;
}

BigInt::Rossi csplit::compute_combinations(unsigned characters,unsigned len_max,unsigned len_min)
{
	if( (len_max<len_min)||(len_max>max_password_length)||(characters<1) )
	{
		return m_zero;
	}

	BigInt::Rossi total_combinations = m_zero;

	for(unsigned i=len_min; i<=len_max; i++)
	{
		BigInt::Rossi k(characters);
		BigInt::Rossi counter(1);
		for(unsigned j=0; j<i; j++)
		{
			counter = counter *k;
		}
		total_combinations = total_combinations+counter;
	}

	return total_combinations;
}

BigInt::Rossi csplit::string_to_integer(string password)
{
	const char *p_pwd = password.c_str();
	size_t len_pwd = password.length();

	BigInt::Rossi v(0);

	string s_character_set = m_character_set;
	for(size_t i=0; i<len_pwd; i++)
	{
		size_t pos = s_character_set.find(p_pwd[len_pwd-i-1]);
		BigInt::Rossi bit(pos);
		if(i==0)
			v = v + (bit+1)*1;
		else
			v = v + (bit+1)*compute_combinations(m_characters,i);
	}

	return v;
}

string csplit::integer_to_string(BigInt::Rossi integer)
{
	return "";
}

bool csplit::init(unsigned len_min,unsigned len_max,char *character_set)
{
	if( (len_min<1)||(len_min>len_max)||(character_set==0)||(character_set[0]==0)||(len_max>max_password_length) )
	{
		return false;
	}

	m_len_min = len_min;
	m_len_max = len_max;

	m_characters = 0;
	while(1)
	{
		if( character_set[m_characters]==0) {
			m_character_set[m_characters]=0;
			break;
		}
		m_character_set[m_characters]=character_set[m_characters];
		m_characters++;
	}

	m_total_combinations = compute_combinations(m_characters,m_len_max,m_len_min);

	#ifdef _DEBUG	
	string s = m_total_combinations.toStrDec();
	printf("当前组合密码数:\n%s\n",s.c_str());
	#endif

	return true;
}

//切割
void csplit::run(void)
{
	if(!m_characters) return;
	
	BigInt::Rossi one_combinations(split_combinations);
	BigInt::Rossi one_multiple(split_multiple);

	//切割以后每一份的大小(估算)
	one_combinations = one_combinations * one_multiple;


	//切割
	BigInt::Rossi base(0);
	while( base<m_total_combinations )
	{
		//

	}

}