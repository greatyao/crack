#include "split.h"
#include "guidgenerator.h"

#include <vector>
using namespace std;

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
	200,//algo_mediawiki,       //md5(salt.'-'.md5(password)) plugin (Wikimedia)
	702,//algo_oscommerce,      //md5(salt,password) plugin (osCommerce)
	100,//algo_ipb2,            //md5(md5(salt).md5(pass)) plugin (IPB > 2.x)
	1282,//algo_joomla,          //md5(password,salt) plugin (joomla)
	160,//algo_vbulletin,       //md5(md5(pass).salt) plugin
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
	int len = loc_s_charset.length();

	//根据密码长度范围简单估算
	const int k_pos = 10 - len/20;
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
		}
		else
		{
			p_crack_block[i].start = k_pos+i;
		}
		
		//
		if(nsplits==1)
		{
			p_crack_block[i].end = loc_p_ct->endLength;
		}
		else
		{
		if(k_pos>loc_p_ct->endLength)
			p_crack_block[i].end = loc_p_ct->endLength;
		else
			p_crack_block[i].end = k_pos+i;
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

//掩码切割，切一份
struct crack_block *csplit::split_mask(struct crack_task *pct,unsigned &nsplits)
{
	nsplits = 1;
	struct crack_block *p_crack_block = (crack_block *)malloc(sizeof(struct crack_block)*pct->count);

	p_crack_block[0].algo    = pct->algo;//算法
	p_crack_block[0].charset = pct->charset;//字符集
	p_crack_block[0].type    = pct->type;
	p_crack_block[0].special = pct->special;
	p_crack_block[0].hash_idx= 0;
	p_crack_block[0].flag	 = 0;

	new_guid( p_crack_block[0].guid, sizeof(p_crack_block[0].guid) );
	memcpy( p_crack_block[0].john, pct->hashes[0].hash, sizeof(struct crack_hash) );
		
	p_crack_block[0].maskLength = pct->maskLength;
	memcpy(p_crack_block[0].masks, pct->masks, sizeof(pct->masks));
	for(int i=0; i < sizeof(p_crack_block[0].masks); i++)
	{
		if(	p_crack_block[0].masks[i] == 0)
			break;
		else if(p_crack_block[0].masks[i] =='?')
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
	
			memcpy(p_crack_block[j].masks, pct->masks, sizeof(pct->masks));
			for(int i=0; i<sizeof(p_crack_block[0].masks); i++)
			{
				//p_crack_block[j].masks[i] = pct->masks[i];
				if(	p_crack_block[j].masks[i] == 0)
					break;
				else if(	p_crack_block[j].masks[i] =='?')
					p_crack_block[j].masks[i]  = -1;
			}
		}		
	}

	nsplits = nsplits*pct->count;
	return p_crack_block;
}

//字典切割，默认切一份
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

	nsplits = nsplits*pct->count;
	return p_crack_block;
}

static void helper(const string& s, char* masks, int d, int idx)
{
	int len = s.length();
	//printf("helper %d\n", idx);
	for(int i = d-1; i>=0; i--)
	{
		int shang = idx % len;
		masks[i] = s[shang];
		idx /= len;
	}
}

//智能高级切割
struct crack_block *csplit::split_intelligent(struct crack_task *pct,unsigned &nsplits)
{
	//检测输入错误
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

	//生成字符集字符表
	string s_charsets = make_character_table((crack_charset)pct->charset);

	//保存切割结果
	vector <crack_block> cb_result;

