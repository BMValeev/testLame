//
// Created by gyxma on 30.01.2021.
//

#ifndef TESTLAME_WORKERBASE_H
#define TESTLAME_WORKERBASE_H

#include <thread>
#include <atomic>
#include <future>


class QueueBase;
class WorkerBase {
public:
    WorkerBase(QueueBase* parent);
    ~WorkerBase();
    bool isProcessing();
    virtual void execute()=0;

protected:
    std::promise<bool> semaphore;
private:
    void worker();
    QueueBase* mParent;
    std::thread workerThread;
    std::atomic_bool live;
};


#endif //TESTLAME_WORKERBASE_H
