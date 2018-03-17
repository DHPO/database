#ifndef BTREE
#define BTREE

#include "../manage/blockManage.h"

class TreeNode{
public:
    /* init */
    TreeNode(bool isLeaf);
    /* init */
    void init(bool isLeaf);
    /* returns child's offset, or value */
    long find(const char *key);
    /* find whether the key is in this node */
    long findKey(const char *key);
    /* insert a key */
    bool insert(const char *key, long value);
    /* delete a key */
    bool deleteKey(const char *key);

    bool isLeaf;
    long father;
    int amount; // amount of keys
    long prev;
    long next;
    char *getKey(int idx);
    long getVal(int idx);
    bool check();
    void print();
    bool replaceKey(const char *oldKey, const char *newKey);
private:
    char keys[3][10];
    long children[3];
};

class BPlusTree{
public:
    /* init with BlockManager and top node (read from file) */
    BPlusTree(BlockManage *manager, TreeNode *top);
    /* get a record, returns -1 if not found */
    long get(const char *key);
    /* store or modify a key-value pair */
    void set(const char *key, long value);
    /* remove a record */
    bool remove(const char *key);
    /* check if BPlusTree observes rules */
    bool check();
    /* reset cursor */
    void rewind();
    /* get record pointed by cursor, and move cursor to the next */
    long nextrec(char *key);
private:
    TreeNode *top;
    BlockManage *manager;
    TreeNode *getNewTreeNode(bool isLeaf);
    TreeNode *getTreeNodeByPos(long position);
    bool moveToPrev(TreeNode *node);
    bool moveToNext(TreeNode *node);
    void splitTop();
    TreeNode *findFitLeaf(const char *key);
    TreeNode *getFirstLeaf();
    void balance(TreeNode *node);
    int height;
    long cursor;
    int cursorIdx;
};

#endif