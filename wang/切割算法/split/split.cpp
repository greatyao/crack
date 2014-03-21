#include "split.h"

const unsigned long split_combinations = 0xFFFFFFFF;
const unsigned long split_multiple     = 500;
const unsigned long max_password_length= 32; 

#define Big_Int BigInt::Rossi

void debug(BigInt::Rossi d)
{
	#ifdef _DEBUG
	string s2 = d.toStrDec();
	printf("%s\n",s2.c_str());
	#endif
}

csplit::csplit()
{
	m_characters = 0;
	BigInt::Rossi zero(0);
	m_zero = zero;
	BigInt::Rossi one(1);
	m_one  = one;
}
csplit::~csplit()
{
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

BigInt::Rossi csplit::compute_power(BigInt::Rossi x,unsigned y)
{
	BigInt::Rossi total(1);
	for(unsigned i=0; i<y; i++)
	{
		total = total*x;
	}
	return total;
}
//1 base
BigInt::Rossi csplit::string_to_integer(string password)
{
	const char *p_pwd = password.c_str();
	size_t len_pwd = password.length();

	BigInt::Rossi v(0);
	BigInt::Rossi base(m_characters);

	string s_character_set = m_character_set;
	for(size_t i=0; i<len_pwd; i++)
	{
		size_t pos = s_character_set.find(p_pwd[i]);
		BigInt::Rossi bit(pos);
		if(i==(len_pwd-1))
			v = v + (bit);
		else
			v = v + (bit)*compute_power(base,len_pwd-i-1);		
	}

	return v+compute_combinations(m_characters,len_pwd-1);
}

string csplit::integer_to_string(BigInt::Rossi integer)
{
	BigInt::Rossi table_base[max_password_length];//4 20 84 340 1364...... 
 
	BigInt::Rossi base(m_characters);
	string s;						

	//��ʼ��
	table_base[0] = base;
	for(int i=1; i<max_password_length; i++)
	{
		table_base[i] = table_base[i-1]+compute_power(base,i+1);
	}

	//ȷ��λ��
	int bits=1;
	if(integer>=base)
	for(int i=1; i<max_password_length; i++)
	{
		if( (integer>=table_base[i-1])&&(integer<table_base[i]) )
		{
			bits = i+1;
			break;
		}
	}

	if(bits>1) integer = integer - compute_combinations(m_characters,bits-1);

	for(int i=bits; i>0; i--)
	{
		BigInt::Rossi byte = integer/compute_power(base,i-1);
		unsigned long ub = byte.toUlong();
		s.append(1,m_character_set[ub]);

		integer = integer - compute_power(base,i-1)*byte;
	}

	return s;
}

string csplit::make_character_table(enum crack_charset k)
{
	const char *p[charset_custom+1]={
		"0123456789",
		"abcdefghijklmnopqrstuvwxyz",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
		"abcdefghijklmnopqrstuvwxyz0123456789",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$%^&*()_+{}|\":?><-=[]\\';/.",
		""
	};
	string s;

	if(k<charset_custom)
	{
		s = p[k];
	}
	else{
		throw("error");
	}	
	return s;
}

bool csplit::init_bf(unsigned len_min,unsigned len_max,char *character_set)
{
	if( (len_min<1)||(len_min>len_max)||(character_set==0)||(character_set[0]==0)||(len_max>max_password_length) )
	{
		throw("error");
		//return false;
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
	printf("��ǰ���������:\n%s\n",s.c_str());
	#endif

	return true;
}

bool csplit::init(struct crack_task *pct)
{
	if(pct==0) return false;

	memcpy(&m_crack_task,pct,sizeof(m_crack_task));
	string s_char_set = make_character_table((crack_charset)m_crack_task.charset);
	return init_bf(m_crack_task.len_min,m_crack_task.len_max,(char*)s_char_set.c_str());
}

//�и�
struct crack_block *csplit::split_default(unsigned &nsplits)
{	
	Big_Int big_split_combinations(split_combinations);
	Big_Int big_split_multiple(split_multiple);
	Big_Int big_nsplits(0);

	big_split_combinations = big_split_combinations * big_split_multiple;
	big_nsplits = m_total_combinations/big_split_combinations;

	nsplits = big_nsplits.toUlong();


	struct crack_block *p_crack_block = (crack_block *)new char [sizeof(struct crack_block)*nsplits];
	
	for(unsigned i=0; i<nsplits; i++)
	{
		p_crack_block[i].algo = m_crack_task.algo;//�㷨
		p_crack_block[i].charset = m_crack_task.charset;//�ַ���
		p_crack_block[i].type = m_crack_task.type;
		p_crack_block[i].special = m_crack_task.special;
		memcpy( p_crack_block[i].guid, m_crack_task.guid, sizeof(p_crack_block[i].guid) );
		//memcpy(p_crack_block[i].john,m_crack_task.hashes,sizeof(crack_block.john));

		//unsigned short start;	//��ʼ����
		//unsigned short end;		//��������
		//��������������
		//unsigned short start2;	//55555-99999:start2=5,end2=9	000-55555:start2=0,end2=5
		//unsigned short end2;

	}



	return p_crack_block;
}