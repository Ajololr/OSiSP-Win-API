#include <iostream>
#include <windows.h>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include "ThreadTask.h"

#define INPUT_PATH  "E:\\University\\OSiSP-Win-API\\lab-5\\input.txt"
#define OUTPUT_PATH  "E:\\University\\OSiSP-Win-API\\lab-5\\output.txt"
#define STRING_SIZE 1024
#define THREAD_COUNT 3

using namespace std;

queue <ThreadTask> originalQueue;
vector<string> buffer;

CRITICAL_SECTION pushCriticalSection;
CRITICAL_SECTION popCriticalSection;

void pushInQueue(ThreadTask queueElement) {

    EnterCriticalSection(&pushCriticalSection);

    originalQueue.push(queueElement);

    LeaveCriticalSection(&pushCriticalSection);
}

ThreadTask popFromQueue() {
    EnterCriticalSection(&popCriticalSection);

    ThreadTask result = originalQueue.front();
    originalQueue.pop();

    LeaveCriticalSection(&popCriticalSection);

    return result;
}


void SortFile(ThreadTask queueElem) {
    int startIndex = queueElem.startOffset - 1;
    int finishIndex = queueElem.finishOffset;
    sort(buffer.begin() + startIndex, buffer.begin() + finishIndex);
}

void queueElementHandler() {
    ThreadTask queueElement = popFromQueue();
    SortFile(queueElement);
    return;
}

void queueHandle(int countOfThreads) {
    HANDLE* threadArray = new HANDLE[countOfThreads];
    for (int i = 0; i < countOfThreads; i++)
    {
        if (!originalQueue.empty()) {
            HANDLE HThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(queueElementHandler), NULL, 0, NULL);
            threadArray[i] = HThread;
        }
    }
    for (int i = 0; i < THREAD_COUNT; i++) {
        WaitForSingleObject(threadArray[i], INFINITE);
    }
    for (int i = 0; i < countOfThreads; i++) {
        CloseHandle(threadArray[i]);
    }
    free(threadArray);
}

int main() {
    InitializeCriticalSection(&pushCriticalSection);
    InitializeCriticalSection(&popCriticalSection);

    ifstream fin(INPUT_PATH);
    long stringCount = 0;
    char str[STRING_SIZE];

    int j = 0;
    while (!fin.eof()) {
        fin.getline(str, STRING_SIZE);
        stringCount++;
        buffer.resize(stringCount);
        buffer[j] = str;
        if (buffer[j].size() && buffer[j][buffer[j].size() - 1] != '\n') {
            buffer[j] += '\n';
        }
        j++;
    }

    fin.close();

    int countOfThreads;
    long stringCountForThread;
    int lastThreadCount;
    if (stringCount > THREAD_COUNT) {
        countOfThreads = THREAD_COUNT;
        stringCountForThread = stringCount / THREAD_COUNT;
        lastThreadCount = stringCount % THREAD_COUNT;
    }
    else {
        stringCountForThread = 1;
        lastThreadCount = 0;
        countOfThreads = stringCount;
    }
    for (int i = 0; i < countOfThreads; i++) {
        ThreadTask queueTask;
        if (i != countOfThreads - 1) {
            queueTask = ThreadTask(stringCountForThread * i + 1, stringCountForThread * (i + 1));
        }
        else {
            queueTask = ThreadTask(stringCountForThread * i + 1, stringCountForThread * (i + 1) + lastThreadCount);
        }
        pushInQueue(queueTask);
    }

    queueHandle(countOfThreads);

    int countSort = stringCount / stringCountForThread - 1;
    for (int i = 0; i < countSort; i++) {
        if (i == (countSort - 1)) {
            std::inplace_merge(buffer.begin(), buffer.begin() + stringCountForThread * (i + 1),
                buffer.end());
        }
        else {
            std::inplace_merge(buffer.begin(), buffer.begin() + stringCountForThread * (i + 1),
                buffer.begin() + stringCountForThread * (i + 2));
        }
    }

    ofstream fout(OUTPUT_PATH);
    for (int i = 0; i < buffer.size(); i++) {
        fout << buffer[i];
    }
    fout.close();

    DeleteCriticalSection(&pushCriticalSection);
    DeleteCriticalSection(&popCriticalSection);
    return 0;
}
