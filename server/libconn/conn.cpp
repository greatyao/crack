/* conn.h
 *
 * Conn socket for tcp, udp and http
 * Copyright (C) 2012-2014 YAO Wei
 *
 * Created By YAO Wei at  03/19/2012
 */

#include <WinSock2.h>
#include <wininet.h>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string>

#include "conn.h"
#include "udt.h"
#include "ws32_internal.h"

#pragma comment(lib, "wininet.lib")

#define CONN_MAGIC	0x98765432
static const char* cli_fmt =  "POST / HTTP/1.1\r\n"
			"Content-type: application/octet-stream\r\n"
			"User-Agent:Mozilla/5.0 (compatible; MSIE 8; Windows NT)\r\n"
			"Accept:*/*\r\n"
			"Host:%s\r\n"
			"Connection:Keep-Alive\r\n"
			"Content-Length:%d\r\n\r\n";
static const int cli_len = 36;

static const char* svr_fmt = "HTTP/1.1 200 OK\r\n"
					"Server: nginx/1.0.8\r\n"
					"Date:%s\r\n"
					"Content-Type:application/octet-stream\r\n"
					"Content-Length:%d\r\n"
					"Connection:Keep-Alive\r\n"
					"Accept-Ranges:bytes\r\n\r\n";
static const int svr_len = 36;
		
typedef struct conn_struct
{
	int magic;
	int type;
	union{
		UDTSOCKET udtsock;
		SYSSOCKET tcpsock;
		struct {
			HINTERNET hSession;
			HINTERNET hConnect;
			HINTERNET hRequest;
		}httpclient;
	}sock;
}conn_struct;


int conn_error;

static bool first = false;
static struct ws32_loadlibrary_func ws32;
struct ws32_loadlibrary_func* gWS32;

