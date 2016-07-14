#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "psumemory.h"

#define NUM 100

typedef struct _test{
    int a;
    char one;
    char two;
} test;

void write_test(test* ptr){
    if (ptr == NULL){
        return;
    }
    ptr->a = 1;
    ptr->one = 'A';
    ptr->two = 'B';
}

void read_test(test* ptr, FILE* f){
    if (ptr == NULL){
        return;
    }
    fprintf(f, "This struct contains: %d, %c and %c. \n", ptr->a, ptr->one, ptr->two);
}


int main(){
    FILE* f = fopen ("test_output7.txt", "w");
    int size;
    int sizeOfRegion = 1 << 20;// 1MB
    int a = psumeminit(0, sizeOfRegion);
    if (a == -1){
        fprintf(f, "Initialization failed!\n");
    }
    void* sample[10];
    for (int i = 0; i < 10; i++){
        void* pointer_array[NUM];
        for (int i = 0; i < NUM; ++i){
            pointer_array[i] = NULL;
        }
        int duration = 0;
        for (int i = 0; i < NUM; ++i)
        {
            size = rand()%248 + 8;
            gettimeofday(&start,NULL);
            test* a = (test*)psumalloc(size);
            gettimeofday(&finish,NULL);
            
            duration += (finish.tv_sec*1000000 + finish.tv_usec)- (start.tv_sec*1000000 + start.tv_usec);
            
            write_test(a);
            read_test(a, f);
            if (a == NULL){
                fprintf(f, "No.%d : No extra space for allocation in memory!\n", i);
            }
            else{
                pointer_array[i] = a;
                fprintf(f, "NO.%d chunk has been allocated. the size is %d bytes \n", i, size);
            }
        }sample[i] = duration;
        
        for (int i = 0; i < NUM; ++i)
        {
            int a = psufree(pointer_array[i]);
            if (a == 0){
                fprintf(f, "No.%d chunk has been freed. \n", i);
            }
            else{
                fprintf(f, "Can not free No.%d chunk. \n", i);
            }
        }
    }
    qsort((void *)sample, LENGTH(sample), sizeof(sample[0]), (int (*)(const void*, const void*))strcmp );
    printf("\n25th percentile psumalloc=%d",sample[2]);
    printf("\n75th percentile psumalloc=%d",sample[7]);
    printf("\nAverage psumalloc=%d",sample[4])；
    fclose(f);
    return 0;
}