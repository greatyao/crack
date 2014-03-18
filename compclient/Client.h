#ifndef __CLIENT_H__
#define __CLIENT_H__


class Client
{
public:
	static Client& Get();
	
	int Connect(const char* ip, unsigned short port);
	
	int WriteToServer(const void* data, int size);
	
	int ReadFromServer(void* data, int size);
	
private:
	Client();
	~Client();
	
	int Read(void* data, int size);
	
	int Write(const void* data, int size);

private:
	int sck;
	char ip[16];
	unsigned short port;
	int connected;//0��ʾ�Ͽ���1��ʾ��������2��ʾ��������
};

#endif