#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include "psumemory.h"

static int FitAlgo;

FreeList *head = NULL;


int psumeminit(int algo, int sizeOfRegion){
    if ((algo != 0 && algo != 1) || sizeOfRegion < 0){
        return -1;
    }
    int pagesize = getpagesize();
    if (sizeOfRegion % pagesize == 0){}
    else{
        sizeOfRegion = sizeOfRegion/pagesize * pagesize + pagesize; //round up
    }
    head = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    
    head->size = sizeOfRegion - sizeof(FreeList);
    head->next = NULL;
    head->prev = NULL;
    head->isfree = 1;
    FitAlgo = algo; 
 return 0;
    
}

void *SplitChunk(FreeList *ToBeSplit, int s){
    FreeList *new = NULL;
    new = (void*)ToBeSplit + s + sizeof(FreeList);
    new->size = ToBeSplit->size - s - sizeof(FreeList);
    new->isfree = 1;
    ToBeSplit->size = s;
    new->next = ToBeSplit->next;
    new->prev = ToBeSplit;
    if (ToBeSplit->next != NULL){
        ToBeSplit->next->prev = new;
    }
    ToBeSplit->next = new;
    return ToBeSplit;
    
}

void *psumalloc(int size){
    FreeList *tmp = head;
    FreeList *loop = head;
    FreeList *firstfree = head;
    
    while (firstfree != NULL){
       if ((firstfree->isfree == 1) && (firstfree->size > size + sizeof(FreeList))){
             tmp = firstfree;
	     break;
            }
          firstfree=firstfree->next;
        }
    
       
    if (firstfree == NULL){
        return NULL;
    }
    
    if (FitAlgo == 0){      //best fit
        while (loop != NULL){
            if ((loop->size >= size + sizeof(FreeList)) && loop->isfree == 1 && loop->size < tmp->size){
                tmp = loop;
            }
            loop = loop->next;
           
        }
    }
    if (FitAlgo == 1){      //worst fit
        while (loop != NULL){
            if ((loop->size >= size + sizeof(FreeList)) && loop->isfree == 1 && loop->size > tmp->size){
                tmp = loop;
            }
            loop = loop->next;
	   
        }
    }
    if (tmp->size < size || tmp->isfree == 0){
        return NULL;
    }
    if (tmp->size > size + sizeof(FreeList) && tmp->isfree == 1){
   
	tmp = SplitChunk(tmp,size);
   
    }
    tmp->isfree = 0;
    void* pointer =(void*) tmp + sizeof(FreeList);
    return  pointer;
}

FreeList *fusion(FreeList *ptr){
	if (ptr->next != NULL && ptr->next->isfree == 1){
            ptr->size += (ptr->next->size + sizeof(FreeList));
            ptr->next = ptr->next->next;
	    if(ptr->next != NULL){
		ptr->next->prev = ptr;
            }
         }
    return ptr;
}

int psufree(void *ptr){
    ptr = ptr - sizeof(FreeList);
    FreeList *sptr = head;
    while (sptr != ptr && sptr != NULL){
        sptr = sptr->next;
    }
    if (sptr == NULL){
        return -1;
    }

    if (sptr->isfree == 1) {
        return -1;
    }
    
    sptr->isfree = 1;
    
    //fusion
    
    if (sptr->next != NULL && sptr->next->isfree == 1){
         sptr = fusion(sptr);
    }
    if (sptr->prev != NULL && sptr->prev->isfree == 1){
        sptr = fusion(sptr->prev);
    } 
    
    return 0;
}

void psumemdump(){
    FreeList *loop = head;
    while(loop->next != NULL){
        printf("\nThis Chunk's size: %d next: %p prev: %p  isfree: %d \n", loop->size , loop->next , loop->prev , loop->isfree);  //%zu for size_t, %p for pointer.
        loop = loop->next;
    }
}




