#ifndef LAB5_QUEUE_H
#define LAB5_QUEUE_H

#include <queue>
#include <windows.h>
#include "ThreadTask.h"

using namespace std;

class Queue {
public:
    queue<ThreadTask> queue;
    CRITICAL_SECTION pushCriticalSection;
    CRITICAL_SECTION popCriticalSection;

    Queue();
    ~Queue();

    void pushCS(ThreadTask Element);

    ThreadTask popCS();

};


#endif