int conn_init(struct ws32_loadlibrary_func* func)
{
	if(first)	return 0;

	HMODULE module = LoadLibraryA("ws2_32.dll");
	if(module == NULL)	return -1;

	func->WSAStartup = (int (WINAPI *)(WORD,LPWSADATA))GetProcAddress(module, "WSAStartup");
	func->WSACleanup = (int (WINAPI *)(void))GetProcAddress(module, "WSACleanup");
	func->WSAGetLastError = (int (WINAPI *)(void))GetProcAddress(module, "WSAGetLastError");
	func->accept = (SOCKET (WINAPI *)(SOCKET,sockaddr *,int *))GetProcAddress(module, "accept");
	func->bind = (int (WINAPI *)(SOCKET,const sockaddr *,int))GetProcAddress(module, "bind");
	func->closesocket = (int (WINAPI *)(SOCKET))GetProcAddress(module, "closesocket");
	func->connect = (int (WINAPI *)(SOCKET,const sockaddr *,int))GetProcAddress(module, "connect");
	func->getpeername = (int (WINAPI *)(SOCKET,sockaddr *,int *))GetProcAddress(module, "getpeername");
	func->getsockname = (int (WINAPI *)(SOCKET,sockaddr *,int *))GetProcAddress(module, "getsockname");
	func->getsockopt = (int (WINAPI *)(SOCKET,int,int,char *,int *))GetProcAddress(module, "getsockopt");
	func->listen = (int (WINAPI *)(SOCKET,int))GetProcAddress(module, "listen");
	func->recv = (int (WINAPI *)(SOCKET,char *,int,int))GetProcAddress(module, "recv");
	func->select = (int (WINAPI *)(int,fd_set *,fd_set *,fd_set *,const timeval *))GetProcAddress(module, "select");
	func->send = (int (WINAPI *)(SOCKET,const char *,int,int))GetProcAddress(module, "send");
	func->setsockopt = (int (WINAPI *)(SOCKET,int,int,const char *,int))GetProcAddress(module, "setsockopt");
	func->socket = (SOCKET (WINAPI *)(int,int,int))GetProcAddress(module, "socket");
	func->WSASendTo = (int (WINAPI *)(SOCKET,LPWSABUF,DWORD,LPDWORD,DWORD,const sockaddr *,int,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE))GetProcAddress(module, "WSASendTo");
	func->WSARecvFrom = (int (WINAPI *)(SOCKET,LPWSABUF,DWORD,LPDWORD,LPDWORD,sockaddr *,LPINT,LPWSAOVERLAPPED,LPWSAOVERLAPPED_COMPLETION_ROUTINE))GetProcAddress(module, "WSARecvFrom");
	func->htonl = (unsigned long (WINAPI *)(unsigned long))GetProcAddress(module, "htonl");
	func->htons = (unsigned short (WINAPI *)(unsigned short))GetProcAddress(module, "htons");
	func->getaddrinfo = (int (WINAPI *)(PCSTR,PCSTR,const ADDRINFOA *,PADDRINFOA *))GetProcAddress(module, "getaddrinfo");
	func->freeaddrinfo = (void (WINAPI *)(PADDRINFOA))GetProcAddress(module, "freeaddrinfo");
	func->getnameinfo = (int (WINAPI *)(const SOCKADDR *,socklen_t,PCHAR,DWORD,PCHAR,DWORD,INT))GetProcAddress(module, "getnameinfo");
	func->inet_ntoa = (char *(WINAPI *)(in_addr))GetProcAddress(module, "inet_ntoa");
	func->inet_addr = (unsigned long (WINAPI *)(const char *))GetProcAddress(module, "inet_addr");
	func->ntohl = (unsigned long (WINAPI *)(unsigned long))GetProcAddress(module, "ntohl");
	func->ntohs = (unsigned short (WINAPI *)(unsigned short))GetProcAddress(module, "ntohs");
	func->__WSAFDIsSet = (int (WINAPI *)(SOCKET,fd_set *))GetProcAddress(module, "__WSAFDIsSet");

	if(!func->WSAStartup || !func->WSACleanup || !func->WSAGetLastError || !func->WSARecvFrom || !func->WSASendTo ||
		!func->accept || !func->bind || !func->closesocket ||
		!func->connect || !func->getpeername || !func->getsockname || !func->getsockopt || !func->recv ||
		!func->select || !func->send || !func->setsockopt || !func->socket || !func->listen ||
		!func->htonl || !func->htons || !func->getaddrinfo || !func->freeaddrinfo ||
		!func->getnameinfo || !func->inet_ntoa ||!func->inet_addr || !func->ntohl || !func->ntohs)
	{
#if _WIN32_WINNT > 0x0500
		//FreeLibrary(module);
		OutputDebugStringA("Some functions in ws2_32.dll are not implemented\n");
		//return -1;
#endif
	}

	WSADATA wsaData;
	if(func->WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		FreeLibrary(module);
		OutputDebugStringA("WSAStartup failed\n");
		return -1;
	}

	first = true;
	gWS32 = func;

	return 0;
}

