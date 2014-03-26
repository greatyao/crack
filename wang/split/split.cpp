#include "split.h"

#include <Objbase.h>
#pragma comment(lib,"Rpcrt4.lib")

const unsigned long split_combinations = 0xFFFFFFFF;
const unsigned long split_multiple     = 500;
const unsigned long max_password_length= 20; 

/*
const char *g_charset_table[charset_custom+1]={
	"0123456789",
	"abcdefghijklmnopqrstuvwxyz",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"abcdefghijklmnopqrstuvwxyz0123456789",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
	"675abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~!@#$%^&*()_+{}|\":?><-=[]\\';/.",
	""
};
*/

#define Big_Int BigInt::Rossi

void debug(Big_Int d)
{
	#ifdef _DEBUG
	string s2 = d.toStrDec();
	printf("%s\n",s2.c_str());
	#endif
}

csplit::csplit()
{
	Big_Int zero(0);
	m_zero = zero;
	Big_Int one(1);
	m_one  = one;
}
csplit::~csplit()
{
}

string csplit::new_guid(void)
{
	UUID uuid;
	UuidCreate( &uuid);

	unsigned char * str;
	UuidToStringA( &uuid, &str);

	string s( (char*)str );
	RpcStringFreeA( &str );

	return s;
}

Big_Int csplit::compute_combinations(unsigned characters,unsigned len_max,unsigned len_min)
{
	if( (len_max<len_min)||(len_max>max_password_length)||(characters<1) )
	{
		return m_zero;
	}

	Big_Int total_combinations = m_zero;

	for(unsigned i=len_min; i<=len_max; i++)
	{
		Big_Int k(characters);
		Big_Int counter(1);
		for(unsigned j=0; j<i; j++)
		{
			counter = counter *k;
		}
		total_combinations = total_combinations+counter;
	}

	return total_combinations;
}

Big_Int csplit::compute_power(Big_Int x,unsigned y)
{
	Big_Int total(1);
	for(unsigned i=0; i<y; i++)
	{
		total = total*x;
	}
	return total;
}

Big_Int csplit::compute_power(unsigned x,unsigned y)
{
	Big_Int total(1);
	Big_Int b_x(x);
	for(unsigned i=0; i<y; i++)
	{
		total = total*b_x;
	}
	return total;
}
//1 base
Big_Int csplit::string_to_integer(const string & s_charset,const string &password)
{
	const char *p_pwd = password.c_str();
	size_t len_pwd = password.length();

	Big_Int v(0);
	Big_Int base(s_charset.length());

	string s_character_set = s_charset;
	for(size_t i=0; i<len_pwd; i++)
	{
		size_t pos = s_character_set.find(p_pwd[i]);
		Big_Int bit(pos);
		if(i==(len_pwd-1))
			v = v + (bit);
		else
			v = v + (bit)*compute_power(base,len_pwd-i-1);		
	}

	return v+compute_combinations(s_charset.length(),len_pwd-1);
}

string csplit::integer_to_string(const string & s_charset,Big_Int integer)
{
	Big_Int table_base[max_password_length];//4 20 84 340 1364...... 
 
	Big_Int base(s_charset.length());
	string s;						
	const char *m_character_set = s_charset.c_str();

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

	if(bits>1) integer = integer - compute_combinations(s_charset.length(),bits-1);

	for(int i=bits; i>0; i--)
	{
		Big_Int byte = integer/compute_power(base,i-1);
		unsigned long ub = byte.toUlong();
		s.append(1,m_character_set[ub]);

		integer = integer - compute_power(base,i-1)*byte;
	}

	return s;
}
Big_Int csplit::get_step_length(const string & s_charset,unsigned len_max)
{
	if(s_charset.length()==0)
	{
		return m_zero;
	}

	Big_Int r(0);
	for(unsigned i=0; i<len_max; i++)
	{
		r=r+compute_power(s_charset.length(),i);
	}
	return r;
}

