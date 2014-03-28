 #include <windows.h>
#include <stdio.h>
#include <math.h>

#include "split.h"

int main(int c,char *v[])
{
	struct crack_task* ct= new struct crack_task;
	ct->hashes = new struct crack_hash[2];
	ct->count = 2;


	ct->charset = charset_ascii;
	ct->startLength = 3;
	ct->endLength = 17;
	ct->count = 2;
	ct->algo = algo_md4;
	strcpy(ct->hashes[0].hash, "ABCD");
	strcpy(ct->hashes[1].hash, "1234");


	//²âÊÔ´úÂë
	csplit sp;
	unsigned n;
	struct crack_block * pcb = sp.split_normal(ct,n);

	for(unsigned i=0; i<n; i++)
	{
		printf("%d-%d:%d-%d %s %s\n",pcb[i].start,pcb[i].end,pcb[i].start2,pcb[i].end2, pcb[i].guid, pcb[i].john);
	}
	sp.release_splits((char *)pcb);




	ct->endLength = 9;
	pcb = sp.split_easy(ct,n);

	for(unsigned i=0; i<n; i++)
	{
		printf("%d-%d:%d-%d %s %s\n",pcb[i].start,pcb[i].end,pcb[i].start2,pcb[i].end2, pcb[i].guid, pcb[i].john);
	}
	sp.release_splits((char *)pcb);
	return 1;
}