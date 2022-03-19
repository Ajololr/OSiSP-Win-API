#include "ThreadpoolQueueProcessor.h"
#include <Windows.h>
#include <iostream>
#include <queue>
#include "QueueElem.h"

ThreadpoolQueueProcessor::ThreadpoolQueueProcessor(Queue *queue, vector<string> *Buffer) {
    InitializeThreadpoolEnvironment(&this->callbackEnvironment);

    this->pool = CreateThreadpool(nullptr);
    SetThreadpoolThreadMinimum(this->pool, 1);

    this->cleanupGroup = CreateThreadpoolCleanupGroup();
    SetThreadpoolCallbackCleanupGroup(&this->callbackEnvironment, this->cleanupGroup, nullptr);

    queue = queue;
    buffer = Buffer;
}

void ThreadpoolQueueProcessor::SortFile(ThreadTask queueElem){
    int startIndex = queueElem.startOffset - 1;
    int finishIndex = queueElem.finishOffset;
    sort(buffer->begin()+startIndex,buffer->begin() + finishIndex);
}

void ThreadpoolQueueProcessor::queueElementHandler(ThreadpoolQueueProcessor *queueHandler){
    ThreadTask queueElement = queueHandler->queue->popCS();
    queueHandler->SortFile(queueElement);
}

ThreadpoolQueueProcessor::~ThreadpoolQueueProcessor() {
    CloseThreadpoolCleanupGroup(this->cleanupGroup);
    CloseThreadpool(this->pool);
}

void ThreadpoolQueueProcessor::Process(int threadCount) {
    SetThreadpoolThreadMaximum(this->pool, threadCount);
    PTP_WORK work = CreateThreadpoolWork(WorkCallback, this, &this->callbackEnvironment);
    for (int i = 0; i < threadCount; i++) {
        SubmitThreadpoolWork(work);
    }
}

void ThreadpoolQueueProcessor::Wait() {
    CloseThreadpoolCleanupGroupMembers(this->cleanupGroup, false, nullptr);
}

void ThreadpoolQueueProcessor::WorkCallback(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
    auto *myThis = reinterpret_cast<ThreadpoolQueueProcessor *>(parameter);
    queueElementHandler(myThis);
}



