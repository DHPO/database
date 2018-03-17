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
    TestData testBuf;
    generateTestData(testBuf, nrec);
    int start, end;

    Database *db;

    /* step 1. insert nrec records */
    {
        db = new Database(indexFile.c_str(), dataFile.c_str());
        start = clock();
        for(auto &pair : testBuf){
            db->store(pair.first.c_str(), pair.second.c_str());
        }
        end = clock();
        cout << "Step 1. insert nrec records: " << double(end - start) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        delete db;
    }

    /* step 2. fetch nrec records */
    {
        db = new Database(indexFile.c_str(), dataFile.c_str());
        start = clock();
        int count = 0;
        for(auto &pair : testBuf){
            char *val = db->fetch(pair.first.c_str());
            if (!val || strcmp(val, pair.second.c_str()) != 0)
                count ++;
        }
        end = clock();
        cout << "Step 2. fetch nrec records: " << double(end - start) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        delete db;
    }

    /* Step 3. composite */
    {
        db = new Database(indexFile.c_str(), dataFile.c_str());
        start = clock();
        for (int i = 0; i < 5 * nrec; i++){
            db->fetch(getRandomRecord(nrec).c_str());
            if (i % 37 == 0){
                db->remove(getRandomRecord(nrec).c_str());                
            }
            if (i % 11 == 0){
                string rec = getRandomRecord(nrec);
                db->store(rec.c_str(), rec.c_str());
                db->fetch(rec.c_str());
            }
            if (i % 34 == 17){
                string rec = getRandomRecord(nrec);
                db->modify(rec.c_str(), rec.substr(0, 1).c_str());
            }
            if (i % 34 == 0){
                string rec = getRandomRecord(nrec);
                db->modify(rec.c_str(), (rec + rec).c_str());
            }
        }
        end = clock();
        cout << "Step 3. composite: " << double(end - start) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        delete db;
    }

    /* Step 4. delete and fetch */
    {
        db = new Database(indexFile.c_str(), dataFile.c_str());
        start = clock();
        for(auto &pair : testBuf){
            db->remove(pair.first.c_str());
        }
        char key[10];
        db->rewind();
        while(db->nextrec(key)){
            db->remove(key);
        }
        end = clock();
        cout << "Step 4. delete and fetch: " << double(end - start) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
        delete db;
    }

    return 0;
}
