#include "macros.h"
#include "err.h"
#include "ServerResp.h"
#include "CLog.h"
#include "zlib.h"
#include "algorithm_types.h"
#include "PacketProcess.h"
#include "ClientInfo.h"
#include "conn.h"

static unsigned char pack_flag[5] = {'G', '&', 'C', 'P', 'U'};

static int read_timeout(UINT_PTR fd, unsigned int wait_seconds)
{    
	int ret = 0;    
	if (wait_seconds > 0)    
	{             
		fd_set read_fdset;        
		struct timeval timeout;             
		
		FD_ZERO(&read_fdset);        
		FD_SET(fd, &read_fdset);             
		timeout.tv_sec = wait_seconds;        
		timeout.tv_usec = 0;             
		
		return select(fd + 1, &read_fdset, NULL, &read_fdset,  &timeout);    
	}         
	return ret;
}

int Read(UINT_PTR sck, unsigned char *cmd, short* status, void* data, int size)
{
	control_header hdr;
	//if(recv(sck, (char*)&hdr, sizeof(hdr), 0) <= 0) 
	if(conn_read((conn)sck, (char*)&hdr, sizeof(hdr), 1) <= 0) 
	{
		int err = conn_error;
		CLog::Log(2, "failed to recv header %d\n", err);
		return err == 10060 ? ERR_TIMEOUT : ERR_CONNECTIONLOST;
	}
	
	if(memcmp(hdr.magic, pack_flag, 5) != 0)
		return ERR_INVALIDDATA;
		
	*cmd = hdr.cmd;
	*status = hdr.response;
	int totalN = hdr.compressLen;
	int origN = hdr.dataLen;
	
	if(origN < 0 || size < origN)
		return ERR_INVALIDDATA;
	if(origN == 0)
		return 0;
	
	unsigned char* buf = NULL;
	int m;
	if(totalN == -1) 
	{
		buf = (unsigned char*)data;
		m = origN;
	}
	else
	{
		buf = new unsigned char[totalN];
		m = totalN;
	}
	
	int total = 0;
	int n;

	n = conn_read((conn)sck, (char *)buf, m, 1);
	if(n <= 0)
	{
		CLog::Log(2, "failed to recv header %d\n", GetLastError());
		delete []buf;
		return ERR_CONNECTIONLOST;
	}
	/*do{
		if((n=recv(sck, (char *)buf+total, m-total, 0)) <= 0)
		{
			int err = GetLastError();
			CLog::Log(2, "failed to recv body %d\n", err);
			delete []buf;
			return err == 10060 ? ERR_TIMEOUT : ERR_CONNECTIONLOST;
		}
		total += n;
		if(total == m) break;
	}while(1);
	*/
	
	if(totalN == -1) 
		return m; 
	
	unsigned long uncompressLen = size;
	int ret = uncompress((Bytef*)data, (uLongf*)&uncompressLen, buf, totalN);
	delete []buf;
	if(ret != 0 || uncompressLen != origN)
		return ERR_UNCOMPRESS;
	
	return uncompressLen;
}

static int mysend(UINT_PTR sck, void* buf, int size, int flag)
{
	int total = 0;
	int n;
	do{	
		if((n=send(sck, (char *)buf+total, size-total, flag)) < 0)
			return -1;
		total += n;
		if(total == size) break;
	}while(1);	

	return size;
}

int Write(UINT_PTR sck, unsigned char cmd, short status, const void* data, int size,bool bCompress)
{
	struct control_header hdr = INITIALIZE_EMPTY_HEADER(cmd);
	hdr.response = status;
	if(!data || size == 0)
	{
		//if(mysend(sck, (char *)&hdr, sizeof(hdr), 0) < 0)
		if(conn_write((conn)sck, (char *)&hdr, sizeof(hdr), 1) < 0)
			return ERR_CONNECTIONLOST;
		return 0;
	}
	
	unsigned long destLen = 1.1*size+8;	
	unsigned char* dest = new unsigned char[destLen];
	unsigned char *pBuf = NULL;
	int ret = 0;

	if (bCompress){
		ret = compress(dest, &destLen, (const Bytef*)data, size);
		if(ret != 0)
		{
			delete []dest;
			return ERR_COMPRESS;
		}

		pBuf = dest;
		
		hdr.compressLen = destLen;
	}else {

		
		hdr.compressLen = -1;
		destLen = size;
		pBuf =(unsigned char *)data;
		
	}
	
	hdr.dataLen = size;
	
	//if(mysend(sck, (char *)&hdr, sizeof(hdr), 0) < 0 || mysend(sck, (char *)pBuf, destLen, 0) < 0)
	if(conn_write((conn)sck, (char *)&hdr, sizeof(hdr), 1) < 0 || conn_write((conn)sck, (char *)pBuf, destLen, 1) < 0)
	{
		delete []dest;
		return ERR_CONNECTIONLOST;
	}
	
	delete []dest;
	return destLen;
}


void DispatchThread(void* p){
	CClientInfo* client = (CClientInfo*)p;
	char* ip = client->GetIP();
	int port = client->GetPort();
	SOCKET cliSocket = client->GetSocket();

	INT nRet = 0;
	UINT len = 0;

	BYTE recvBuf[MAX_BUF_LEN*4];
	INT cmdheader = sizeof(control_header);
	unsigned char cmd;
	short status;
	conn_set allConns;
	allConns.push_back((conn)cliSocket);


	while(1)
	{
		cmd = 0;
		//int r = read_timeout(cliSocket, 5);
		conn_set readConns, excpConns;
		int r = conn_selectEx(allConns, &readConns, &excpConns, 5000);
		if( r == 0)
		{
			CLog::Log(LOG_LEVEL_DEBUG, "Reading %s:%d TIMEOUT\n",ip, port);
			continue;
		}
		else if(r < 0) 
		{
			CLog::Log(LOG_LEVEL_DEBUG, "select failed\n");
			break;
		}

		int m = Read(cliSocket, &cmd, &status, recvBuf, sizeof(recvBuf));
		//CLog::Log(LOG_LEVEL_DEBUG, "%s:%d recv cmd %d status %d body %d\n",ip, port, cmd, status, m);

		if(m == ERR_CONNECTIONLOST) {
			cmd = CMD_CLIENT_QUIT;
			memset(recvBuf,0,36000);
			memcpy(recvBuf,ip,strlen(ip));
			m = port;
			doRecvData(p,recvBuf,m,cmd);
			break;//ÍÆ³öÁË
		}else if(m == ERR_INVALIDDATA || m == ERR_UNCOMPRESS || m == ERR_TIMEOUT)
			continue;
		
		doRecvData(p, recvBuf, m, cmd);
	}

	CLog::Log(LOG_LEVEL_WARNING, "Client [%s:%d] Quit!\n",ip,port);
	delete p;
}
