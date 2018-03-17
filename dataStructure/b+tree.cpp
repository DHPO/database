#include "b+tree.h"
#include <iostream>
#include <cstring>
#define MAXLEN (10)
using namespace std;

TreeNode::TreeNode(bool isLeaf)
{
    init(isLeaf);
}

void TreeNode::init(bool isLeaf){
    this->isLeaf = isLeaf;
    amount = 0;
    prev = -1;
    next = -1;
    father = -1;
}

long TreeNode::find(const char *key){
    if (strncmp(keys[0], key, MAXLEN) > 0)
        return -1;

    int idx = 0;

    while(strncmp(key, keys[idx], MAXLEN) >= 0 && idx < amount)
        idx++;

    return children[idx - 1];
}

long TreeNode::findKey(const char *key){
    for (int i = 0; i < amount; i++)
        if (strncmp(keys[i], key, MAXLEN) == 0)
            return children[i];
    return -1;
}

bool TreeNode::replaceKey(const char *oldKey, const char *newKey){
    for (int i = 0; i < amount; i++)
        if (strncmp(keys[i], oldKey, MAXLEN) == 0){
            strncpy(keys[i], newKey, MAXLEN);
            return true;
        }
    return false;
}

bool TreeNode::insert(const char *key, long value){
    if (amount == 3)
        return false;
    if (amount == 0){
        strncpy(keys[0], key, MAXLEN);
        children[0] = value;
        amount = 1;
    }
    else if (amount == 1){
        if (strncmp(keys[0], key, MAXLEN) > 0){
            strncpy(keys[1], keys[0], MAXLEN);
            children[1] = children[0];
            strncpy(keys[0], key, MAXLEN);
            children[0] = value;
        }
        else{
            strncpy(keys[1], key, MAXLEN);
            children[1] = value;
        }
        amount = 2;
    }
    else{
        if (strncmp(keys[0], key, MAXLEN) > 0){
            strncpy(keys[2], keys[1], MAXLEN);
            children[2] = children[1];
            strncpy(keys[1], keys[0], MAXLEN);
            children[1] = children[0];
            strncpy(keys[0], key, MAXLEN);
            children[0] = value;
        }
        else if (strncmp(keys[1], key, MAXLEN) > 0){
            strncpy(keys[2], keys[1], MAXLEN);
            children[2] = children[1];
            strncpy(keys[1], key, MAXLEN);
            children[1] = value;
        }
        else{
            strncpy(keys[2], key, MAXLEN);
            children[2] = value;
        }
        amount = 3;
    }
    return true;
}

bool TreeNode::deleteKey(const char *key){
    if (key == NULL)
        return false;
    if (strncmp(keys[0], key, MAXLEN) == 0 && amount >= 1){
        strncpy(keys[0], keys[1], MAXLEN);
        children[0] = children[1];
        strncpy(keys[1], keys[2], MAXLEN);
        children[1] = children[2];
        amount--;
    }
    else if (strncmp(keys[1], key, MAXLEN) == 0 && amount >= 2){
        strncpy(keys[1], keys[2], MAXLEN);
        children[1] = children[2];
        amount--;
    }
    else if (strncmp(keys[2], key, MAXLEN) == 0 && amount ==3){
        amount = 2;
    }
    else
        return false;
    return true;
}

char *TreeNode::getKey(int idx){
    if (idx >=0 && idx < amount)
        return keys[idx];
    else
        return NULL;
}

long TreeNode::getVal(int idx){
    if(idx >= 0 && idx < amount)
        return children[idx];
    else
        return -1;
}

bool TreeNode::check(){
    if (amount == 0 || amount == 1)
        return true;
    else if (amount == 2)
        return strncmp(getKey(0), getKey(1), MAXLEN) < 0;
    else if (amount == 3)
        return strncmp(getKey(0), getKey(1), MAXLEN) < 0 && strncmp(getKey(1), getKey(2), MAXLEN) < 0;
    else
        return false;
}

