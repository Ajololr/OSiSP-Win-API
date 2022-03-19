#ifndef LAB5_QUEUEELEM_H
#define LAB5_QUEUEELEM_H


class ThreadTask {
public:
    long startOffset{ 0 };
    long finishOffset{ 0 };

    ThreadTask(long start, long finish);

    ThreadTask();
};



#endif
