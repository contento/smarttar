#include <stdio.h>
#include <alloc.h>

#define NUM_PTRS 10
#define NUM_BYTES 16

int main(void)
{
	struct heapinfo hi;
	char *array[NUM_PTRS];
	int i;

	for (i=0;i<NUM_PTRS;i++)
		array[i]=(char *)malloc(NUM_BYTES);
	for (i=0;i<NUM_PTRS;i+=2)
		free(array[i]);
	malloc(65520);

	i = heapfillfree(0x5A);
	printf("heapfillfree() returns %d\n",i);
	
	i = heapcheckfree(0x5A);
	printf("heapcheckfree() returns %d\n",i);
		
	hi.ptr = NULL;
	printf("   Size   Status   Address\n");
	printf("   ----   ------   -------\n");
	while(heapwalk(&hi)==_HEAPOK)
		printf("%7lx    %s    %p\n",
			hi.size,hi.in_use?"used":"free",hi.ptr);
	
	return(0);
}