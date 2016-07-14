//
//  main.c
//  Cmpsc473p2
//
//  Created by Chen Chen on 3/21/15.
//  Copyright (c) 2015 Chen Chen. All rights reserved.
//

#define NUM 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/time.h>
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
    struct timeval start;
    struct timeval finish;
    FILE* f = fopen ("test_output5.txt", "w");
    int size;
    int sizeOfRegion = 1 << 20;// 1MB
    int a = psumeminit(0, sizeOfRegion);
    if (a == -1){
        fprintf(f, "Initialization failed!\n");
    }
    
    void* sample_malloc[20];
    void* sample_free[20];
    void* pointer_array[20][NUM];
    for (int i = 0; i < 20; i++){
        for (int j = 0; j < NUM; j++){
            pointer_array[i][j] = NULL;
        }
    }

    for (int i = 0; i < 16; i++){
        int duration_malloc = 0;
        for (int j = 0; j < NUM; ++j)
        {
            size = rand()%248 + 8;
            gettimeofday(&start,NULL);
            test* a = (test*)psumalloc(size);
            gettimeofday(&finish,NULL);
            
            duration_malloc += (finish.tv_sec*1000000 + finish.tv_usec)- (start.tv_sec*1000000 + start.tv_usec);

            write_test(a);
            read_test(a, f);
            if (a == NULL){
                fprintf(f, "No. %d: No extra space for allocation in memory!\n", j);
            }
            else{
                pointer_array[i][j] = a;
                fprintf(f, "NO.%d chunk has been allocated, the size is %d bytes\n", j, size);
            }
        }sample_malloc[i] = duration_malloc;
    }
    
   
    int half = 16;
    while (half != 0){
        half /= 2;
        int bound;
        if (half){
            bound = half * 2;
        }
        else{
            bound = 1;
        }
        for (int i = half; i < bound ; i++){
            int duration_free = 0;
            for (int j = 0; j < NUM; j++)
            {
                gettimeofday(&start,NULL);
                int a = psufree(pointer_array[i][j]);
                gettimeofday(&finish,NULL);
                if (a == 0){
                    fprintf(f, "No.%d chunk has been freed. \n", j);
                }
                else{
                    fprintf(f, "Can not free No.%d chunk. \n", j);
                }
            }sample_free[i] = duration_free;
        }
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