void TreeNode::print(){
    for (int i = 0; i < amount; i++){
        cout << getKey(i) << " "; 
    }
}

BPlusTree::BPlusTree(BlockManage *manager, TreeNode *top){
    this->manager = manager;
    this->top = top;
    height = 0;
    rewind();
}

long BPlusTree::get(const char *key){
    TreeNode *ptr = top;
    long pos = 0;
    while(!ptr->isLeaf){
        pos = ptr->find(key);
        if (pos == -1)
            return -1;
        ptr = (TreeNode *)manager->getAddress(pos);
    }
    return ptr->findKey(key);
}

void BPlusTree::set(const char *key, long value){
    TreeNode *leaf = findFitLeaf(key);
    char oldKey[MAXLEN];
    if (leaf->getKey(0) != NULL)
        strncpy(oldKey, leaf->getKey(0), MAXLEN);
    if (leaf->findKey(key) != -1){
        leaf->deleteKey(key);
        leaf->insert(key, value);
        return;
    }
    while (leaf->amount == 3){
        balance(leaf);
        strncpy(oldKey, leaf->getKey(0), MAXLEN);

        if (!leaf->isLeaf)
            leaf = getTreeNodeByPos(leaf->find(key));
        TreeNode *next = getTreeNodeByPos(leaf->next);
        if (next && strncmp(next->getKey(0), key, MAXLEN) < 0)
            leaf = next;
    }
    leaf->insert(key, value);
    TreeNode *prev = getTreeNodeByPos(leaf->prev);
    TreeNode *next = getTreeNodeByPos(leaf->next);
    if (strncmp(leaf->getKey(0), key, MAXLEN) == 0){
        TreeNode *node = leaf;
        TreeNode *father = getTreeNodeByPos(node->father);
        /*if (father->findKey(prev->getKey(prev->amount - 1)) != -1)
            oldKey = prev->getKey(prev->amount - 1);*/
        while(father){
            if (!father->replaceKey(oldKey, key)/* && !father->replaceKey(father->getKey(1), key)*/)
                throw "error1";
            if (!father || strncmp(father->getKey(0), key, MAXLEN) != 0)
                break;
            father = getTreeNodeByPos(father->father);
        }
    }
}

TreeNode *BPlusTree::getNewTreeNode(bool isLeaf){
    TreeNode *result;
    try{
        result = (TreeNode *)manager->getAddress(manager->alloc(sizeof(TreeNode)));
        result->init(isLeaf);
    }catch(...){
        top = (TreeNode *)manager->getAddress(12);
        throw "expand";
    }
    return result;
}

TreeNode *BPlusTree::getTreeNodeByPos(long position){
    if (position == -1)
        return 0;
    return (TreeNode *)manager->getAddress(position);
}

bool BPlusTree::moveToPrev(TreeNode *node){
    TreeNode *prev = getTreeNodeByPos(node->prev);
    if (!prev || prev->amount == 3 || (!prev->isLeaf && prev->amount == 2))
        return false;
    else{
        prev->insert(node->getKey(0), node->getVal(0));
        if (!node->isLeaf)
            getTreeNodeByPos(node->getVal(0))->father = node->prev;
        TreeNode *father = getTreeNodeByPos(node->father);
        while(father){
            if (!father->replaceKey(node->getKey(0), node->getKey(1)))
                throw "error1";
            if (!father || strncmp(father->getKey(0), node->getKey(1), MAXLEN) != 0)
                break;
            father = getTreeNodeByPos(father->father);
        }
        node->deleteKey(node->getKey(0));
        return true;
    }
}

