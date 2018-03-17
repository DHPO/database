#include "blockManage.h"
#include <cstdlib>

BlockManage::BlockManage(void *address){
    baseAddress = address;
    fileMode = false;
}

BlockManage::BlockManage(FileManage *filemanage){
    baseAddress = filemanage->getPtr();
    fileSize = filemanage->getFileSize();
    fileUsedSize = *(int *)getAddress(0);
    fileMode = true;
    this->filemanage = filemanage;
}

long BlockManage::alloc(long size){
    if (!fileMode)
        return getPosition(std::malloc(size));
    else{
        if (*(long *)getAddress(4) != -1){
            long result = *(long *)getAddress(4);
            *(long *)getAddress(4) = *(long *)getAddress(result);
            return result;
        }
        if (fileUsedSize + size >= fileSize){
            filemanage->expand(fileSize/PAGE + 1); // TO OPTIMIZE
            baseAddress = filemanage->getPtr();
            fileSize = filemanage->getFileSize();
            throw "expand";
        }
        long result = fileUsedSize;
        fileUsedSize += size;
        *(int *)getAddress(0) = fileUsedSize;
        return result;
    }
}

bool BlockManage::getMode(){
    return fileMode;
}

bool BlockManage::free(long position){
    if (!fileMode)
        std::free((void *)position);
    else{
        *(long *)getAddress(position) = *(long *)getAddress(4);
        *(long *)getAddress(4) = position;
    }
    return true;
}

bool BlockManage::init(){
    *(int *)getAddress(0) = 12;
    *(long *)getAddress(4) = -1;
    fileUsedSize = 12;
    return true;
}

void * BlockManage::getAddress(long position){
    if (position == -1)
        return 0;
    return (char *)baseAddress + (unsigned long)position;
}

long BlockManage::getPosition(void *address){
    return long((char *)address - (char *)baseAddress);
}

void BlockManage::setBaseAddress(void *address){
    baseAddress = address;
}