string csplit::make_character_table(enum crack_charset k)
{
	string s;

	if(k<charset_custom)
	{
		s = crack_charsets[k];
	}
	else{
	}	
	return s;
}
/*
bool csplit::init_bf(unsigned len_min,unsigned len_max,char *character_set)
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
*/
//默认根据预定义密码数量切割算法
struct crack_block *csplit::split_default(struct crack_task *pct,unsigned &nsplits)
{	
	if( (pct==0)||(pct->count<1) ) 
	{
		return 0;
	}

	struct crack_task * loc_p_ct = pct;
	string loc_s_charset;
	Big_Int loc_total_combinations;

	loc_s_charset = make_character_table((crack_charset)loc_p_ct->charset);
	loc_total_combinations = compute_combinations(loc_s_charset.length(),loc_p_ct->endLength,loc_p_ct->startLength);


	#ifdef _DEBUG
	double pp = 0;
	for(unsigned i = loc_p_ct->startLength; i <=loc_p_ct->endLength; i++)
		pp += pow(loc_s_charset.length(), (double)i);
	printf("%g\n", pp);
	#endif


	Big_Int big_split_combinations(split_combinations);
	Big_Int big_split_multiple(split_multiple);
	Big_Int big_nsplits(1);

	//计算估算的每份切割的密码量
	big_split_combinations = big_split_combinations * big_split_multiple;	//切割每份大小

	#ifdef _DEBUG
	printf("total:%s\n",loc_total_combinations.toStrDec());
	#endif
	//get_step_length
	//if(big_split_combinations>)//不切

	//简单字符集模式下最大能切割的份数
	unsigned max_splits = loc_s_charset.length();
	Big_Int bit_max_splits(max_splits);

	//简单模式切割，每份切割的密码数量
	Big_Int one_split = loc_total_combinations/bit_max_splits;

	//确定每份实际大小
	unsigned step = 0;
	for(unsigned i=1; i<=max_splits; i++)
	{
		Big_Int temp(i);
		if( (one_split*temp)>big_split_combinations )
		{
			one_split = one_split *temp;//合适的每份切割的密码数据量
			step = i;					//
			break;
		}
	}

	//确定最终切割份数
	if(step==0)
	{
		nsplits = 1;
	}
	else
	{
		Big_Int temp = loc_total_combinations/one_split;

		if( temp > bit_max_splits)
		{
			nsplits	= max_splits;
		}
		else
		{
			nsplits = temp.toUlong();
		}
		
		//nsplits	= max_splits/step;
		//if( (nsplits*step)!=max_splits ) nsplits++;
	}

	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*nsplits*loc_p_ct->count);

	for(unsigned i=0; i<nsplits; i++)
	{
		p_crack_block[i].algo    = loc_p_ct->algo;//算法
		p_crack_block[i].charset = loc_p_ct->charset;//字符集
		p_crack_block[i].type   = loc_p_ct->type;
		p_crack_block[i].special= loc_p_ct->special;

		string s_guid =new_guid();

		memcpy( p_crack_block[i].guid, s_guid.c_str(), s_guid.length()+1 );
		memcpy( p_crack_block[i].john, loc_p_ct->hashes[0].hash, sizeof(struct crack_hash) );
		
		if(i==0)//第一个
		{
			p_crack_block[i].start = loc_p_ct->startLength;
			p_crack_block[i].start2 = 0;//索引
		}
		else
		{
			p_crack_block[i].start = loc_p_ct->endLength;
			p_crack_block[i].start2 = (i)*(loc_s_charset.length()/nsplits);
		}
		p_crack_block[i].end = loc_p_ct->endLength;

		//索引2
		if( (i+1)==nsplits)//最后一个
		{
			p_crack_block[i].end2 = loc_s_charset.length()-1;
		}
		else
		{
			p_crack_block[i].end2 = (i+1)*(loc_s_charset.length()/nsplits);
		}
	}

	if(loc_p_ct->count>1)//多个
	{
		for(int i=1; i<loc_p_ct->count; i++)
		{
			memcpy( &p_crack_block[nsplits*i],p_crack_block, sizeof(struct crack_block)*nsplits);
			for(unsigned j=0; j<nsplits; j++)
			{
				memcpy( p_crack_block[nsplits*i+j].john, loc_p_ct->hashes[i].hash, sizeof(struct crack_hash) );

				string s_guid =new_guid();
				memcpy( p_crack_block[nsplits*i+j].guid,  s_guid.c_str(), s_guid.length()+1);
			}
		}		
	}

	nsplits = nsplits*loc_p_ct->count;
	return p_crack_block;
}

