//
// Created by gyxma on 01.02.2021.
//

#ifndef TESTLAME_MP3ENCODER_H
#define TESTLAME_MP3ENCODER_H

#include "lame.h"
#include "wavReader.h"

class mp3Encoder {
public:
    mp3Encoder(wavReader *audioFile);
    bool encode(std::ofstream* mFileToWrite,uint32_t nsamples = 0);

    //void setOutput(std::ostream* fileToWrite);
protected:
    bool initLame();
private:
    //std::ostream* mFileToWrite;
    wavReader *mAudioFile;
    lame_global_flags* gfp_;
    int quality_;
    std::string bufferToWrite;
};


#endif //TESTLAME_MP3ENCODER_H
