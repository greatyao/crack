 #include <windows.h>
#include <stdio.h>
#include <math.h>

#include "split.h"

int main(int c,char *v[])
{
	struct crack_task* ct= (struct crack_task*)malloc(sizeof(*ct)+2*sizeof(crack_hash));

	ct->charset = charset_ascii;
	ct->startLength = 3;
	ct->endLength = 17;
	ct->count = 2;
	strcpy(ct->hashes[0].hash, "ABCD");


	//²âÊÔ´úÂë
	csplit sp;
	unsigned n;
	struct crack_block * pcb = sp.split_easy(ct,n);

	for(unsigned i=0; i<n; i++)
	{
		printf("%d-%d:%d-%d\n",pcb[i].start,pcb[i].end,pcb[i].start2,pcb[i].end2);
	}
	sp.release_splits((char *)pcb);
	return 1;
}