//
// Created by gyxma on 30.01.2021.
//

#ifndef TESTLAME_QUEUEBASE_H
#define TESTLAME_QUEUEBASE_H
#include "WorkerBase.h"
#include <vector>
#include <mutex>
#define WAIT_SEC_BEFORE_KILL 2

class QueueBase {
public:
    QueueBase(unsigned int workThreads=1);
    ~QueueBase() {  }
    bool isProcessing();
    virtual bool checkTasks(WorkerBase* worker)=0;
    virtual void addWorker()=0;
protected:
    unsigned int mWorkThreads;
    std::mutex taskGuard;
    std::vector<WorkerBase*> workersPool;
    unsigned int waitSecBeforeKill;
};


#endif //TESTLAME_QUEUEBASE_H