//简单切割
struct crack_block *csplit::split_easy(struct crack_task *pct,unsigned &nsplits)
{
	if( (pct==0)||(pct->count<1) ) return 0;

	struct crack_task * loc_p_ct = pct;
	string loc_s_charset;
	Big_Int loc_total_combinations;

	loc_s_charset = make_character_table((crack_charset)loc_p_ct->charset);
	loc_total_combinations = compute_combinations(loc_s_charset.length(),loc_p_ct->endLength,loc_p_ct->startLength);

	//根据密码长度范围简单估算
	const int k_pos = 10;
	if( (loc_p_ct->startLength == loc_p_ct->endLength)||( loc_p_ct->endLength<=k_pos ))
		nsplits = 1;
	else if(loc_p_ct->startLength<=10)
	{
		nsplits =  1+loc_p_ct->endLength-10;
	}
	else
	{
		nsplits = loc_p_ct->endLength-10;			
	}

	//分配数据空间
	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*nsplits*loc_p_ct->count);

	for(unsigned i=0; i<nsplits; i++)
	{
		p_crack_block[i].algo    = loc_p_ct->algo;//算法
		p_crack_block[i].charset = loc_p_ct->charset;//字符集
		p_crack_block[i].type   = loc_p_ct->type;
		p_crack_block[i].special= loc_p_ct->special;

		string s_guid =new_guid();

		memcpy( p_crack_block[i].guid, s_guid.c_str(), s_guid.length()+1 );
		memcpy( p_crack_block[i].john, loc_p_ct->hashes[0].hash, sizeof(struct crack_hash) );
		
		//按照密码长度切分
		if(i==0)//第一个
		{
			p_crack_block[i].start = loc_p_ct->startLength;
			p_crack_block[i].start2 = 0;//索引
		}
		else
		{
			p_crack_block[i].start = k_pos+i;
			p_crack_block[i].start2 = 0;
		}
		p_crack_block[i].end = k_pos+i;

		//索引2
		if( (i+1)==nsplits)//最后一个
		{
			p_crack_block[i].end2 = loc_s_charset.length()-1;
		}
		else
		{
			p_crack_block[i].end2 = loc_s_charset.length()-1;
		}
	}

	if(loc_p_ct->count>1)//多个
	{
		for(int i=1; i<loc_p_ct->count; i++)
		{
			memcpy( &p_crack_block[nsplits*i],p_crack_block, sizeof(struct crack_block)*nsplits);
			for(unsigned j=0; j<nsplits; j++)
			{
				memcpy( p_crack_block[nsplits*i+j].john, loc_p_ct->hashes[i].hash, sizeof(struct crack_hash) );
				string s_guid =new_guid();
				memcpy( p_crack_block[nsplits*i+j].guid,  s_guid.c_str(), s_guid.length()+1);
			}
		}		
	}

	nsplits = nsplits*loc_p_ct->count;
	return p_crack_block;
}

//根据hash速度切割
struct crack_block *csplit::split_normal(struct crack_task *pct,unsigned &nsplits)
{
	return 0;
}

void csplit::release_splits(char *p)
{
	if(p)
	{
		free(p);
	}
}