bool BPlusTree::moveToNext(TreeNode *node){
    TreeNode *next = getTreeNodeByPos(node->next);
    if (!next || next->amount == 3 || (!next->isLeaf && next->amount == 2))
        return false;
    else{
        int maxIdx = node->amount - 1;
        next->insert(node->getKey(maxIdx), node->getVal(maxIdx));
        if (!node->isLeaf)
            getTreeNodeByPos(node->getVal(maxIdx))->father = node->next;
        TreeNode *father = getTreeNodeByPos(next->father);
        //father->replaceKey(next->getKey(1), next->getKey(0));
        while(father){
            if (!father->replaceKey(next->getKey(1), next->getKey(0)))
                throw "error1";
            if (!father || strncmp(father->getKey(0), next->getKey(0), MAXLEN) != 0)
                break;
            father = getTreeNodeByPos(father->father);
        }
        node->deleteKey(node->getKey(maxIdx));
        return true;
    }
}

void BPlusTree::splitTop(){
    TreeNode *newNode1 = getNewTreeNode(top->isLeaf);
    TreeNode *newNode2 = getNewTreeNode(top->isLeaf);

    newNode1->insert(top->getKey(0), top->getVal(0));
    newNode1->insert(top->getKey(1), top->getVal(1));
    newNode2->insert(top->getKey(2), top->getVal(2));

    newNode1->prev = -1; newNode1->next = manager->getPosition(newNode2);
    newNode2->prev = manager->getPosition(newNode1); newNode2->next = -1;

    newNode1->father = manager->getPosition(top); 
    newNode2->father = manager->getPosition(top); 
    if (!top->isLeaf){
        getTreeNodeByPos(top->getVal(0))->father = manager->getPosition(newNode1);
        getTreeNodeByPos(top->getVal(1))->father = manager->getPosition(newNode1);
        getTreeNodeByPos(top->getVal(2))->father = manager->getPosition(newNode2);
    }

    top->deleteKey(top->getKey(0));
    top->deleteKey(top->getKey(0));
    top->deleteKey(top->getKey(0));
    top->insert(newNode1->getKey(0), manager->getPosition(newNode1));
    top->insert(newNode2->getKey(0), manager->getPosition(newNode2));
    top->isLeaf = false;

    height++;
}

TreeNode *BPlusTree::findFitLeaf(const char *key){
    TreeNode *ptr = top;
    while(!ptr->isLeaf){
        ptr = getTreeNodeByPos(ptr->find(key));
        //TreeNode *prev = getTreeNodeByPos(ptr->prev); // debug
        //TreeNode *next = getTreeNodeByPos(ptr->next);
        if (!ptr)
            break;
    }
    return ptr;
}

void BPlusTree::balance(TreeNode *node){
    if (node == top && top->amount == 3){
        splitTop();
    }
    else if (node->amount == 3){
        if (!moveToPrev(node) && !moveToNext(node)){
            balance(getTreeNodeByPos(node->father));
            if (node->amount == 3 && !moveToPrev(node) && !moveToNext(node))
                balance(node);
        }
    }
    else if (node->amount == 2){
        TreeNode *firstSon = getTreeNodeByPos(node->getVal(0));
        TreeNode *secondSon = getTreeNodeByPos(node->getVal(1));
        TreeNode *newNode = getNewTreeNode(firstSon->isLeaf);

        newNode->insert(secondSon->getKey(1), secondSon->getVal(1));
        if (secondSon->amount == 3)
            newNode->insert(secondSon->getKey(2), secondSon->getVal(2));
        newNode->father = manager->getPosition(node);
        newNode->prev = manager->getPosition(secondSon);
        newNode->next = secondSon->next;

        if (!firstSon->isLeaf){
            if (getTreeNodeByPos(firstSon->getVal(2)))
                getTreeNodeByPos(firstSon->getVal(2))->father = node->getVal(1);
            getTreeNodeByPos(secondSon->getVal(1))->father = manager->getPosition(newNode);
            if (getTreeNodeByPos(secondSon->getVal(2)))
                getTreeNodeByPos(secondSon->getVal(2))->father = manager->getPosition(newNode);
        }

        secondSon->deleteKey(secondSon->getKey(1));
        secondSon->deleteKey(secondSon->getKey(1));
        if (firstSon->amount == 3)
            secondSon->insert(firstSon->getKey(2), firstSon->getVal(2));
        if (secondSon->next != -1)
            getTreeNodeByPos(secondSon->next)->prev = manager->getPosition(newNode);
        secondSon->next = manager->getPosition(newNode);

        firstSon->deleteKey(firstSon->getKey(2));

        node->deleteKey(node->getKey(1));
        node->insert(secondSon->getKey(0), manager->getPosition(secondSon));
        node->insert(newNode->getKey(0), manager->getPosition(newNode));
    }
    else{
        TreeNode *son = getTreeNodeByPos(node->getVal(0));
        TreeNode *newNode = getNewTreeNode(son->isLeaf);

        newNode->insert(son->getKey(2), son->getVal(2));
        newNode->next = son->next;
        newNode->prev = manager->getPosition(son);
        newNode->father = manager->getPosition(node);

        if (!son->isLeaf){
            getTreeNodeByPos(son->getVal(2))->father = manager->getPosition(newNode);
        }

        son->deleteKey(son->getKey(2));
        if (getTreeNodeByPos(son->next))
            getTreeNodeByPos(son->next)->prev = manager->getPosition(newNode);
        son->next = manager->getPosition(newNode);

        node->insert(newNode->getKey(0), manager->getPosition(newNode));
    }
}

