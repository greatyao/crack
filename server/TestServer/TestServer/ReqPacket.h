

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




#endif

