#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>



typedef struct FreeList {
    int size;
    struct FreeList *next;
    struct FreeList *prev;
    int isfree;
} FreeList;

void *SplitChunk(FreeList *ToBeSplit, int s);
int psumeminit(int algo, int sizeOfRegion);
void *psumalloc(int size);
int psufree(void *ptr);
void psumemdump();
