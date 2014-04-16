

#ifndef _RES_PACKET_H_
#define _RES_PACKET_H_

#include "algorithm_types.h"

struct task_upload_res {

	unsigned char guid[40];
};



//deprecate response
struct task_status_res{

	unsigned char guid[40];
	unsigned int status;
	unsigned char password[32];
};

//new get a task result response
struct task_result_info{

	unsigned char john[sizeof(struct crack_hash)];
	unsigned char status;

	unsigned char password[32];
};


struct task_status_info{
	unsigned char guid[40];
	float m_progress;
	float m_speed;
	unsigned m_split_number;
	unsigned m_fini_number;

	unsigned int m_running_time;  //unit seconds
	unsigned int m_remain_time;   //unit seconds
	
	unsigned char m_algo;		  //Ëã·¨
	unsigned char status;		
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