int conn_startup()
{
	if(!first)	conn_init(&ws32);

	WSADATA wsaData;
	if(gWS32->WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	return 0;
}


unsigned long conn_htonl(unsigned long addr)
{
	if(!gWS32 || !gWS32->htonl)	return 0xffffffff;
	return gWS32->htonl(addr);
}

unsigned short conn_htons(unsigned short addr)
{
	if(!gWS32 || !gWS32->htons)	return 0xffff;
	return gWS32->htons(addr);
}

unsigned long conn_ntohl(unsigned long addr)
{
	if(!gWS32 || !gWS32->ntohl)	return 0xffffffff;
	return gWS32->ntohl(addr);
}

unsigned short conn_ntohs(unsigned short addr)
{
	if(!gWS32 || !gWS32->ntohs)	return 0xffff;
	return gWS32->ntohs(addr);
}

char* conn_inet_ntoa(struct in_addr addr)
{
	if(!gWS32 || !gWS32->inet_ntoa)	return NULL;
	return gWS32->inet_ntoa(addr);
}

unsigned long conn_inet_addr(const char * addr)
{
	if(!gWS32 || !gWS32->inet_addr)	return 0xffffffff;
	return gWS32->inet_addr(addr);
}

conn conn_socket(int type)
{
	if(!first)
	{
		if(conn_init(&ws32) < 0)
		{
			conn_error = ERRCONNSETUP;
			return NULL;
		}
	}
	
	conn_struct* c = new conn_struct;
	memset(c, 0, sizeof(conn_struct));
	c->type = type;
	c->magic = CONN_MAGIC;
	if(type == CONN_TCP || type == CONN_HTTP_CLIENT || type == CONN_HTTP_SERVER)
	{
		//单纯的TCP或者HTTP服务端
		//if(type == CONN_TCP || type == CONN_HTTP_SERVER) 
		{
			if( (c->sock.tcpsock = gWS32->socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			{
				conn_error = gWS32->WSAGetLastError();
				delete c;
				return NULL;
			}
		}
// 		else
// 		{
// 			static const char* userAgent = "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.1; )";
// 			if( (c->sock.httpclient.hSession = InternetOpenA(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) == NULL)
// 			{
// 				conn_error = GetLastError();
// 				delete c;
// 				return NULL;
// 			}
// 			printf("InternetOpenA %p\n", c->sock.httpclient.hSession);
// 		}
	}
	else
	{
		if( (c->sock.udtsock = UDT::socket(AF_INET, type == CONN_UDP1 ? SOCK_STREAM : SOCK_DGRAM, 0)) == UDT::INVALID_SOCK)
		{
			conn_error = UDT::getlasterror().getErrorCode();
			delete c;
			return NULL;
		}

		bool opt = true;
		UDT::setsockopt(c->sock.udtsock, 1, UDT_SNDSYN, &opt, sizeof(opt));
		UDT::setsockopt(c->sock.udtsock, 1, UDT_RCVSYN, &opt, sizeof(opt));
	}
	return c;
}

enum ConnType conn_socktype(conn s)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return (ConnType)-1;
	}

	return (ConnType)c->type;
}

conn conn_accept(conn s, struct sockaddr *addr, int *len)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return NULL;
	}

	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER)
	{
		SOCKET incoming = gWS32->accept(c->sock.tcpsock, addr, len);
		if(incoming < 0)
		{
			conn_error = gWS32->WSAGetLastError();
			return NULL;
		}
		//printf("incoming a new client %p\n", incoming);
		conn_struct* c1 = new conn_struct;
		memset(c1, 0, sizeof(conn_struct));
		c1->magic = CONN_MAGIC;
		c1->type = c->type;
		c1->sock.tcpsock = incoming;
		return c1;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		UDTSOCKET a = UDT::accept(c->sock.udtsock, addr, len);
		if(a == UDT::INVALID_SOCK)
		{
			conn_error = UDT::getlasterror().getErrorCode();
			return NULL;
		}
		conn_struct* c1 = new conn_struct;
		c1->magic = CONN_MAGIC;
		c1->type = c->type;
		c1->sock.udtsock = a;

		return c1;
	}
	else
	{
		conn_error = ERRINVOP;
		return NULL;
	}
}

