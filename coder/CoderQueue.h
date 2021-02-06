//
// Created by gyxma on 30.01.2021.
//

#ifndef TESTLAME_CODERQUEUE_H
#define TESTLAME_CODERQUEUE_H

#include <vector>
#include "Coder.h"
#include "../baseClasses/QueueBase.h"
class CoderQueue final : virtual public QueueBase {
public:
    CoderQueue(std::vector<std::string> filesToEncode,unsigned int coderThreads=1);
    ~CoderQueue();
    std::string getFile();
    virtual bool checkTasks(WorkerBase* worker) override ;
    virtual void addWorker() override;
private:
    std::vector<std::string> mFilesToEncode;
};


#endif //TESTLAME_CODERQUEUE_H
