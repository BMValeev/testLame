//
// Created by gyxma on 01.02.2021.
//

#include "mp3Encoder.h"
#include <sstream>
#include <iostream>
#include <thread>


mp3Encoder::mp3Encoder(wavReader *audioFile)
    :mAudioFile(audioFile){ }

bool mp3Encoder::initLame(){
    lame_set_num_channels(gfp_, mAudioFile->getChannels());
    lame_set_in_samplerate(gfp_, mAudioFile->getSampleRateSec());
    lame_set_quality(gfp_, quality_);
    return(lame_init_params(gfp_) < 0)? false:true;
}

bool mp3Encoder::encode(std::ofstream* mFileToWrite,uint32_t nsamples){
    std::stringstream msg;
    msg<<"thread :"<<std::this_thread::get_id()<<",task failed: "<<"Here will be filename";
    if(!mFileToWrite) { // check is filestream correct
        msg<<", Invalid output"<<std::endl;
        std::cout << msg.str()<<std::endl;
        return false;
    }
    if(initLame()){ // initialize lame
        msg<<", lame initialization failed"<<std::endl;
        std::cout << msg.str()<<std::endl;
        return false;
    }
    // auto-determine sample size
    if(!nsamples) {
        nsamples = 4096 / mAudioFile->getByteRateSample() ;
    }
    bufferToWrite.resize(1.25 * nsamples + 7200); // worst-case estimate from lame/API

    // encode process
    while(mAudioFile->has_next()) {
        if(mAudioFile->readChunk(nsamples)){}
        const auto& inbuf = mAudioFile->getBuffer();

        int bytesToWrite(0);
        bytesToWrite=(mAudioFile->getChannels() > 1)
                ?lame_encode_buffer_interleaved(gfp_
                        , const_cast<int16_t*>(&inbuf[0])
                        , inbuf.size() / mAudioFile->getChannels()
                        , reinterpret_cast<unsigned char*>(&bufferToWrite[0])
                        , bufferToWrite.size())
                :lame_encode_buffer(gfp_
                        , const_cast<int16_t*>(&inbuf[0])
                        , nullptr
                        , inbuf.size()
                        , reinterpret_cast<unsigned char*>(&bufferToWrite[0])
                        , bufferToWrite.size());
        if(bytesToWrite<0){
            msg<<", lame_encode_buffer returned error"<<std::endl;
            std::cout << msg.str()<<std::endl;
            return false;
        }
        if(!mFileToWrite->write(bufferToWrite.data(), bytesToWrite)){
            msg<<", Writing to file failed"<<std::endl;
            std::cout << msg.str()<<std::endl;
            return false;
        }
    }

    // flush the rest
    bufferToWrite.resize(7200);
    auto bytesToWrite = lame_encode_flush(gfp_, reinterpret_cast<unsigned char*>(&bufferToWrite[0]), bufferToWrite.size());
    if(bytesToWrite < 0) {
        msg<<", lame_encode_flush returned error"<<std::endl;
        std::cout << msg.str()<<std::endl;
        return false;
    }
    if(bytesToWrite > 0 && !mFileToWrite->write(bufferToWrite.data(), bytesToWrite)) {
        msg<<", Writing to file failed"<<std::endl;
        std::cout << msg.str()<<std::endl;
        return false;
    }
    return true;
}