int conn_bind (conn s, const struct sockaddr *addr, int namelen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER)
	{
		if( (r = gWS32->bind(c->sock.tcpsock, addr, namelen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		if( (r = UDT::bind(c->sock.udtsock, addr, namelen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
	else
	{
		conn_error = ERRINVOP;
		return -1;
	}
}

int conn_close(conn s)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER || c->type == CONN_HTTP_CLIENT)
		gWS32->closesocket(c->sock.tcpsock);
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
		UDT::close(c->sock.udtsock);
// 	else
// 	{
// 		if(c->sock.httpclient.hSession)	InternetCloseHandle(c->sock.httpclient.hSession);
// 		if(c->sock.httpclient.hConnect)	InternetCloseHandle(c->sock.httpclient.hConnect);
// 		if(c->sock.httpclient.hRequest)	InternetCloseHandle(c->sock.httpclient.hRequest);
// 	}
	c->magic = 0;
	delete c;
	s = NULL;
	return 0;
}

int conn_connect(conn s, const struct sockaddr *name, int namelen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT)
	{
		if( (r = gWS32->connect(c->sock.tcpsock, name, namelen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
// 	else if(c->type == CONN_HTTP_CLIENT)
// 	{
// 		struct sockaddr_in* addr = (struct sockaddr_in*)name;
// 		printf("connecting %s:%d\n", gWS32->inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
// 		c->sock.httpclient.hConnect = InternetConnectA(c->sock.httpclient.hSession, inet_ntoa(addr->sin_addr), 
// 			ntohs(addr->sin_port),  NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
// 		if(c->sock.httpclient.hConnect == NULL)
// 		{
// 			conn_error = GetLastError();
// 			return -1;
// 		}
// 		printf("Connection %p\n", c->sock.httpclient.hConnect);
// 		return 0;
// 	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		if( (r = UDT::connect(c->sock.udtsock, name, namelen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
	else
	{
		conn_error = ERRINVOP;
		return -1;
	}
}

int conn_listen(conn s, int backlog)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER)
	{
		if( (r = gWS32->listen(c->sock.tcpsock, backlog)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		if((r = UDT::listen(c->sock.udtsock, backlog)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return 0;
	}
	else 
	{
		conn_error = ERRINVOP;
		return -1;
	}
}

static int myRead(SOCKET s, char* buf, int len)
{
	int offset = 0, nRead;

	do{
		//printf("%d %d\n", offset, len);
		if(offset == len) break;
		if( (nRead = gWS32->recv(s, buf+offset, len-offset, 0))<= 0)
		{
			conn_error = gWS32->WSAGetLastError();
			return -1;
		}
		offset += nRead;
	}while(1);

	return offset;
}

int conn_read(conn s, char * buf, int len, int flags, char* rawHttp)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP)
	{
		if(flags == 1)
			return myRead(c->sock.tcpsock, buf, len);
		if((r = gWS32->recv(c->sock.tcpsock, buf, len, 0)) < 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_HTTP_SERVER || c->type == CONN_HTTP_CLIENT)
	{
		if(flags)
		{
			if((r = gWS32->recv(c->sock.tcpsock, buf, len, 0)) < 0)
				conn_error = gWS32->WSAGetLastError();
			return r;
		}
		
		char myHeader[1024] = {0};
		char* header = myHeader;
		if(rawHttp) header = rawHttp;
		int hdrLen = c->type == CONN_HTTP_SERVER ? cli_len : svr_len;
		char output[1024];		

		/**首先读取HTTP头**/
		if((r = myRead(c->sock.tcpsock, header, hdrLen)) < 0)
			return r;

		if(strstr(header, "HTTP") == NULL)
		{
			sprintf(output, "recv partly header %s", header);
			OutputDebugStringA(output);
			conn_error = ERRIGNORE;
			return 0;
		}

		if(char* p = strstr(header, "\r\n\r\n"))
		{
			len = r - (p - header) - 4;
			memcpy(buf, p+4, len);
			*(p + 4) = 0;
			return len;
		}

		//读取到最后的\r\n\r\n
retry:
		//printf("【%s】", header);
		int nTry = 0, maxTry = 512;
		do
		{
			if(nTry == maxTry)
			{
				conn_error = ERRIGNORE;
				OutputDebugStringA("exceed maxtry");
				return 0;
			}

			r = gWS32->recv(c->sock.tcpsock, header+hdrLen, 1, flags);
			if(r < 0)
			{
				conn_error = gWS32->WSAGetLastError();			
				return r;
			}

			nTry ++;
			hdrLen ++;
			if((header+hdrLen-1)[0] == '\r')
				break;
		}while(1);

		r = myRead(c->sock.tcpsock, header+hdrLen, 3);
		if(r < 0)
		{
			conn_error = gWS32->WSAGetLastError();			
			return r;
		}
		if(strncmp(header+hdrLen, "\n\r\n", 3) != 0)
		{
			if((header+hdrLen)[0] == '\n')	
			{
				hdrLen += 3;
				goto retry;
			}

			hdrLen += 3;
			conn_error = ERRIGNORE;
			sprintf(output, "http end error{%s}", header);
			OutputDebugStringA(output);
			return 0;
		}

		char* findCL = strstr(header,"Content-Length");
		int nCL = 0;
		if(findCL)
		{
			findCL += strlen("Content-Length") + 1;
			char* endCL =  strstr(findCL,"\r\n");
			if(endCL)
			{
				char value[32] = {0};
				strncpy(value, findCL, endCL - findCL);
				nCL = atoi(value);
			}
		}

		len = nCL;
		if(nCL == 0)	return 0;
		return  myRead(c->sock.tcpsock, buf, len);
	}
	else if(c->type == CONN_UDP1)
	{
		int offset = 0;
		do{
			r = UDT::recv(c->sock.udtsock, buf+offset, len-offset, flags);
			if (r == UDT::ERROR)
			{
				conn_error = UDT::getlasterror().getErrorCode();
				return r;
			}
			offset += r;

		}while(offset < len);
		return offset;
	}
	else if(c->type == CONN_UDP2)
	{
		int offset = 0;
		do{
			r = UDT::recvmsg(c->sock.udtsock, buf+offset, len-offset);
			if (r == UDT::ERROR)
			{
				conn_error = UDT::getlasterror().getErrorCode();
				return r;
			}
			offset += r;

		}while(offset < len);
		return offset;
	}
	else return -1;
}

static int myWrite(SOCKET s, const char* buf, int len)
{
	int offset = 0, nWrite;

	do{
		if(offset == len) break;
		if( (nWrite = gWS32->send(s, buf+offset, len-offset, 0))<= 0)
		{
			conn_error = gWS32->WSAGetLastError();
			return -1;
		}
		offset += nWrite;
	}while(1);

	return offset;
}

int conn_write(conn s, const char * buf, int len, int flags)
{
	conn_struct* c = (conn_struct*)s;
	len = len < 0 ? 0 : len;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP)
	{
		return myWrite(c->sock.tcpsock, buf, len);
	}
	else if(c->type == CONN_HTTP_SERVER)
	{
		if(flags)
			return myWrite(c->sock.tcpsock, buf, len);
		
		char hdr[1024];
		int hdrLen;
		static clock_t lastT = clock();
		time_t now = time(NULL);
		char nowStr[512];
		strcpy(nowStr, ctime(&now));
		if(nowStr[strlen(nowStr)-1] == '\n')	nowStr[strlen(nowStr)-1] = 0;
		sprintf_s(hdr, sizeof(hdr), svr_fmt, nowStr, len);
		hdrLen = strlen(hdr);
		
		if((r = myWrite(c->sock.tcpsock, hdr, hdrLen))< 0 ||
			(r = myWrite(c->sock.tcpsock, buf, len)) < 0 )
			return -1;
		
		if( 1.0*(clock() - lastT) / CLOCKS_PER_SEC > 0.33f)	
		{
			lastT = clock();
			Sleep(80);
		}
		return r;
	}
	else if(c->type == CONN_HTTP_CLIENT)
	{
		if(flags)
			return myWrite(c->sock.tcpsock, buf, len);
		
		char hdr[1024];
		int hdrlen;
		static clock_t lastT = clock();

		sockaddr_in addr;
		int addrlen = sizeof(addr);
		gWS32->getpeername(c->sock.tcpsock, (struct sockaddr*)&addr, &addrlen);
		sprintf(hdr, cli_fmt, gWS32->inet_ntoa(addr.sin_addr), len);
		hdrlen = strlen(hdr);
		
		if((r = myWrite(c->sock.tcpsock, hdr, hdrlen)) < 0 ||
			(r = myWrite(c->sock.tcpsock, buf, len)) < 0)
			return -1;
		
		if( 1.0*(clock() - lastT) / CLOCKS_PER_SEC > 0.33f)	
		{
			lastT = clock();
			Sleep(80);
		}
		return r;
	}
	else if(c->type == CONN_UDP1)
	{
		int offset = 0;
		do{
			r = UDT::send(c->sock.udtsock, buf+offset, len-offset, flags);
			if (r == UDT::ERROR)
			{
				conn_error = UDT::getlasterror().getErrorCode();
				return r;
			}
			offset += r;

		}while(offset < len);
		return offset;
	}
	else
	{
		int offset = 0;
		do{
			r = UDT::sendmsg(c->sock.udtsock, buf+offset, len-offset);
			if (r == UDT::ERROR)
			{
				conn_error = UDT::getlasterror().getErrorCode();
				return r;
			}
			offset += r;
		}while(offset < len);
		return offset;
	}

}

int conn_getpeername(conn s, struct sockaddr* name, int* namelen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT || c->type == CONN_HTTP_SERVER)
	{
		if( (r = gWS32->getpeername(c->sock.tcpsock, name, namelen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		if( (r = UDT::getpeername(c->sock.udtsock, name, namelen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
	else
	{
		conn_error = ERRINVOP;
		return -1;
	}
}

int conn_getsockname(conn s, struct sockaddr* name, int* namelen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT || c->type == CONN_HTTP_SERVER)
	{
		if( (r = gWS32->getsockname(c->sock.tcpsock, name, namelen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else
	{
		if( (r = UDT::getsockname(c->sock.udtsock, name, namelen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
}


int conn_getsockopt(conn s, int level, ConnOpt optname, void* optval, int* optlen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	int l = SOL_SOCKET, opt, willdo = 1;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER)
	{
		switch(optname)
		{
		case CONN_MAXBW:
		case CONN_MSS:
		case CONN_MSGTTL:
		case CONN_RENDEZVOUS:
			willdo = 0; break;
		case CONN_SNDBUF:
			opt = SO_SNDBUF; break;
		case CONN_RCVBUF:
			opt = SO_RCVBUF; break;
		case CONN_LINGER:
			opt = SO_LINGER; break;
		case CONN_SNDTIMEO:
			opt = SO_SNDTIMEO; break;
		case CONN_RCVTIMEO:
			opt = SO_RCVTIMEO; break;
		case CONN_REUSEADDR:
			opt = SO_REUSEADDR; break;
		}
		if(!willdo) {
			conn_error = ERRUNKNOWN;
			return -1;
		}
		if((r = gWS32->getsockopt(c->sock.tcpsock, l, opt, (char*)optval, optlen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		UDTOpt opt;
		int r = 0;
		switch(optname)
		{
		case CONN_MAXBW:
			opt = UDT_MAXBW; break;
		case CONN_MSS:
			opt = UDT_MSS; break;
		case CONN_MSGTTL:
			opt = UDT_MSGTTL; break;
		case CONN_RENDEZVOUS:
			opt = UDT_RENDEZVOUS; break;
		case CONN_SNDBUF:
			opt = UDT_SNDBUF; break;
		case CONN_RCVBUF:
			opt = UDT_RCVBUF; break;
		case CONN_LINGER:
			opt = UDT_LINGER; break;
		case CONN_SNDTIMEO:
			opt = UDT_SNDTIMEO; break;
		case CONN_RCVTIMEO:
			opt = UDT_RCVTIMEO; break;
		case CONN_REUSEADDR:
			opt = UDT_REUSEADDR; break;
		}
		if((r = UDT::getsockopt(c->sock.udtsock, level, opt, optval, optlen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
	else
	{
		conn_error = ERRINVOP;
		return -1;
	}
}

int conn_setsockopt(conn s, int level, ConnOpt optname, const void* optval, int optlen)
{
	conn_struct* c = (conn_struct*)s;
	if(c == NULL || c->magic != CONN_MAGIC)	
	{
		conn_error = ERRINVSOCK;
		return -1;
	}

	int r = 0;
	int l = SOL_SOCKET, opt, willdo = 1;
	if(c->type == CONN_TCP || c->type == CONN_HTTP_SERVER)
	{
		switch(optname)
		{
		case CONN_MAXBW:
		case CONN_MSS:
		case CONN_MSGTTL:
		case CONN_RENDEZVOUS:
			willdo = 0; break;
		case CONN_SNDBUF:
			opt = SO_SNDBUF; break;
		case CONN_RCVBUF:
			opt = SO_RCVBUF; break;
		case CONN_LINGER:
			opt = SO_LINGER; break;
		case CONN_SNDTIMEO:
			opt = SO_SNDTIMEO; break;
		case CONN_RCVTIMEO:
			opt = SO_RCVTIMEO; break;
		case CONN_REUSEADDR:
			opt = SO_REUSEADDR; break;
		}
		if(!willdo) {
			conn_error = ERRUNKNOWN;
			return -1;
		}
		if((r = gWS32->setsockopt(c->sock.tcpsock, l, opt, (char*)optval, optlen)) != 0)
			conn_error = gWS32->WSAGetLastError();
		return r;
	}
	else if(c->type == CONN_UDP1 || c->type == CONN_UDP2)
	{
		UDTOpt opt;
		int r = 0;
		switch(optname)
		{
		case CONN_MAXBW:
			opt = UDT_MAXBW; break;
		case CONN_MSS:
			opt = UDT_MSS; break;
		case CONN_MSGTTL:
			opt = UDT_MSGTTL; break;
		case CONN_RENDEZVOUS:
			opt = UDT_RENDEZVOUS; break;
		case CONN_SNDBUF:
			opt = UDT_SNDBUF; break;
		case CONN_RCVBUF:
			opt = UDT_RCVBUF; break;
		case CONN_LINGER:
			opt = UDT_LINGER; break;
		case CONN_SNDTIMEO:
			opt = UDT_SNDTIMEO; break;
		case CONN_RCVTIMEO:
			opt = UDT_RCVTIMEO; break;
		case CONN_REUSEADDR:
			opt = UDT_REUSEADDR; break;
		}
		if((r = UDT::setsockopt(c->sock.udtsock, level, opt, optval, optlen)) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		return r;
	}
	else
	{
		conn_error = ERRINVOP;
		return -1;
	}
}


static conn find_udt(const conn_set& fds, UDTSOCKET sock)
{
	for(conn_set::const_iterator it = fds.begin(); it != fds.end(); it++)
	{
		conn_struct* c = (conn_struct*)*it;
		if(c == NULL || c->magic != CONN_MAGIC)
			continue;
		if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT || c->type == CONN_HTTP_SERVER)
			continue;
		if(c->sock.udtsock == sock)
			return c;
	}
	return NULL;
}

static conn find_tcp(const conn_set& fds, SOCKET sock)
{
	for(conn_set::const_iterator it = fds.begin(); it != fds.end(); it++)
	{
		conn_struct* c = (conn_struct*)*it;
		if(c == NULL || c->magic != CONN_MAGIC)
			continue;
		if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT || c->type == CONN_HTTP_SERVER)
		{
			if(c->sock.tcpsock == sock)	return c;
		}
	}
	return NULL;
}

int conn_selectEx(const conn_set& fds, conn_set* readfds, conn_set* exceptfds, int msTimeOut)
{
	int nudp = 0, ntcp = 0;
	std::vector<SOCKET> tfds;
	fd_set rset, eset;
	std::vector<UDTSOCKET> ufds;
	std::vector<UDTSOCKET> readufds, exceptufds;

	if (NULL != readfds)	readfds->clear();
	if (NULL != exceptfds)	exceptfds->clear();
	FD_ZERO(&rset);
	FD_ZERO(&eset);

	for(conn_set::const_iterator it = fds.begin(); it != fds.end(); it++)
	{
		conn_struct* c = (conn_struct*)*it;
		if(c == NULL || c->magic != CONN_MAGIC)
			continue;

		if(c->type == CONN_TCP || c->type == CONN_HTTP_CLIENT || c->type == CONN_HTTP_SERVER)
		{
			ntcp++;
			FD_SET(c->sock.tcpsock, &rset);
			FD_SET(c->sock.tcpsock, &eset);
			tfds.push_back(c->sock.tcpsock);
		}
		else
		{
			nudp++;
			ufds.push_back(c->sock.udtsock);
		}
	}

	int r1 = 0, r2 = 0;
	if(nudp)
	{
		if( (r1 = UDT::selectEx(ufds, &readufds, NULL, &exceptufds, msTimeOut*nudp/(nudp+ntcp+0.0001))) == UDT::ERROR)
			conn_error = UDT::getlasterror().getErrorCode();
		else
		{
			for(std::vector<UDTSOCKET>::iterator it = readufds.begin(); it != readufds.end(); it++)
				readfds->push_back(find_udt(fds, *it));

			for(std::vector<UDTSOCKET>::iterator it = exceptufds.begin(); it != exceptufds.end(); it++)
				exceptfds->push_back(find_udt(fds, *it));
		}
	}

	if(ntcp)
	{
		timeval tv;
		int timeOut = msTimeOut*ntcp/(ntcp+nudp+0.0001);
		tv.tv_sec = timeOut / 1000;
		tv.tv_usec = timeOut % 1000 * 1000;
		r2 = gWS32->select(0, &rset, NULL, &eset, &tv);

		if(r2 < 0)	conn_error = gWS32->WSAGetLastError();
		else
		{
			for(std::vector<SOCKET>::iterator it = tfds.begin(); it != tfds.end(); it++)
			{
				if(FD_ISSET(*it, &rset))
					readfds->push_back(find_tcp(fds, *it));

				if(FD_ISSET(*it, &eset))
					exceptfds->push_back(find_tcp(fds, *it));
			}
		}
	}

	return (r1 < 0 || r2 < 0) ? -1 : r1 + r2;
}