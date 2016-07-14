#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/time.h>
#include "psumemory.h"

#define NUM 100

typedef struct _test{
    int a;
    int b;
    int c;
    char one;
    char two;
    char three;
} test;

void write_test(test* ptr){
    if (ptr == NULL){
        return;
    }
    ptr->a = 1;
    ptr->b = 2;
    ptr->c = 3;
    ptr->one = 'A';
    ptr->two = 'B';
    ptr->three = 'C';
}

void read_test(test* ptr, FILE* f){
    if (ptr == NULL){
        return;
    }
    fprintf(f, "This struct contains: %d, %d, %d, %c, %c and %c. \n", ptr->a, ptr->b, ptr->c, ptr->one, ptr->two, ptr->three);
}

int main(){
    struct timeval start;
    struct timeval finish;
	FILE* f = fopen ("test_output3.txt", "w");
	int size;
	int sizeOfRegion = 1 << 20;// 1MB
	int a = psumeminit(0, sizeOfRegion);
	if (a == -1){
		fprintf(f, "Initialization failed!\n");
	}
    void* sample_malloc[20];
    void* sample_free[20];
    for (int i = 0; i < 20; i++){
		void* pointer_array[NUM];
        for (int i = 0; i < NUM; i++){
            pointer_array[i] = NULL;
        }
        int duration_malloc = 0;
		for (int i = 0; i < NUM; ++i)
		{
			if (i % 2 == 0){
				size = 64;
			}
			else{
				size = 64 * 1024;
			}
            gettimeofday(&start,NULL);
			test* a = (test*)psumalloc(size);
            gettimeofday(&finish,NULL);
            
             duration_malloc += (finish.tv_sec*1000000 + finish.tv_usec)- (start.tv_sec*1000000 + start.tv_usec);
            
            write_test(a);
            read_test(a, f);
			if (a == NULL){
				fprintf(f, "No extra space for allocation in memory!\n");
			}
			else{
				pointer_array[i] = a;
				fprintf(f, "NO.%d chunk has been allocated. the size is %d bytes\n", i, size);
			}
        }sample_malloc[i] = duration_malloc;

        int duration_free = 0;
		for (int i = 0; i < NUM; ++i)
		{
			gettimeofday(&start,NULL);
            int a = psufree(pointer_array[i]);
            gettimeofday(&finish,NULL);
			if (a == 0){
				fprintf(f, "No.%d chunk has been freed. \n", i);
			}
			else{
				fprintf(f, "Can not free No.%d chunk. \n", i);
			}
		}sample_free[i] = duration_free;
    }
    qsort((void *)sample_malloc, 20, sizeof(sample_malloc[0]), (int (*)(const void*, const void*))strcmp );
    qsort((void *)sample_free, 20, sizeof(sample_free[0]), (int (*)(const void*, const void*))strcmp );
    printf("\n25th percentile psumalloc=%d",sample_malloc[4]);
    printf("\n75th percentile psumalloc=%d",sample_malloc[14]);
    printf("\nAverage psumalloc=%d\n",sample_malloc[9]);
    printf("\n25th percentile psufree=%d",sample_free[4]);
    printf("\n75th percentile psufree=%d",sample_free[14]);
    printf("\nAverage psufree=%d\n",sample_free[9]);
	fclose(f);
	return 0;
}