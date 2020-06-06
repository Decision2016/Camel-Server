//
// Created by Decision on 2020/5/11.
//

#include "FileManager.h"

FileManager::FileManager(std::string _path, Logger *_logger):logger(_logger) {
    path = _path;
}

bool FileManager::getDirInfo(std::string &infoString) {
    infoString.clear();
    DIR *dir;
    struct dirent *ptr;
    std::string dirInfo, fileInfo;
    int count = 0;

    if ((dir=opendir(path.c_str())) == nullptr)
    {
        logger -> error("An error occurred while open path %s.", path.c_str());
        return false;
    }

    dirInfo.clear();
    fileInfo.clear();

    while ((ptr = readdir(dir)) != nullptr) {
        if(count != 2) {
            count++;
            continue;
        }
        std::string name(ptr->d_name);
        if(ptr -> d_type == 4) {
            dirInfo += ("0/folder/" + name + ";");
        }
        else {
            std::string extName;
            getExtName(extName, name);
            fileInfo.append("1/" + extName + "/" + name + ";");
        }
    }
    infoString = dirInfo + fileInfo;
}

void FileManager::getExtName(std::string &extName,const std::string &fileName) {
    int pos = fileName.find_last_of('.');
    if (pos == std::string::npos) extName = "other";
    else extName = fileName.substr(pos + 1);
}

bool FileManager::checkDirExist() {
    DIR *dir;
    dir=opendir(path.c_str());
    if (dir == nullptr) return false;
    return true;
}

bool FileManager::createDirectory() {
    if (checkDirExist()) return false;
    int isCreated = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
    if (! isCreated) {
        logger -> success("User create directory success.");
        return true;
    }
    else {
        logger -> error("Some error occurred while user create directory.");
        return false;
    }
}