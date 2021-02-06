//
// Created by gyxma on 29.01.2021.
//

#ifndef TESTLAME_CODER_H
#define TESTLAME_CODER_H
#include <map>

#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include "../baseClasses/WorkerBase.h"
#include "../audio/wavReader.h"
#include "../audio/mp3Encoder.h"

class Coder final: public WorkerBase {
public:
    Coder(QueueBase* parent,std::string fileToEncode);
    ~Coder();
    void encode();
    void setFilenameToEncode(std::string fileToEncode);
    std::string getFilenameToWrite();
    std::string getFilename(){return mFileToEncode;}
    virtual void execute() override;
private:
    wavReader* mWavReader;
    mp3Encoder* mMp3Encoder;
    std::string mFileToEncode;
};


#endif //TESTLAME_CODER_H
