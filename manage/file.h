#ifndef FILEMANAGE
#define FILEMANAGE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE (1<<12)

class FileManage{
public:
    /* init by filename */
    FileManage(const char *filename);
    /* clear mmap */
    ~FileManage();
    /* get size of file */
    int getFileSize();
    /* get mapped memory address */
    void *getPtr();
    /* expand size of file, unit: page */
    bool expand(int pages);
    /* test if a file exists */
    static bool exists(const char *filename);
private:
    int size;
    char *filename;
    void *ptr;
};

#endif