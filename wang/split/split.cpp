#include "split.h"
#include "guidgenerator.h"

const unsigned long split_combinations = 0xFFFFFFFF;
const unsigned long split_multiple     = 500;
const unsigned long max_password_length= 20; 

#define Big_Int BigInt::Rossi


//数值表示每秒能跑的兆密码量
//最小为1
//如果是0，则不切割
unsigned long speed_algorithm[]=
{
	300,//切割基准, 300表示切割以后的每份数据大致需要300秒计算完成
	2554,//algo_md4=0x01,        //MD4 plugin
	1284,//algo_md5,             //MD5 plugin
	361,//algo_md5md5,          //md5(md5(pass)) plugin
	1,//algo_md5unix,         //MD5(Unix) plugin (shadow files)
	1,//algo_mediawiki,       //md5(salt.'-'.md5(password)) plugin (Wikimedia)
	702,//algo_oscommerce,      //md5(salt,password) plugin (osCommerce)
	1,//algo_ipb2,            //md5(md5(salt).md5(pass)) plugin (IPB > 2.x)
	1282,//algo_joomla,          //md5(password,salt) plugin (joomla)
	1,//algo_vbulletin,       //md5(md5(pass).salt) plugin
	14,//algo_desunix,         //DES(Unix) plugin (.htpasswd)
	400,//algo_sha1,            //SHA1 plugin
	1,//algo_sha1sha1,        //sha1(sha1(pass)) plugin
	162,//algo_sha256,          //SHA-256 plugin
	1,//algo_sha256unix,      //SHA256(Unix) plugin (shadow files)
	9,//algo_sha512,          //SHA-512 plugin
	1,//algo_sha512unix,      //SHA512(Unix) plugin (shadow files)
	1,//algo_msoffice_old,    //MS Office XP/2003 plugin
	1,//algo_msoffice,        //MS Office 2007/2010/2013 plugin
	1,//algo_django256,       //Django SHA-256 plugin
	22,//algo_zip,             //ZIP passwords plugin
	1,//algo_rar,             //RAR3 passwords plugin
	1,//algo_apr1,            //Apache apr1 plugin
	1,//algo_bfunix,          //bfunix plugin (shadow files)
	1,//algo_dmg,             //FileVault (v1)  passwords plugin
	1,//algo_drupal7,         //Drupal >=7 hashes plugin
	201,//algo_lm,              //LM plugin
	1,//algo_luks,            //LUKS encrypted block device plugin
	670,//algo_mscash,          //Domain cached credentials plugin
	1,//algo_mscash2,         //Domain cached credentials v2 plugin
	396,//algo_mssql_2000,      //Microsoft SQL Server 2000 plugin
	397,//algo_mssql_2005,      //Microsoft SQL Server 2005 plugin
	9,//algo_mssql_2012,      //Microsoft SQL Server 2012 plugin
	188,//algo_mysql5,          //MySQL > 4.1 plugin
	400,//algo_nsldap,          //LDAP SHA plugin
	397,//algo_nsldaps,         //LDAP SSHA (salted SHA) plugin
	2441,//algo_ntlm,            //NTLM plugin
	1,//algo_o5logon,         //Oracle TNS O5logon
	56,//algo_oracle_old,      //Oracle 7 up to 10r2 plugin
	397,//algo_oracle11g,       //Oracle 11g plugin
	266,//algo_osx_old,         //MacOSX <= 10.6 system passwords plugin
	8,//algo_osxlion,         //MacOSX Lion system passwords plugin
	1,//algo_phpbb3,          //phpBB3 hashes plugin
	861,//algo_pixmd5,          //Cisco PIX password hashes plugin
	1,//algo_privkey,         //SSH/SSL private key passphrase plugin
	261,//algo_ripemd160,       //RIPEMD-160 plugin
	1,//algo_sapg,            //SAP CODVN G passwords plugin
	1,//algo_sl3,             //Nokia SL3 plugin
	266,//algo_smf,             //SMF plugin
	1,//algo_wordpress,       //Wordpress hashes plugin
	1,//algo_wpa,             //WPA-PSK plugin
};


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

