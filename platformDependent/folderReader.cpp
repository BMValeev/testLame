//
// Created by gyxma on 06.02.2021.
//

#include "folderReader.h"
#include <iostream>


folderReader::folderReader(std::string path)
    :folderPath(path){

}
std::vector<std::string> folderReader::readDirectory(){
    std::vector<std::string> names;
    std::string extensionWav=".wav";
#ifdef __WIN32__
    std::string search_path = folderPath + "/*.wav";
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
#elif __linux__
    if (auto dir = opendir(folderPath.c_str())) {
        while (auto f = readdir(dir)) {
            std::string filename = f->d_name;
            if (!filename.substr(filename.size() - 4, filename.size() - 1).compare(extensionWav)) {
                names.push_back(filename);
                std::cout << filename << std::endl;
            }
        }
        closedir(dir);
    }
    return names;
#else
#endif
    return names;
}
void folderReader::setDirectory(std::string directoryToRead){
    folderPath=directoryToRead;
}