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
	charset_custom,		//�Լ�����
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
	unsigned char algo;		//�����㷨
	unsigned char charset;	//�����ַ���
	unsigned char type;		//��������
	unsigned char special;	//�Ƿ����ļ����ܣ�pdf+office+rar+zip��
	char guid[40];			//����˵�workitem��GUID
	char john[HASHFILE_MAX_PLAIN_LENGTH];			//��ʽ��e10adc3949ba59abbe56e057f20f883e �����Ϊhashֵ
							//������ļ����ܣ��������ļ���
	unsigned short start;	//��ʼ����
	unsigned short end;		//��������
	//��������������
	unsigned short start2;	//55555-99999:start2=5,end2=9	000-55555:start2=0,end2=5
	unsigned short end2;
	char custom[0]; //�û��Զ�����ַ���
};

//hash
struct crack_hash
{
	char hash[HASHFILE_MAX_PLAIN_LENGTH];
	char salt[HASHFILE_MAX_PLAIN_LENGTH];
	char salt2[HASHFILE_MAX_PLAIN_LENGTH];
};

//��������
struct crack_task
{
	unsigned char algo;		//�����㷨
	unsigned char charset;	//�����ַ���
	unsigned char type;		//��������
	unsigned char special;	//�Ƿ����ļ����ܣ�pdf+office+rar+zip��
	unsigned char filename[256];	//�û����������ļ���
	char guid[40];			//�û��˵������GUID
	int count;				//��Ҫ���ܵ�Hash������������ļ�=1��
	struct crack_hash hashes[0];			//������Ҫ��̬����
	
};

//��������һ��crack_task/crack_hash/crack_block���ߵĹ�ϵ
//�û��ϴ�һ���������񣨽����ļ����㷨���ַ������ַ����ȣ���
//����˽����ļ��õ����ɸ�crack_hash�����Ա�����crack_task���棬��Ҫ��̬���룩��
//Ȼ���и��㷨����ָ�����ɸ�crack_block

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