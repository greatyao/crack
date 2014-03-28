#include "guidgenerator.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

#if defined(WIN32) || defined(WIN64)	
#include <windows.h>
typedef CRITICAL_SECTION pthread_mutex;
#define pthread_mutex_init(m, v) InitializeCriticalSection(m)
#define pthread_mutex_lock(m) EnterCriticalSection(m)
#define pthread_mutex_unlock(m)	LeaveCriticalSection(m)
#define snprintf _snprintf
#else
#include <unistd.h>
#include <pthread.h>
#endif

static unsigned int accu = 0;
pthread_mutex mutex;

//创建一个guid字符串
void new_guid(char* guid, int size)
{
	if(accu == 0)
	{
		pthread_mutex_init(&mutex, NULL);
		accu = 1;
	}
	
	pthread_mutex_lock(&mutex);
	time_t t;									
	struct tm *tm1;								
	time(&t);									
	tm1 = localtime(&t);
	char buffer[256];
	strftime(buffer, 256, "%Y%m%d-%H%M%S", tm1);
	accu ++;

	snprintf(guid, size, "%s-%08x", buffer, accu);
	pthread_mutex_unlock(&mutex);	
}
