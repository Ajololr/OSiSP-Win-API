#include "Queue.h"

void Queue::pushCS(ThreadTask queueElement){

    EnterCriticalSection(&pushCriticalSection);

    this->queue.push(queueElement);

    LeaveCriticalSection(&pushCriticalSection);
}

ThreadTask Queue::popCS(){

    EnterCriticalSection(&popCriticalSection);

    ThreadTask result = this->queue.front();
    this->queue.pop();

    LeaveCriticalSection(&popCriticalSection);

    return result;
}

Queue::Queue() {
    InitializeCriticalSection(&popCriticalSection);
    InitializeCriticalSection(&pushCriticalSection);
}

Queue::~Queue() {
    DeleteCriticalSection(&popCriticalSection);
    DeleteCriticalSection(&pushCriticalSection);
}