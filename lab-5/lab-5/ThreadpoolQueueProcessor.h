#ifndef LAB5_THREADPOOLQUEUEPROCESSOR_H
#define LAB5_THREADPOOLQUEUEPROCESSOR_H

#include <Windows.h>
#include <queue>
#include <string>

#include "QueueElem.h"
#include "Queue.h"

using namespace std;

class ThreadpoolQueueProcessor {
public:
    Queue *queue;
    vector<string> *buffer;

    ThreadpoolQueueProcessor(Queue *queue, vector<string>* Buffer);

    ~ThreadpoolQueueProcessor();

    void Process(int threadCount);

    void Wait();

private:
    PTP_POOL pool;
    PTP_CLEANUP_GROUP cleanupGroup;
    TP_CALLBACK_ENVIRON callbackEnvironment{};

    void SortFile(ThreadTask queueElem);

    static void queueElementHandler(ThreadpoolQueueProcessor *queueHandler);

    static void CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
};


#endif
