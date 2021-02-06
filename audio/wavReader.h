//
// Created by gyxma on 01.02.2021.
//

#ifndef TESTLAME_WAVREADER_H
#define TESTLAME_WAVREADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
class wavReader {
public:
    wavReader();
    bool setNewFile(std::ifstream* fileToRead);
    bool readChunk(uint32_t nsamples);
    std::vector<int16_t>& getBuffer();
    bool has_next();
    uint16_t getChannels(){return mChannels; }
    uint32_t getSampleRateSec(){return mSampleRateSec; }
    uint32_t getAverageByteRateSec(){return mAverageByteRateSec; }
    uint16_t getBlockAlign(){return mBlockAlign; }
    uint16_t getBitRateSample(){return mBitRateSample; }
    uint32_t getByteRateSample(){return mByteRateSample; }
    uint32_t getQuality(){return mByteRateSample; }
    void skipChunk();
    bool seekDataBlock();
private:
    template<typename T>
    T readInteger();
    bool readHeader();
    std::string readString(unsigned int bytesToRead);
    std::ifstream* mFileToRead;
    uint16_t mChannels,mBlockAlign,mBitRateSample;
    uint32_t mSampleRateSec, mAverageByteRateSec, mByteRateSample;
    uint32_t remainingChunkSize;
    std::vector<int16_t> bufferToEncode;
};


#endif //TESTLAME_WAVREADER_H
