#ifdef STACK_H
#define STACK_H


void 
makestack(void **me, void **stack){
    asm("_makestack");
}


void *
_swapstacks_(void **olds, void **news, void *ret){
    asm ("_swapstacks");
}


static inline 
void * 
swapstacks(void **olds, void **news, void *ret){
    asm volatile("":::"memory");
    return _swapstacks_(olds, news, ret);
}




#endif
