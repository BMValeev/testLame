#include <iostream>
#include <map>
#include <vector>
#include "coder/CoderQueue.h"
#include "platformDependent/folderReader.h"


int main(int argc, char *argv[]) {
    const auto processor_count = std::thread::hardware_concurrency();
    std::cout << "Number of processors :"<<processor_count << std::endl;
    std::vector<std::string> files;
    std::cout<<"Get next arguments:"<<std::endl;
    for (int count=0; count < argc; ++count){
        std::cout<< count << " " << argv[count]<<std::endl;
    }
    std::string newPath;
    if(argc<2){
        std::cout<<"Path is not set, please set as first arg to program:" <<std::endl;
        std::cout<<"e.g. testLame /usr/local/wav" <<std::endl;
    }
    else{
        std::string newPath(argv[1]);
    }
    if(!newPath.size()){
        newPath="C://Qt//";
    }
    std::cout<<"Path set: to" <<newPath<<std::endl;
    folderReader dirs(newPath);

    auto file=dirs.readDirectory();
    for(auto i: file){
        files.emplace_back(i);
        std::cout<<i<<std::endl;
    }
    for(auto item=0;item<10;item++){
        files.emplace_back(std::to_string(item)+".wav");
    }
    if(!files.size()){
        std::cout<<"Now wav files found in the folder" <<std::endl;
        return 1;
    }
    CoderQueue queue(files, processor_count);
    while(queue.isProcessing()){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout<<__PRETTY_FUNCTION__ <<std::endl;
    return 0;
}
