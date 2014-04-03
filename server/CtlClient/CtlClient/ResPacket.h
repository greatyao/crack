

#ifndef _RES_PACKET_H_
#define _RES_PACKET_H_


struct task_upload_res {

	unsigned char guid[40];
};


struct task_status_res{

	unsigned char guid[40];
	unsigned int status;
	unsigned char password[32];
};


struct task_status_info{

	unsigned char guid[40];
	float m_progress;
	
	unsigned m_split_number;
	unsigned m_fini_number;
	unsigned int status;
	
};

struct compute_node_info{

	unsigned char guid[40];
	unsigned int cputhreads;
	unsigned int gputhreads;
	unsigned char hostname[50];
	unsigned char ip[20];
	unsigned char os[48];
		
};



//////upload file start 

struct file_upload_res {

	
	unsigned char guid[40];
	void *f;
	unsigned int len;
	unsigned int offset;
};


struct file_upload_start_res{

	unsigned char guid[40];
	void *f;
	unsigned int len;
	unsigned int offset;

};

struct file_upload_end_res {
	
	unsigned char guid[40];
	void *f;
	unsigned int len;
	unsigned int offset;

};



#endif


