//
// Created by gyxma on 01.02.2021.
//


#include "wavReader.h"

#include <algorithm> // generate_n
#include <exception> // terminate
#include <iterator> // ifstream_iterator
#include <string>
#include <thread>

#include <utility> // swap

wavReader::wavReader()
    :remainingChunkSize(0){ }

std::vector<int16_t>& wavReader::getBuffer(){
    return bufferToEncode;
}
bool wavReader::has_next(){
     mFileToRead->peek(); return mFileToRead->good();
}
bool wavReader::setNewFile(std::ifstream* fileToRead){
    if(remainingChunkSize){return false;}
    mFileToRead=fileToRead;
    if(!readHeader()){return false;}
    return true;
}
template<typename T>
T wavReader::readInteger(){
    static_assert(std::is_integral<T>::value, "Input type is not integer");
    std::string bytes(sizeof(T), 0);
    mFileToRead->read(&bytes[0], sizeof(T));
    if(__BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__) {
        switch(sizeof(T)) {
            case 2:
                std::swap(bytes[0], bytes[1]);
                break;
            case 4:
                std::swap(bytes[0], bytes[3]);
                std::swap(bytes[1], bytes[2]);
                break;
            case 8:
                std::swap(bytes[0], bytes[7]);
                std::swap(bytes[1], bytes[6]);
                std::swap(bytes[2], bytes[5]);
                std::swap(bytes[3], bytes[4]);
                break;
            case 1:
                break;
            default:
                std::terminate();
        }
    }
    return *reinterpret_cast<const T*>(bytes.data());
}
bool wavReader::readHeader()    {
    std::stringstream msg;
    msg<<"thread :"<<std::this_thread::get_id()<<",task failed: ";
    if(readString(4) != "RIFF") {
        msg<<"Not RIFF file";
        std::cout<<msg.str()<<std::endl;
        return false;
    }
    uint32_t in_size = readInteger<uint32_t>();
    if(readString(4) != "WAVE") {
        msg<<"Not WAVE file";
        std::cout<<msg.str()<<std::endl;
        return false;
    }
    while(mFileToRead->good() && readString(4) != "fmt ") {
        skipChunk();
    }
    if(mFileToRead->good()){
        msg<<"Don't find format chunk";
        std::cout<<msg.str()<<std::endl;
        return false;
    }
    auto fmt_chunk_size = readInteger<uint32_t>();

    if(readInteger<uint16_t>() != 0x1){
        msg<<"No PCM format";
        std::cout<<msg.str()<<std::endl;
        return false;
    }

    mChannels = readInteger<uint16_t>();
    mSampleRateSec = readInteger<uint32_t>();
    mAverageByteRateSec = readInteger<uint32_t>();
    mBlockAlign = readInteger<uint16_t>();
    mBitRateSample = readInteger<uint16_t>();
    mByteRateSample = mBlockAlign / mChannels; // sample size is M-byte with M = block-align / Nchannels
    mFileToRead->ignore(fmt_chunk_size - 16); // skip the rest of the fmt header ...
    return true;
}

std::string wavReader::readString(unsigned int bytesToRead){
    std::string bytes(bytesToRead, 0);
    mFileToRead->read(&bytes[0], bytesToRead);
    return bytes;
}


// Skips the next chunk of the wave file. Assumes that the ckID has
// already been read and the next 4 bytes contain the chunk size.
void wavReader::skipChunk(){
    auto chunk_size = readInteger<uint32_t>();
    mFileToRead->ignore(chunk_size);
}

bool wavReader::seekDataBlock(){
    mFileToRead->peek(); // make sure eof is set properly if we are at the end of the file
    while(mFileToRead->good() && readString(4) != "data") {
        skipChunk();
    }
    return (!mFileToRead)
           ? false
           : true;
}

// Read the next sample from the current data chunk. Seek the next
// chunk if the current chunk is finished.
bool wavReader::readChunk(uint32_t nsamples){
    if(!remainingChunkSize) {
        seekDataBlock();
        if(mFileToRead->eof()) {
            bufferToEncode.resize(0);
            return true;
        }

        remainingChunkSize = readInteger<uint32_t>();
        if(!remainingChunkSize) {
            return false;
        }
    }
    bufferToEncode.resize(nsamples * getChannels(), 0);
    if(bufferToEncode.size() * getByteRateSample() > remainingChunkSize) {
        bufferToEncode.resize(remainingChunkSize / getByteRateSample(), 0);
    }
    if(getBitRateSample() == 16) {
        mFileToRead->read(reinterpret_cast<char*>(&bufferToEncode[0]), 2*bufferToEncode.size());
        if(__BYTE_ORDER__==__ORDER_BIG_ENDIAN__) {
            for(int16_t& s : bufferToEncode) {
                s = ((s & 0xFF) << 8) | ((s & 0xFF00) >> 8);
            }
        }
    } else if(getBitRateSample() == 8) {
        auto input_it = std::istream_iterator<unsigned char>(*mFileToRead);
        std::generate_n(bufferToEncode.begin(), bufferToEncode.size(),
                        [&input_it]() -> int16_t {
                            return 257*(*input_it++) - 32768; // [0,255] to [-32768,32767]
                        });
    } else {
        return false;
    }
    remainingChunkSize -= mFileToRead->gcount();
    return true;
}


