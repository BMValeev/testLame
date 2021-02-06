//
// Created by gyxma on 06.02.2021.
//

#ifndef TESTLAME_FOLDERREADER_H
#define TESTLAME_FOLDERREADER_H
#ifdef __linux__
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#elif __WIN32__
#include <Windows.h>
#else
#endif

#include <vector>
#include <string>
class folderReader {
public:
    folderReader(std::string path);
    ~folderReader(){}
    std::vector<std::string> readDirectory();
    void setDirectory(std::string directoryToRead);
private:
    std::string folderPath;
};


#endif //TESTLAME_FOLDERREADER_H
