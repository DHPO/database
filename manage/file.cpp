#include "file.h"
#include <cstring>

FileManage::FileManage(const char *filename){
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw "Can't open file";

    this->filename = new char[strlen(filename)+1];
    strcpy(this->filename, filename);
    /* get size */
    struct stat buf;
    fstat(fd, &buf);
    size = buf.st_size;
    if (size < PAGE)
        expand(1);

    /* mmap */
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);
}

FileManage::~FileManage(){
    munmap(ptr, size);
}

int FileManage::getFileSize(){
    return size;
}

void *FileManage::getPtr(){
    return ptr;
}

bool FileManage::expand(int pages){
    int totalSize = pages * PAGE + size;
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return false;

    lseek(fd, totalSize - 1, SEEK_SET);
    write(fd, "a", 1);
    munmap(ptr, size);
    size = totalSize;
    ptr = mmap(ptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return true;
}

bool FileManage::exists(const char *filename){
    int fd = open(filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return false;
    else{
        close(fd);
        return true;
    }
}