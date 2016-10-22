#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <cstring>
#include <ucontext.h>

using coro_func_t=void (*)();

#define DFT_STACK_PER_CORO (1<<12)
class Schedule;

class Coroutine{
public:
    Coroutine *next;

    union{
        struct{
            int idle : 1;
            int ready : 1;
            int running : 1;
            int suspended : 1;
        };
        int8_t status;
    };
    

    Schedule* sch;
    coro_func_t func;
    ucontext_t ctx;
	ucontext_t* father;
    char stack[DFT_STACK_PER_CORO];

    void reset(coro_func_t func){
		sch = nullptr;
		this->func = func;
		memset(stack,0,sizeof(stack));
    }
};


class Condition{
private:
    intptr_t victims;
public:
    Condition():victims(0){}
    inline void add_victim(Coroutine* victim){
        victim->next = reinterpret_cast<Coroutine *>(victims);
        victims = reinterpret_cast<intptr_t>(victim);
    }
};


class CoroutineQueue{
private:
    Coroutine* _head;
    Coroutine* _tail;
    uint64_t _size;

public:
    CoroutineQueue()
        : _head(nullptr)
        , _tail(nullptr)
        , _size(0){}

    void enqueue(Coroutine * c){
        if(nullptr == _head) _head = c;
        else _tail->next = c;
        _tail = c;
        c->next = nullptr;
        _size++;
    }

    Coroutine* dequeue(){
        Coroutine* ret = _head;
        if(nullptr != ret){
            _head = _head->next;
            ret->next = nullptr;
            _size -- ;
        }else{
            _tail = nullptr;
        }
        return ret;
    }

    Coroutine* front(){
        return _head;
    }
    uint64_t size(){
        return _size;
    }

};



//<template F>
class Schedule{
public:
    CoroutineQueue _candidates;
    //CoroutineQueue _idles;

    ucontext_t main;
    Coroutine * _executant;

    bool running;


    Schedule() : running( true )
               , _executant(nullptr) {}

    Coroutine * get_cur_coro(){
        return _executant;
    }

	void await(){
		do{
			Coroutine * front = _candidates.dequeue();
			front -> running = 1;
			front -> ready = 0;
			_executant = front;
			running = false;
			swapcontext(&main, &(front->ctx));
			running = true;
		}while(_candidates.size() != 0);
	}

};

Coroutine * _spawn_coro(coro_func_t func){
    Coroutine * coro = new Coroutine;
    coro->reset(func);
    return coro;
}

void _delete_coro(Coroutine * coro){
    free(coro);
    coro = nullptr;
}

void tramp(void * arg);

void coroutine_create(Schedule * sch, coro_func_t func){
    Coroutine * coro = _spawn_coro( func );
    getcontext(&(coro->ctx));
    coro->ctx.uc_stack.ss_sp = coro->stack;
    coro->ctx.uc_stack.ss_size = DFT_STACK_PER_CORO;
    coro->ctx.uc_stack.ss_flags = 0;
    coro->ctx.uc_link = &(sch->main);
    coro->ready = 1;
    coro->sch = sch;

	if(sch->running){
		coro->father = &sch->main;
	}else{
		coro->father = &(sch->_executant->ctx);
	}

    makecontext(&(coro->ctx), (void (*)(void))(tramp), 1, reinterpret_cast<void*>(coro));
    swapcontext(coro->father, &(coro->ctx));
    sch->_candidates.enqueue( coro );
}

void coroutine_yeild(Schedule * sch){
    Coroutine * next_coro = sch->_candidates.dequeue();
	if( nullptr == next_coro ) return;

    Coroutine * coro = sch->_executant;
    sch->_candidates.enqueue( coro );
    sch->_executant = next_coro;

    coro->running = 0;
    coro->ready = 1;
    next_coro->ready = 0;
    next_coro->running = 1;

    swapcontext( &(coro->ctx), &(next_coro->ctx) );
}

void tramp(void * arg){
    Coroutine * cur = reinterpret_cast<Coroutine*>(arg);
    swapcontext( &(cur->ctx), cur->father );

    // this time it is a real swapcontext
    cur->func();
    cur->running = 0;
    cur->idle = 1;
}


#endif
