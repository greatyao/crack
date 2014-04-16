 #include <windows.h>
#include <stdio.h>
#include <math.h>

#include "split.h"

int main(int c,char *v[])
{
	struct crack_task* ct= new struct crack_task;
	ct->hashes = new struct crack_hash[2];
	ct->count = 2;


	ct->charset = charset_num;
	ct->startLength = 4;
	ct->endLength = 12;
	ct->count = 2;
	ct->algo = algo_md5md5;
	strcpy(ct->hashes[0].hash, "ABCD");
	strcpy(ct->hashes[1].hash, "1234");

	ct->type = bruteforce;


	//测试代码
	csplit sp;
	unsigned n;
	struct crack_block * pcb = sp.split_intelligent(ct,n);

	if(pcb)
	for(unsigned i=0; i<n; i++)
	{
		if(pcb[i].type == bruteforce)
			printf("暴力 [%d %d] %s %s %d\n", pcb[i].start, pcb[i].end, pcb[i].guid, pcb[i].john, pcb[i].hash_idx);
		else if(pcb[i].type == mask)
			printf("掩码 %d %s %s %d\n", pcb[i].maskLength, pcb[i].guid, pcb[i].john, pcb[i].hash_idx);
	}
	printf("份数%d\n", n);

	sp.release_splits((char *)pcb);


	return 1;
}