//
// Created by gyxma on 29.01.2021.
//

#include "Coder.h"
#include <iostream>
#include <sstream>
#include <istream>
#include <regex>
#include <functional>

Coder::~Coder(){
    delete mMp3Encoder;
    delete mWavReader;
}
Coder::Coder(QueueBase* parent,std::string fileToEncode)
    :WorkerBase(parent)
    ,mFileToEncode(fileToEncode)    {
    mWavReader =new wavReader();
    mMp3Encoder = new mp3Encoder(mWavReader);
    semaphore.set_value(true);
}
std::string Coder::getFilenameToWrite(){
    std::string extensionWav=".wav";
    std::string extensionMp3=".mp3";
    if(!mFileToEncode.substr(mFileToEncode.size()-4,mFileToEncode.size()-1).compare(extensionWav)){
        std::string tempString=mFileToEncode.substr(0,mFileToEncode.size()-4);
        tempString=tempString.append(extensionMp3);
        return tempString;
    }
    return std::string();
}
void Coder::encode() {
    std::cout<<" | " <<std::this_thread::get_id()<<":thread, filename:"<<mFileToEncode<<" | "<<std::endl;
    try {
        std::string filenameToWrite=getFilenameToWrite();
        if(!filenameToWrite.size()){
            throw std::logic_error("wrong filename ");
        }
        std::ifstream mReadStream(mFileToEncode, std::ifstream::in);
        std::ofstream mWriteStream(getFilenameToWrite(), std::ofstream::out);
        if (!mWriteStream or !mReadStream) {
            mReadStream.close();
            mWriteStream.close();
            throw std::logic_error("files are not opened");
        }
        if (!mWavReader->setNewFile(&mReadStream)) {
            mReadStream.close();
            mWriteStream.close();
            throw std::logic_error("WAVE was not read");
        }
        if (!mMp3Encoder->encode(&mWriteStream)) {
            mReadStream.close();
            mWriteStream.close();
            throw std::logic_error("files are not encoded");
        }
        mReadStream.close();
        mWriteStream.close();
    }
    catch(const std::exception& e){
        std::cout<<" | " <<std::this_thread::get_id()<<":thread, filename:"<<mFileToEncode<<" wrong:"<<e.what()<<" | "<<std::endl;
        return;
    }
    return;
}

void Coder::setFilenameToEncode(std::string fileToEncode) {
    mFileToEncode= fileToEncode;
}
void Coder::execute() {
    encode();
}