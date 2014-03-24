#ifndef ALGORITHM_TYPES_H
#define ALGORITHM_TYPES_H

#define HASHFILE_MAX_PLAIN_LENGTH 128
#define HASHFILE_MAX_LINE_LENGTH 256	

enum crack_charset
{
	charset_num,		//0123456789
	charset_lalpha,		//abcdefghijklmnopqrstuvwxyz
	charset_ualpha,		//ABCDEFGHIJKLMNOPQRSTUVWXYZ
	charset_alpha,		//l_alpha+ualpha
	charset_lalphanum,	//lalpha+num
	charset_ualphanum,	//ualpha+num
	charset_alphanum,	//alpha+num
	charset_ascii,		//alpha+num+~!@#$%^&*()_+{}|":?><-=[]\';/.,
	charset_custom,		//自己定义
};

enum crack_type
{
	bruteforce,
	dict,
	rule,
};

enum crack_algorithm
{
	algo_md4=0x01,        //MD4 plugin
	algo_md5,             //MD5 plugin
	algo_md5md5,          //md5(md5(pass)) plugin
	algo_md5unix,         //MD5(Unix) plugin (shadow files)
	algo_mediawiki,       //md5(salt.'-'.md5(password)) plugin (Wikimedia)
	algo_oscommerce,      //md5(salt,password) plugin (osCommerce)
	algo_ipb2,            //md5(md5(salt).md5(pass)) plugin (IPB > 2.x)
	algo_joomla,          //md5(password,salt) plugin (joomla)
	algo_vbulletin,       //md5(md5(pass).salt) plugin
	algo_desunix,         //DES(Unix) plugin (.htpasswd)
	algo_sha1,            //SHA1 plugin
	algo_sha1sha1,        //sha1(sha1(pass)) plugin
	algo_sha256,          //SHA-256 plugin
	algo_sha256unix,      //SHA256(Unix) plugin (shadow files)
	algo_sha512,          //SHA-512 plugin
	algo_sha512unix,      //SHA512(Unix) plugin (shadow files)
	algo_msoffice_old,    //MS Office XP/2003 plugin
	algo_msoffice,        //MS Office 2007/2010/2013 plugin
	algo_django256,       //Django SHA-256 plugin
	algo_zip,             //ZIP passwords plugin
	algo_rar,             //RAR3 passwords plugin
	algo_apr1,            //Apache apr1 plugin
	algo_bfunix,          //bfunix plugin (shadow files)
	algo_dmg,             //FileVault (v1)  passwords plugin
	algo_drupal7,         //Drupal >=7 hashes plugin
	algo_lm,              //LM plugin
	algo_luks,            //LUKS encrypted block device plugin
	algo_mscash,          //Domain cached credentials plugin
	algo_mscash2,         //Domain cached credentials v2 plugin
	algo_mssql_2000,      //Microsoft SQL Server 2000 plugin
	algo_mssql_2005,      //Microsoft SQL Server 2005 plugin
	algo_mssql_2012,      //Microsoft SQL Server 2012 plugin
	algo_mysql5,          //MySQL > 4.1 plugin
	algo_nsldap,          //LDAP SHA plugin
	algo_nsldaps,         //LDAP SSHA (salted SHA) plugin
	algo_ntlm,            //NTLM plugin
	algo_o5logon,         //Oracle TNS O5logon
	algo_oracle_old,      //Oracle 7 up to 10r2 plugin
	algo_oracle11g,       //Oracle 11g plugin
	algo_osx_old,         //MacOSX <= 10.6 system passwords plugin
	algo_osxlion,         //MacOSX Lion system passwords plugin
	algo_phpbb3,          //phpBB3 hashes plugin
	algo_pixmd5,          //Cisco PIX password hashes plugin
	algo_privkey,         //SSH/SSL private key passphrase plugin
	algo_ripemd160,       //RIPEMD-160 plugin
	algo_sapg,            //SAP CODVN G passwords plugin
	algo_sl3,             //Nokia SL3 plugin
	algo_smf,             //SMF plugin
	algo_wordpress,       //Wordpress hashes plugin
	algo_wpa,             //WPA-PSK plugin
};

//workitem
struct crack_block
{
	unsigned char algo;		//解密算法
	unsigned char charset;	//解密字符集
	unsigned char type;		//解密类型
	unsigned char special;	//是否是文件解密（pdf+office+rar+zip）
	char guid[40];			//服务端的workitem的GUID
	char john[HASHFILE_MAX_PLAIN_LENGTH];			//格式：e10adc3949ba59abbe56e057f20f883e 后面的为hash值
							//如果是文件解密，这里存放文件名
	unsigned short start;	//开始长度
	unsigned short end;		//结束长度
	//以下两个是索引
	unsigned short start2;	//55555-99999:start2=5,end2=9	000-55555:start2=0,end2=5
	unsigned short end2;
	char custom[0]; //用户自定义的字符集
};

//hash
struct crack_hash
{
	char hash[HASHFILE_MAX_PLAIN_LENGTH+4];
	char salt[HASHFILE_MAX_PLAIN_LENGTH];
	char salt2[HASHFILE_MAX_PLAIN_LENGTH];
};

//解密任务
struct crack_task
{
	unsigned char algo;		//解密算法
	unsigned char charset;	//解密字符集
	unsigned char type;		//解密类型
	unsigned char special;	//是否是文件解密（pdf+office+rar+zip）
	unsigned char startLength;//起始长度
	unsigned char endLength;	//终结长度
	unsigned char filename[256];	//用户传过来的文件名
	char guid[40];			//用户端的任务的GUID
	int count;				//需要解密的Hash个数（如果是文件=1）
	struct crack_hash hashes[0];			//这里需要动态申请
	
};

//这里描述一下crack_task/crack_hash/crack_block三者的关系
//用户上传一个解密任务（解密文件、算法、字符集、字符长度），
//服务端解析文件得到若干个crack_hash（可以保存在crack_task里面，需要动态申请），
//然后切割算法将其分割成若干个crack_block

struct crack_status
{
	char guid[40];				//workitem/crack_block的GUID
	unsigned int progress;		//进度	
	float speed;				//速度
	unsigned int remainTime;	//剩余时间
};

#define WORK_ITEM_AVAILABLE		0	//workitem的起始状态，可供其他计算单元使用
#define WORK_ITEM_LOCK			1	//workitem已经被一个计算单元占用，但是不确定计算单元的解密任务是否进行，此时不能被其他计算单元使用
#define WORK_ITEM_UNLOCK		2	//计算单元通知服务端unlock该资源，重新设置为avaiable，以供其他计算节点使用
#define WORK_ITEM_WORKING		3	//计算单元正在对该workitem进行解密任务
#define WORK_ITEM_CRACKED		4	//计算单元完成解密任务，同时破解出密码
#define WORK_ITEM_UNCRACKED		5	//计算单元完成解密任务，但没有破解出密码

struct crack_result
{
	char guid[40];			//workitem/crack_block的GUID
	unsigned int status;	//workitem的结果状态
	char password[32];		//如果解密成功，存放密码
};

struct hash_list_s
{
    char *username;
    char *hash;
    char *salt;
    char *salt2;
    struct hash_list_s *prev;
    struct hash_list_s *next;
    struct hash_list_s *indexprev;
    struct hash_list_s *indexnext;
};


#endif