//默认根据预定义密码数量切割算法
struct crack_block *csplit::split_default(struct crack_task *pct,unsigned &nsplits)
{	
	if( (pct==0)||(pct->count<1) ) 
	{
		return 0;
	}

	
	if((pct->type==mask))
	{
		return split_mask(pct,nsplits);
	}
	else if((pct->type==dict))
	{
		return split_dic(pct,nsplits);
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
	if( (step==0)  )
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
		p_crack_block[i].hash_idx = 0;

		new_guid(p_crack_block[i].guid, sizeof(p_crack_block[i].guid));

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

				new_guid( p_crack_block[nsplits*i+j].guid,  sizeof(p_crack_block[nsplits*i+j].guid));
				p_crack_block[nsplits*i+j].hash_idx = i;
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

	
	if((pct->type==mask))
	{
		return split_mask(pct,nsplits);
	}
	else if((pct->type==dict))
	{
		return split_dic(pct,nsplits);
	}

	struct crack_task * loc_p_ct = pct;
	string loc_s_charset;
	Big_Int loc_total_combinations;

	loc_s_charset = make_character_table((crack_charset)loc_p_ct->charset);
	loc_total_combinations = compute_combinations(loc_s_charset.length(),loc_p_ct->endLength,loc_p_ct->startLength);

	//根据密码长度范围简单估算
	const int k_pos = 10;
	if( (loc_p_ct->startLength == loc_p_ct->endLength)||( loc_p_ct->endLength<=k_pos ))
	{
		nsplits = 1;
	}
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
		p_crack_block[i].hash_idx = 0;

		new_guid( p_crack_block[i].guid, sizeof(p_crack_block[i].guid) );
		memcpy( p_crack_block[i].john, loc_p_ct->hashes[0].hash, sizeof(struct crack_hash) );
		
		//按照密码长度切分
		if(i==0)//第一个
		{
			p_crack_block[i].start = loc_p_ct->startLength;
			//p_crack_block[i].start2 = 0;//索引
			p_crack_block[i].start2 = -1;
		}
		else
		{
			p_crack_block[i].start = k_pos+i;
			//p_crack_block[i].start2 = 0;
			p_crack_block[i].start2 = -1;
		}
		if(k_pos>loc_p_ct->endLength)
			p_crack_block[i].end = loc_p_ct->endLength;
		else
		p_crack_block[i].end = k_pos+i;

		//索引2
		if( (i+1)==nsplits)//最后一个
		{
			//p_crack_block[i].end2 = loc_s_charset.length()-1;
			p_crack_block[i].end2 = -1;//
		}
		else
		{
			//p_crack_block[i].end2 = loc_s_charset.length()-1;
			p_crack_block[i].end2 = -1;//
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
				new_guid( p_crack_block[nsplits*i+j].guid,  sizeof(p_crack_block[nsplits*i+j].guid));
				p_crack_block[nsplits*i+j].hash_idx = i;
			}
		}		
	}

	nsplits = nsplits*loc_p_ct->count;
	return p_crack_block;
}

//根据hash速度切割
struct crack_block *csplit::split_normal(struct crack_task *pct,unsigned &nsplits)
{	
	if( (pct==0)||(pct->count<1) ) 
	{
		return 0;
	}
	
	if((pct->type==mask))
	{
		return split_mask(pct,nsplits);
	}
	else if((pct->type==dict))
	{
		return split_dic(pct,nsplits);
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


	Big_Int big_split_combinations(speed_algorithm[loc_p_ct->algo]);
	Big_Int big_split_multiple(1000000*speed_algorithm[0]);
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
	if( (step==0) )
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

	if(m_zero==big_split_combinations)
		nsplits = 1;

	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*nsplits*loc_p_ct->count);

