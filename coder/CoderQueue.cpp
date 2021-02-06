//
// Created by gyxma on 30.01.2021.
//

#include "CoderQueue.h"
#include <chrono>
#include <iostream>
#include <sstream>

std::string CoderQueue::getFile(){
    std::lock_guard<std::mutex> lock(taskGuard);
    if(mFilesToEncode.size()>0){
        std::string newFilename=mFilesToEncode.back();
        mFilesToEncode.pop_back();
        return newFilename;
    }
    return std::string();
}

CoderQueue::CoderQueue(std::vector<std::string> filesToEncode,unsigned int coderThreads)
    :QueueBase(coderThreads)
    ,mFilesToEncode(filesToEncode){
     while(workersPool.size()<mWorkThreads){
        addWorker();
    }
}
bool CoderQueue::checkTasks(WorkerBase* worker) {
    Coder* coder = dynamic_cast<Coder*>(worker);
    std::string filename=getFile();
    if(filename.size()==0){
        return false;
    }
    coder->setFilenameToEncode(filename);
    return true;
}


void CoderQueue::addWorker() {
    std::string filename=getFile();
    if(filename.size()==0){
        return;
    }
    workersPool.push_back(reinterpret_cast<WorkerBase*>(new Coder(this,filename)));
}


CoderQueue::~CoderQueue(){
    unsigned int i(0);
    while(isProcessing() and i < waitSecBeforeKill * 10){
        i++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for (auto obj: workersPool){
        Coder * coder=dynamic_cast<Coder*>(obj);
        delete coder;
    }
}
