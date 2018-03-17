#ifndef DATABASE
#define DATABASE
#include "../dataStructure/b+tree.h"
#include "../manage/blockManage.h"
#include "../manage/file.h"
#include <iostream>
using namespace std;

class Database{
public:
    /* open or create a database (create if two files not exist) */
    Database(const char *index, const char *data);
    /* close database */
    ~Database();
    /* store a record, the length of key must be shorter than 9 */
    bool store(const char *key, const char *value);
    /* fetch a record, returns NULL if not found */
    char *fetch(const char *key);
    /* reset cursor */
    void rewind();
    /* fetch the next record after the cursor, returns NULL if reaches the end */
    char *nextrec(char *key);
    /* modify an existing record, returns false if the record not exists */
    bool modify(const char *key, const char *value);
    /* remove an existing record, returns false if the record not exists */
    bool remove(const char *key);

private:
    FileManage *indexFile, *dataFile;
    BlockManage *indexBlock, *dataBlock;
    BPlusTree *tree;
    
};

#endif