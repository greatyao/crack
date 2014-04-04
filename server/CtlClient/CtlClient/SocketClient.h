#pragma once

class CSocketClient
{
public:
	CSocketClient(void);
	~CSocketClient(void);
	
	int Init(char *ip,int port);

	//���ض�ȡ��data���ֽ����������ظ�����0��ʾ���ݽ�������û������
	//����������ܴ�һ���Է��Ͳ��꣬����seq�����֣���ʱ�ж����ݽ���?��read�Ƿ񷵻�0
	int Read(unsigned char  *cmd,short *status,void *data,int size, unsigned int* seq = NULL);
	
	//�����������С��һ���Ծͷ�����ϣ���seqĬ��Ϊ0
	//���������seq��1��ʼ�����������뷢��һ��dataLenΪ-1�����ݣ��������ݽ���
	int Write(unsigned char cmd, short status,void *data,int size, unsigned int seq = 0);
	int WriteNoCompress(unsigned char cmd, short status,void *data,int size, unsigned int seq = 0);

	int Finish();
public:
		

	SOCKET m_clientsocket;

};
