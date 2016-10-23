#include "Coroutine.hpp"
#include <iostream>
#include <cstdio>

Schedule * sch;
Condition * cond;
int res;

void func1(){
    int cnt = 1;
    for(int i = 0 ; i < 5 ; i ++){
        printf("hello %d\n",cnt++);
        coroutine_yeild(sch);
    }
}

void func2(){
    int cnt = 2;
    for(int i = 0 ; i < 10 ; i ++){
        printf("I will follow this %d\n",i);
        coroutine_yeild(sch);
    }
}

void func3(){
    for(int i = 0 ; i < 10 ; i++){
        if(res == 0){
            printf("I will wait for a res\n");
            coroutine_wait(sch, cond);
        }
        res--;
        printf("I got one\n");
    }
}

void func3_(){
    for(int i = 1 ; i < 9 ; i ++){
        coroutine_yeild(sch);
        printf("produce one\n");
        res++;
        coroutine_signal(sch, cond);
    }
}

void func4(){
   coroutine_create(sch, func2); 
   coroutine_yeild(sch);
   coroutine_create(sch, func1); 
   coroutine_yeild(sch);
   coroutine_create(sch, func3); 
   coroutine_yeild(sch);
   coroutine_create(sch, func3_); 
}



int main(){
    sch = new Schedule();
    cond = new Condition();
    res = 1;
    coroutine_create(sch, func1);
    coroutine_create(sch, func4);
    sch->await();
    return 0;
}

