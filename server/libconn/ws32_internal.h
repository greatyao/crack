#ifndef WS32_INTERNAL_H
#define WS32_INTERNAL_H

#include <ws2tcpip.h>

struct ws32_loadlibrary_func
{
	int (__stdcall *WSAStartup)(WORD, LPWSADATA lpWSAData);
	int (__stdcall *WSACleanup)(void);
	SOCKET (__stdcall *socket)(int, int, int);
	int (__stdcall *WSAGetLastError)(void);
	SOCKET (__stdcall *accept)(SOCKET, struct sockaddr*, int*);
	int (__stdcall *bind)(SOCKET, const struct sockaddr*, int);
	int (__stdcall *closesocket)(SOCKET);
	int (__stdcall *connect)(SOCKET, const struct sockaddr*, int);
	int (__stdcall *listen)(SOCKET, int);
	int (__stdcall *recv)(SOCKET, char*, int, int);
	int (__stdcall *send)(SOCKET, const char*, int, int);
	int (__stdcall *getpeername)(SOCKET, struct sockaddr*, int*);
	int (__stdcall *getsockname)(SOCKET, struct sockaddr*, int*);
	int (__stdcall *getsockopt)(SOCKET, int, int, char*, int*);
	int (__stdcall *setsockopt)(SOCKET, int, int, const char*, int);
	int (__stdcall *select)(int, fd_set*, fd_set*, fd_set*, const struct timeval*);

	int (__stdcall *__WSAFDIsSet)(SOCKET fd, fd_set*);

	int (__stdcall *WSASendTo)(SOCKET,LPWSABUF, DWORD, LPDWORD, DWORD,
		const struct sockaddr*, int, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

	int (__stdcall *WSARecvFrom)(SOCKET, LPWSABUF,DWORD, LPDWORD, LPDWORD,
		struct sockaddr*, LPINT,  LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

	int (__stdcall *getaddrinfo)(PCSTR, PCSTR, const ADDRINFOA*, PADDRINFOA*);

	void (__stdcall *freeaddrinfo)(PADDRINFOA);

	int (__stdcall *getnameinfo)(const SOCKADDR*, socklen_t, PCHAR, DWORD, PCHAR, DWORD, INT);

	unsigned long (__stdcall *htonl)(unsigned long);
	unsigned short (__stdcall *htons)(unsigned short);
	unsigned long (__stdcall *ntohl)(unsigned long);
	unsigned short (__stdcall *ntohs)(unsigned short);

	char* (__stdcall *inet_ntoa)(struct in_addr);
	unsigned long (__stdcall *inet_addr)(const char *);
};

extern struct ws32_loadlibrary_func* gWS32;

#undef FD_ISSET
#define FD_ISSET(fd, set) gWS32->__WSAFDIsSet((SOCKET)(fd), (fd_set FAR *)(set))

#endif