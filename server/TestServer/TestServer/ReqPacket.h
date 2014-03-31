

#ifndef _REQ_PACKET_H_
#define _REQ_PACKET_H_


//��¼��Ϣ

struct client_login_req {
	
	char m_osinfo[16];	//����ϵͳ��Ϣ
	char m_ip[20];		//IP��ַ��Ϣ

	char m_type;		//�ͻ�������,control , compute
	
	char m_hostname[50];//��������
	unsigned char m_guid[40]; //�ڵ�guid
	
	unsigned int m_clientsock;
	int m_gputhreads;
	int m_cputhreads;			
};

//������Ϣ
struct client_keeplive_req {

	unsigned char m_guid[40];
	
};

struct task_start_req {

	unsigned char guid[40];
};



struct task_stop_req {

	unsigned char guid[40];
};




struct task_pause_req {

	unsigned char guid[40];
};


struct task_delete_req {

	unsigned char guid[40];
};


struct task_result_req {

	unsigned char guid[40];
};


struct download_file_req {

	unsigned char guid[40];
};



//upload file struct

struct file_upload_req {

	unsigned char guid[40];

};


struct file_upload_start_req {


	unsigned char guid[40];
	void *f;
	unsigned int len;
	unsigned int offset;

};



#endif

