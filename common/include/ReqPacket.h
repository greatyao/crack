#ifndef _REQ_PACKET_H_
#define _REQ_PACKET_H_


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
#if defined(WIN64) || defined(X64)
	void* f;
#else
	void* f;
	int padding;
#endif
	unsigned int len;
	unsigned int offset;

};

struct file_upload_end_req{
	unsigned char guid[40];
#if defined(WIN64) || defined(X64)
	void* f;
#else
	void* f;
	int padding;
#endif
	unsigned int len;
	unsigned int offset;

};


#endif