	bool flag = false;//是否第一份切割标记
	int d = 1;
	int factor = 36;
	double one = 3600*1e6 *(double)speed_algorithm[pct->algo];//每份长度(即每小时的运算量)
	int len = s_charsets.length();
	__int64 totalSplit = 1;
	int step = 1;
	int totalStep = len;
	const int MAX_D = (len>=36) ? 3 : 4;
	const int MAX_SPLIT = 1000000;
	nsplits = 0;
	int start = pct->startLength;
	int end = pct->endLength;
	if(speed_algorithm[pct->algo] == 1)
		one *= len;
	one *= factor;
	for(int i = start; i <= end; i++)
	{
		int rLength = end - i + 1;
		double ll = (double)pow((double)len, i);
		if(ll < one)continue;

		
		if(i-1 >= start && (flag == false))
		{
			//第一份，暴力破解
			crack_block m_cb;
			
			m_cb.start = start;
			m_cb.end   = i -1;

			m_cb.type   = bruteforce;
			m_cb.algo   = pct->algo;
			m_cb.charset= pct->charset;
			m_cb.special= pct->special;
			m_cb.hash_idx = 0;

			new_guid( m_cb.guid, sizeof(m_cb.guid) );
			memcpy( m_cb.john, pct->hashes[0].hash, sizeof(struct crack_hash) );


			nsplits++;
			cb_result.push_back(m_cb);
			#ifdef _DEBUG
			printf("第%d份 暴力破解[%d, %d]\n", nsplits, m_cb.start, m_cb.end);
			#endif			
		}
		flag = true;

		if(totalSplit == 1)
		{
			totalSplit = ceil(1.0*ll/one);
			printf("估算%llu\n", totalSplit);
			int k;
			for(k = 1; k <= MAX_D; k++)
			{
				if(pow((double)len, k) >= totalSplit) break;
			}

			if(k == MAX_D+1)
			{
				step = 4;
				d = MAX_D;
				totalStep = pow((double)len, d);
				totalSplit = totalStep/step;
			}
			else
			{
				d = k;
				totalStep = pow((double)len, d);
				step = ceil(1.0*totalStep / totalSplit);
			}
		}
		
		//掩码
		crack_block m_cb = {0};
		m_cb.flag = 1;
		m_cb.type = mask;
		m_cb.maskLength = i;

		m_cb.algo    = pct->algo;//算法
		m_cb.charset = pct->charset;//字符集
		m_cb.special = pct->special;
		m_cb.hash_idx= 0;

		memcpy( m_cb.john, pct->hashes[0].hash, sizeof(struct crack_hash) );
	
		#ifdef _DEBUG
		printf("掩码破解 长度%d, 份数%d 步长%d\n", i, (int)totalSplit, step);
		#endif

		int aStep = 0;
		int aSplit = 0;
		do
		{
			memset(m_cb.masks1, 0, sizeof(m_cb.masks1));
			memset(m_cb.masks2, 0, sizeof(m_cb.masks2));

			//前面的
			if((aStep % len) + step < len)
			{
				helper(s_charsets, m_cb.masks1, d, aStep);
				helper(s_charsets, m_cb.masks2, d, aStep+step-1); 
				aStep += step;
			}
			else
			{
				helper(s_charsets, m_cb.masks1, d, aStep);
				aStep = aStep/len*len+len;
				helper(s_charsets, m_cb.masks2, d, aStep-1); 
			}
			#ifdef _DEBUG
			printf("第%d份:掩码破解 【%s-%s】\n", nsplits+1,  m_cb.masks1, m_cb.masks2);
			#endif

			//将后面的设成？
			int maskLen = i < sizeof(m_cb.masks1) ? i : sizeof(m_cb.masks1);
			for(int k = d; k < maskLen; k++)
			{
				m_cb.masks1[k] = -1;
				m_cb.masks2[k] = -1;
			}

			new_guid( m_cb.guid, sizeof(m_cb.guid) );
			nsplits++;
			cb_result.push_back(m_cb);

			if(nsplits * pct->count > MAX_SPLIT)
				break;

			if(++aSplit >= (int)totalSplit || aStep >= totalStep ) break;
		}while(1);

		if(d < MAX_D){
			totalSplit *= len;
			totalStep *= len;
		}
		d++;
		if(d > MAX_D) d = MAX_D;

	}

	if(flag == false)
	{
		//切割一份
		crack_block m_cb;
		m_cb.type = bruteforce;
		m_cb.start = start;
		m_cb.end = end;

		m_cb.algo   = pct->algo;
		m_cb.charset= pct->charset;
		m_cb.special= pct->special;
		m_cb.hash_idx = 0;

		new_guid( m_cb.guid, sizeof(m_cb.guid) );
		memcpy( m_cb.john, pct->hashes[0].hash, sizeof(struct crack_hash) );
		
		cb_result.push_back(m_cb);
		nsplits++;
	}

	unsigned int size = sizeof(struct crack_block)*nsplits*pct->count;
	printf("cost %dM memory\n", size >> 20);
	struct crack_block *p_crack_block = (crack_block *)malloc(size);
	//vector 写入p_crack_block
	for(size_t i=0; i<cb_result.size(); i++)
	{		
		crack_block m_cb = cb_result[i];
		memcpy(&p_crack_block[i],&m_cb,sizeof(struct crack_block));
	}
	cb_result.clear();

	//count
	if(pct->count>1)//多个
	{
		for(int i=1; i<pct->count; i++)
		{
			memcpy( &p_crack_block[nsplits*i],p_crack_block, sizeof(struct crack_block)*nsplits);
			for(unsigned j=0; j<nsplits; j++)
			{
				memcpy( p_crack_block[nsplits*i+j].john, pct->hashes[i].hash, sizeof(struct crack_hash) );

				new_guid( p_crack_block[nsplits*i+j].guid,  sizeof(p_crack_block[nsplits*i+j].guid));
				p_crack_block[nsplits*i+j].hash_idx = i;
			}
		}		
	}

	nsplits = nsplits*pct->count;
	return p_crack_block;
}


void csplit::release_splits(char *p)
{
	if(p)
	{
		free(p);
	}
}

struct crack_block *csplit::split_task(struct crack_task *pct,unsigned &nsplits)
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

	if(pct->special == 1)
		return split_easy(pct, nsplits);

	return split_intelligent(pct, nsplits);
}