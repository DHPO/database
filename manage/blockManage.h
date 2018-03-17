#ifndef BLOCKMANAGE
#define BLOCKMANAGE

#include "file.h"

class BlockManage{
public:
    BlockManage(void *address = 0);
    BlockManage(FileManage *filemanage);
    long alloc(long size);
    bool free(long position);
    void *getAddress(long position);
    long getPosition(void *address);
    void setBaseAddress(void *address = 0);
    bool init();
    bool getMode();
private:
    void *baseAddress;
    long fileUsedSize;
    long fileSize;
    bool fileMode;
    FileManage *filemanage;
};

#endif