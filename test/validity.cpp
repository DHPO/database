#include "../main/database.h"
#include <vector>
#include <cstring>

typedef vector<pair<string, string>> TestData;
const int nrec = 100000;
const string indexFile = "index";
const string dataFile = "data";

string getRandomRecord(int nrec){
    return to_string(rand()%(nrec*2));
}

void generateTestData(TestData& test, int nrec) {
    while(test.size() < nrec){
        string num = getRandomRecord(nrec);
        test.push_back(pair<string, string>(num, num));
    }
}

int main(){
    {
        /* Generate test data */
        TestData testBuf;
        generateTestData(testBuf, nrec);

        /* Test store and fetch */
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                db.store(pair.first.c_str(), pair.second.c_str());
            }
        }
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                char *val = db.fetch(pair.first.c_str());
                if (!val || strcmp(val, pair.second.c_str()) != 0)
                    throw "Error";
            }
        }
        /* Test modify an existing record */
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                db.modify(pair.first.c_str(), (pair.second + pair.second).c_str());
            }
        }
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                char *val = db.fetch(pair.first.c_str());
                if (!val || strcmp(val, (pair.second + pair.second).c_str()) != 0)
                    throw "Error";
            }
        }

        /* Delete an existing record, then modify, fetch, delete again */
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                db.remove(pair.first.c_str());
            }
        }
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                if (db.fetch(pair.first.c_str()))
                    throw "Error";
                if (db.modify(pair.first.c_str(), "abc"))
                    throw "Error";
                if (db.remove(pair.first.c_str()))
                    throw "Error";
            }
        }

        /* nextrec and delete */
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            for(auto &pair : testBuf){
                db.store(pair.first.c_str(), pair.second.c_str());
            }
        }
        {
            Database db(indexFile.c_str(), dataFile.c_str());
            char key[10];
            db.rewind();
            while(db.nextrec(key)){
                db.remove(key);
            }
        }
    }
    int fd = open("data", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    cout << fd << endl;
    if (fd != -1)
        close(fd);

    return 0;
}