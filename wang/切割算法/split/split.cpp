#include "split.h"

const unsigned long split_combinations = 0xFFFFFFFF;
const unsigned long split_multiple     = 500;
const unsigned long max_password_length= 20; 

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

BigInt::Rossi csplit::compute_power(unsigned x,unsigned y)
{
	BigInt::Rossi total(1);
	BigInt::Rossi b_x(x);
	for(unsigned i=0; i<y; i++)
	{
		total = total*b_x;
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

	//初始化
	table_base[0] = base;
	for(int i=1; i<max_password_length; i++)
	{
		table_base[i] = table_base[i-1]+compute_power(base,i+1);
	}

	//确定位数
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
BigInt::Rossi csplit::get_step_length(unsigned len_max)
{
	if(m_characters==0)
	{
		throw("error");
	}

	Big_Int r(0);
	for(int i=0; i<len_max; i++)
	{
		r=r+compute_power(m_characters,i);
	}
	return r;
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
	printf("当前组合密码数:\n%s\n",s.c_str());
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

//简单切割算法
struct crack_block *csplit::split_default(unsigned &nsplits)
{	
	Big_Int big_split_combinations(split_combinations);
	Big_Int big_split_multiple(split_multiple);
	Big_Int big_nsplits(1);

	big_split_combinations = big_split_combinations * big_split_multiple;	//切割每份大小

	//get_step_length
	//if(big_split_combinations>)//不切

	//最大能切割的份数
	unsigned max_splits = m_characters;
	Big_Int bit_max_splits(max_splits);
	//最大切割份数中平均每份的数量
	Big_Int one_split = m_total_combinations/bit_max_splits;

	//确定每份实际大小
	unsigned step = 0;
	for(unsigned i=1; i<=max_splits; i++)
	{
		Big_Int temp(i);
		if( (one_split*temp)>big_split_combinations )
		{
			one_split = one_split *temp;
			step = i;
			break;
		}
	}

	//确定最终切割份数
	nsplits	= max_splits/step;
	if( (nsplits*step)!=max_splits ) nsplits++;

	struct crack_block *p_crack_block = (crack_block *)new char [sizeof(struct crack_block)*nsplits];

	for(unsigned i=0; i<nsplits; i++)
	{
		p_crack_block[i].algo = m_crack_task.algo;//算法
		p_crack_block[i].charset = m_crack_task.charset;//字符集
		p_crack_block[i].type = m_crack_task.type;
		p_crack_block[i].special = m_crack_task.special;
		memcpy( p_crack_block[i].guid, m_crack_task.guid, sizeof(p_crack_block[i].guid) );
		//memcpy(p_crack_block[i].john,m_crack_task.hashes,sizeof(crack_block.john));
		if(i==0)//第一个
		{
			p_crack_block[i].start = m_crack_task.len_min;
			p_crack_block[i].start2 = 0;//索引
		}
		else
		{
			p_crack_block[i].start = m_crack_task.len_max;
			p_crack_block[i].start2 = (i)*(m_characters/nsplits);
		}
		p_crack_block[i].end = m_crack_task.len_max;

		//索引2
		if( (i+1)==nsplits)//最后一个
		{
			p_crack_block[i].end2 = m_characters;
		}
		else
		{
			p_crack_block[i].end2 = (i+1)*(m_characters/nsplits);
		}
	}

	return p_crack_block;
}

void csplit::release_splits(char *p)
{
	if(p)
	{
		delete p;
	}
}