	for(unsigned i=0; i<nsplits; i++)
	{
		p_crack_block[i].algo    = loc_p_ct->algo;//算法
		p_crack_block[i].charset = loc_p_ct->charset;//字符集
		p_crack_block[i].type   = loc_p_ct->type;
		p_crack_block[i].special= loc_p_ct->special;
		p_crack_block[i].hash_idx = 0;

		new_guid( p_crack_block[i].guid, sizeof(p_crack_block[i].guid) );
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

				new_guid( p_crack_block[nsplits*i+j].guid,  sizeof(p_crack_block[nsplits*i+j].guid));
				p_crack_block[nsplits*i+j].hash_idx = i;
			}
		}		
	}

	nsplits = nsplits*loc_p_ct->count;
	return p_crack_block;
}


//掩码切割
struct crack_block *csplit::split_mask(struct crack_task *pct,unsigned &nsplits)
{
	nsplits = 1;
	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*pct->count);

	p_crack_block[0].algo    = pct->algo;//算法
	p_crack_block[0].charset = pct->charset;//字符集
	p_crack_block[0].type    = pct->type;
	p_crack_block[0].special = pct->special;
	p_crack_block[0].hash_idx= 0;

	new_guid( p_crack_block[0].guid, sizeof(p_crack_block[0].guid) );
	memcpy( p_crack_block[0].john, pct->hashes[0].hash, sizeof(struct crack_hash) );
		
	p_crack_block[0].maskLength = pct->maskLength;
	for(int i=0; p_crack_block[0].masks[i]!=0; i++)
	{
		p_crack_block[0].masks[i] = pct->masks[i];
		if(	p_crack_block[0].masks[i] =='?')
			p_crack_block[0].masks[i]  = -1;
	}


	if(pct->count>1)//多个
	{
		for(int j=1; j<pct->count; j++)
		{
			memcpy( &p_crack_block[nsplits*j],p_crack_block, sizeof(struct crack_block));

			memcpy( p_crack_block[nsplits*j].john, pct->hashes[j].hash, sizeof(struct crack_hash) );
			new_guid( p_crack_block[nsplits*j].guid,  sizeof(p_crack_block[0].guid));
			p_crack_block[nsplits*j].hash_idx = j;
	
			for(int i=0; i<18; i++)
			{
				p_crack_block[0].masks[i] = pct->masks[i];
				if(	p_crack_block[0].masks[i] =='?')
					p_crack_block[0].masks[i]  = -1;
			}
		}		
	}

	return p_crack_block;
}

struct crack_block *csplit::split_dic(struct crack_task *pct,unsigned &nsplits)
{
	nsplits = 1;
	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*pct->count);

	p_crack_block[0].algo    = pct->algo;//算法
	p_crack_block[0].charset = pct->charset;//字符集
	p_crack_block[0].type    = pct->type;
	p_crack_block[0].special = pct->special;
	p_crack_block[0].hash_idx= 0;

	new_guid( p_crack_block[0].guid, sizeof(p_crack_block[0].guid) );
	memcpy( p_crack_block[0].john, pct->hashes[0].hash, sizeof(struct crack_hash) );
		
	p_crack_block[0].dict_idx = pct->dict_idx;
	if(pct->count>1)//多个
	{
		for(int j=1; j<pct->count; j++)
		{
			memcpy( &p_crack_block[nsplits*j],p_crack_block, sizeof(struct crack_block));

			memcpy( p_crack_block[nsplits*j].john, pct->hashes[j].hash, sizeof(struct crack_hash) );
			new_guid( p_crack_block[nsplits*j].guid,  sizeof(p_crack_block[0].guid));
			p_crack_block[nsplits*j].hash_idx = j;
	
			p_crack_block[j].dict_idx = pct->dict_idx;
		}		
	}

	return p_crack_block;
}

void csplit::release_splits(char *p)
{
	if(p)
	{
		free(p);
	}
}