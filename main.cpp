#include "Coroutine.hpp"
#include <iostream>
#include <cstdio>

Schedule * sch;

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
        printf("I will follow you forever %d\n",i);
        coroutine_yeild(sch);
    }
}

void func3(){
   coroutine_create(sch, func2); 
   coroutine_yeild(sch);
   coroutine_create(sch, func1); 
}

int main(){
    sch = new Schedule();
    coroutine_create(sch, func1);
    coroutine_create(sch, func3);
    sch->run();
    return 0;
}