//
//
//template<std::size_t Width>
//inline std::string read_string(std::ifstream& in)
//{
//    std::string bytes(Width, 0);
//    in.read(&bytes[0], Width); // explictely allowed since C++11
//    return bytes;
//} // read_string
//
//
//
//std::string checkEndiannes(){
//    if (__BYTE_ORDER__ ==__ORDER_BIG_ENDIAN__  ) { return "bigEndian"; }
//    else if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) { return "littleEndian"; }
//    else{return "undefined";}
//}
//
//template<typename Result>
//inline Result read_integral(std::ifstream& fileToRead){
//    static_assert(std::is_integral<Result>::value, "only integral types");
//    static thread_local std::string bytes(sizeof(Result), 0);
//    fileToRead.read(&bytes[0], sizeof(Result));  // explictely allowed since C++11
//    if(checkEndiannes().compare("littleEndian")) {
//        switch(sizeof(Result)) {
//            case 2:
//                std::swap(bytes[0], bytes[1]);
//                break;
//            case 4:
//                std::swap(bytes[0], bytes[3]);
//                std::swap(bytes[1], bytes[2]);
//                break;
//            case 8:
//                std::swap(bytes[0], bytes[7]);
//                std::swap(bytes[1], bytes[6]);
//                std::swap(bytes[2], bytes[5]);
//                std::swap(bytes[3], bytes[4]);
//                break;
//            case 1:
//                break;
//            default:
//                std::terminate();
//        }
//        return *reinterpret_cast<const Result*>(bytes.data());
//    }
//    else if(checkEndiannes().compare("bigEndian")){
//        return *reinterpret_cast<const Result*>(bytes.data());
//    }
//    else{
//
//    }
//        switch(sizeof(Result)) {
//            case 1:
//                break;
//            case 2:
//                std::swap(bytes[0], bytes[1]);
//                break;
//            case 4:
//                std::swap(bytes[0], bytes[3]);
//                std::swap(bytes[1], bytes[2]);
//                break;
//            case 8:
//                std::swap(bytes[0], bytes[7]);
//                std::swap(bytes[1], bytes[6]);
//                std::swap(bytes[2], bytes[5]);
//                std::swap(bytes[3], bytes[4]);
//                break;
//            default:
//                std::terminate();
//        }
//    }
//    return *reinterpret_cast<const Result*>(bytes.data());
//} // read_integral
//
//// ======== helper functions ========
//    namespace {
//
//        constexpr static uint32_t endiadness{0xAABBCCDD};
//
//        inline bool is_little_endian() {
//            return ((unsigned char*)&endiadness)[0] == 0xDD;
//        }
//
//        template<typename Result>
//        inline Result read_integral(std::ifstream& in)
//        {
//            static_assert(std::is_integral<Result>::value, "only integral types");
//            static thread_local std::string bytes(sizeof(Result), 0);
//            in.read(&bytes[0], sizeof(Result));  // explictely allowed since C++11
//            if(!is_little_endian()) {
//                switch(sizeof(Result)) {
//                    case 1:
//                        break;
//                    case 2:
//                        std::swap(bytes[0], bytes[1]);
//                        break;
//                    case 4:
//                        std::swap(bytes[0], bytes[3]);
//                        std::swap(bytes[1], bytes[2]);
//                        break;
//                    case 8:
//                        std::swap(bytes[0], bytes[7]);
//                        std::swap(bytes[1], bytes[6]);
//                        std::swap(bytes[2], bytes[5]);
//                        std::swap(bytes[3], bytes[4]);
//                        break;
//                    default:
//                        std::terminate();
//                }
//            }
//            return *reinterpret_cast<const Result*>(bytes.data());
//        } // read_integral
//
//        template<std::size_t Width>
//        inline std::string read_string(std::ifstream& in)
//        {
//            std::string bytes(Width, 0);
//            in.read(&bytes[0], Width); // explictely allowed since C++11
//            return bytes;
//        } // read_string
//
//    } // anonymous namespace
//
//// Constructs a WavDecoder object, fills the WavHeader and seeks to
//// the first data chunk.
//    WavDecoder::WavDecoder(std::ifstream& in) : in_(in)
//    {
//        __BYTE_ORDER__
//        if(!in_) throw decoder_error("Couldn't open file!");
//        decode_wav_header();
//    }
//
//// Skips the next chunk of the wave file. Assumes that the ckID has
//// already been read and the next 4 bytes contain the chunk size.
//    void WavDecoder::skip_chunk()
//    {
//        auto chunk_size = read_integral<uint32_t>(in_);
//        in_.ignore(chunk_size);
//    }
//
//// Decodes the RIFF/WAVE header and fills the information into
//// header_.  File Specification taken from
//// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
//void WavDecoder::decode_wav_header()    {
//        if(read_string<4>(in_) != "RIFF") throw decoder_error("No RIFF file");
//        uint32_t in_size = read_integral<uint32_t>(in_);
//        if(read_string<4>(in_) != "WAVE") throw decoder_error("No WAVE type");
//
//        while(in_.good() && read_string<4>(in_) != "fmt ") skip_chunk();
//        if(!in_.good()) throw decoder_error("No format chunk");
//
//        auto fmt_chunk_size = read_integral<uint32_t>(in_);
//
//        if(read_integral<uint16_t>(in_) != 0x1) throw decoder_error("No PCM format");
//        header_.channels = read_integral<uint16_t>(in_);
//        header_.samplesPerSec = read_integral<uint32_t>(in_);
//        header_.avgBytesPerSec = read_integral<uint32_t>(in_);
//        header_.blockAlign = read_integral<uint16_t>(in_);
//        header_.bitsPerSample = read_integral<uint16_t>(in_);
//
//        // sample size is M-byte with M = block-align / Nchannels
//        header_.bytesPerSample = header_.blockAlign / header_.channels;
//
//        // skip the rest of the fmt header ...
//        in_.ignore(fmt_chunk_size - 16);
//    }
//
//// Moves the current read position of in_ to point to the size of a
//// data chunk. Assumes the file position is directly before the
//// beginning of a new chunk, i.e. the next 4 bytes are the ckID.
//    void WavDecoder::seek_data()
//    {
//        in_.peek(); // make sure eof is set properly if we are at the end of the file
//        while(in_.good() && read_string<4>(in_) != "data") skip_chunk();
//        if(!in_) throw decoder_error("Couldn't find data chunk!");
//    }
//
//// Read the next sample from the current data chunk. Seek the next
//// chunk if the current chunk is finished.
//    const WavDecoder::char_buffer& WavDecoder::read_samples(uint32_t nsamples)
//    {
//        if(!remaining_chunk_size) {
//            seek_data();
//            if(in_.eof()) { // file finished
//                buf_.resize(0);
//                return buf_;
//            }
//
//            // read chunk size
//            remaining_chunk_size = read_integral<uint32_t>(in_);
//            if(!remaining_chunk_size) throw decoder_error("Empty data chunk!");
//        }
//
//        // make sure there is enough space in the buffer to accomodate nsamples
//        buf_.resize(nsamples * header_.channels, 0);
//
//        // in case not enough samples are available to fulfill nsamples_
//        if(buf_.size() * header_.bytesPerSample > remaining_chunk_size) {
//            buf_.resize(remaining_chunk_size / header_.bytesPerSample, 0);
//        }
//
//        if(header_.bitsPerSample == 16) {
//            // directly stored as signed short ints, no conversion necessary (except endiadness)
//            in_.read(reinterpret_cast<char*>(&buf_[0]), 2*buf_.size());
//            if(!is_little_endian()) {
//                for(int16_t& s : buf_) {
//                    s = ((s & 0xFF) << 8) | ((s & 0xFF00) >> 8);
//                }
//            }
//        } else if(header_.bitsPerSample == 8) {
//            // stored as unsigned chars --> convert to signed short ints
//            auto input_it = std::ifstream_iterator<unsigned char>(in_);
//            std::generate_n(buf_.begin(), buf_.size(),
//                            [&input_it]() -> int16_t {
//                                return 257*(*input_it++) - 32768; // [0,255] to [-32768,32767]
//                            });
//        } else {
//            throw decoder_error("Resolution not supported.");
//        }
//
//        remaining_chunk_size -= in_.gcount();
//
//        return buf_;
//    }
//
//
//