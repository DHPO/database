#include "database.h"
#include <cstring>

Database::Database(const char *index, const char *data){
    bool hasIndex = FileManage::exists(index);
    bool hasData = FileManage::exists(data);

    if (hasIndex ^ hasData){
        throw "file error";
    }

    indexFile = new FileManage(index);
    dataFile = new FileManage(data);
    indexBlock = new BlockManage(indexFile);
    dataBlock = new BlockManage(dataFile);
    if (!hasIndex){
        indexBlock->init();
        dataBlock->init();
        TreeNode *ptr = (TreeNode *)indexBlock->getAddress(indexBlock->alloc(sizeof(TreeNode)));
            ptr->init(true);
        tree = new BPlusTree(indexBlock, ptr);
        tree->set("\0", -1);
    }
    else{
        tree = new BPlusTree(indexBlock, (TreeNode *)indexBlock->getAddress(12));
    }
    
    rewind();
}

Database::~Database(){
    delete indexFile;
    delete dataFile;
    delete indexBlock;
    delete dataBlock;
    delete tree;
}

bool Database::store(const char *key, const char *value){
    if (tree->get(key) != -1)
        return false;
    else{
        long position;
        try{
            position = dataBlock->alloc(strlen(value) + 1);
        }
        catch(...){
            position = dataBlock->alloc(strlen(value) + 1);
        }
        strcpy((char *)dataBlock->getAddress(position), value);
        if (position == -1)
            throw "error"; 
        bool flag = false;
        do{
            try{
                tree->set(key, position);
                flag = true;
            }
            catch(...){}
        }while(!flag);
        return true;
    }
}

char *Database::fetch(const char *key){
    long position = tree->get(key);
    if (position == -1)
        return NULL;
    else{
        return (char *)dataBlock->getAddress(position);
    }
}

bool Database::modify(const char *key, const char *value){
    long position = tree->get(key);
    if (position == -1)
        return false;
    else{
        char *oldVal = (char *)dataBlock->getAddress(position);
        if (strlen(oldVal) >= strlen(value)){ // reuse old block
            strcpy(oldVal, value);
        }
        else{ // use new block
            long position;
            try{
                position = dataBlock->alloc(strlen(value) + 1);
            }
            catch(...){
                position = dataBlock->alloc(strlen(value) + 1);
            }
            strcpy((char *)dataBlock->getAddress(position), value);
            tree->set(key, position);
        }
        return true; 
    }
}

bool Database::remove(const char *key){
    if (tree->get(key) == -1)
        return false;
    else{
        tree->remove(key);
        return true;
    }
}

void Database::rewind(){
    tree->rewind();
    char buf[10];
    tree->nextrec(buf);
}

char *Database::nextrec(char *key){
    long position = tree->nextrec(key);
    if (position == -1)
        return NULL;
    else{
        return (char *)dataBlock->getAddress(position);
    }
}