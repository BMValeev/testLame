//
// Created by gyxma on 30.01.2021.
//

#include "QueueBase.h"
#include <iostream>
#include <sstream>



QueueBase::QueueBase(unsigned int workThreads)
    : mWorkThreads(workThreads)
    , waitSecBeforeKill(WAIT_SEC_BEFORE_KILL){

}

bool QueueBase::isProcessing(){
    for(auto obj : workersPool) {
        if(obj->isProcessing()){
            return true;
        }
    }
    return false;
}
