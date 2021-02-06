//
// Created by gyxma on 30.01.2021.
//

#include "WorkerBase.h"
#include "QueueBase.h"

WorkerBase::WorkerBase(QueueBase* parent)
    :mParent(parent),live(true){
    workerThread=std::thread(&WorkerBase::worker,this);
}

bool WorkerBase::isProcessing(){return live;}



void WorkerBase::worker(){
    auto start=semaphore.get_future();
    start.get();
    while(isProcessing()){
        execute();
        if(!mParent->checkTasks(this)){
            live=false;
        }
    }
    return;
}

WorkerBase::~WorkerBase(){
    if(workerThread.joinable()){workerThread.detach();}
}
