 #include <windows.h>
#include <stdio.h>
#include <math.h>

#include "split.h"

int main(int c,char *v[])
{
	struct crack_task ct={0};

	ct.charset = charset_ascii;
	ct.startLength = 3;
	ct.endLength = 7;


	//²âÊÔ´úÂë
	csplit sp;
	unsigned n;
	sp.init(&ct);
	struct crack_block * pcb = sp.split_default(n);

	for(int i=0; i<n; i++)
	{
		printf("%d-%d:%d-%d\n",pcb[i].start,pcb[i].end,pcb[i].start2,pcb[i].end2);
	}
	sp.release_splits((char *)pcb);
	return 1;
}