TreeNode *BPlusTree::getFirstLeaf(){
    TreeNode *ptr = top;
    while(!ptr->isLeaf){
        ptr = getTreeNodeByPos(ptr->getVal(0));
    }
    return ptr;
}

bool BPlusTree::remove(const char *key){
    TreeNode *leaf = findFitLeaf(key);
    if (strncmp(leaf->getKey(0), key, MAXLEN) == 0){
        TreeNode *ptr = leaf;
        while(ptr){
            TreeNode *father = getTreeNodeByPos(ptr->father);
            ptr->deleteKey(key);
            if (ptr->amount > 0){
                while(father){
                    if (!father->replaceKey(key, ptr->getKey(0)))
                        break;//throw "error1";
                    if (!father || strncmp(father->getKey(0), ptr->getKey(0), MAXLEN) != 0)
                        break;
                    father = getTreeNodeByPos(father->father);
                }
                break;
            }
            else{
                if (getTreeNodeByPos(ptr->prev))
                    getTreeNodeByPos(ptr->prev)->next = ptr->next;
                if (getTreeNodeByPos(ptr->next))
                    getTreeNodeByPos(ptr->next)->prev = ptr->prev;
                manager->free(manager->getPosition(ptr));
                ptr = father;
            }
        }
    }
    return leaf->deleteKey(key);
}

bool BPlusTree::check(){
    TreeNode *ptr = getFirstLeaf();
    char max[MAXLEN] = {0};
    while(ptr){
        ptr->print();
        cout << endl;
        if (ptr->check() == false || strncmp(ptr->getKey(0), max, MAXLEN) <= 0)
            return false;
        else{
            strncpy(max, ptr->getKey(ptr->amount - 1), MAXLEN);
            TreeNode *oldptr = ptr;
            ptr = getTreeNodeByPos(ptr->next);
            if (ptr && getTreeNodeByPos(ptr->prev) != oldptr)
                return false;
        }
    }
    cout << endl;
    return true;
}

void BPlusTree::rewind(){
    cursor = manager->getPosition(getFirstLeaf());
    cursorIdx = 0;
}

long BPlusTree::nextrec(char *key){
    TreeNode *leaf = (TreeNode *)manager->getAddress(cursor);
    if (!leaf){
        return -1;
    }
    else{
        long result = leaf->getVal(cursorIdx);
        while (cursorIdx >= leaf->amount){
            cursor = leaf->next;
            cursorIdx = 0;
            leaf = (TreeNode *)manager->getAddress(cursor);
        }
        strcpy(key, leaf->getKey(cursorIdx));
        if (++cursorIdx >= leaf->amount){
            cursor = leaf->next;
            cursorIdx = 0;
        }
        return result;
    }
}
