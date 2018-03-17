#include "database.h"

int main(){
    Database *db;
    bool success = false;
    do{
        string index, data;
        cout << "Please input file name of index: ";
        cin >> index;
        cout << "Please input file name of data: ";
        cin >> data;
        try{
            db = new Database(index.c_str(), data.c_str());
            success = true;
        }
        catch(...){

        }
    }while(!success);

    while(true){
        string cmd;
        cout << ">> ";
        cin >> cmd;
        if (cmd == "exit")
            break;
        else if (cmd == "store"){
            string key, value;
            cin >> key >> value;
            int start = clock();
            if (key.size() > 9){
                cout << "Key is too long(no longer than 9)!" << endl;
            }
            else{
                if (db->store(key.c_str(), value.c_str())){
                    cout << "OK" << endl;
                }
                else{
                    cout << "Duplicate key: " << key << ": " << db->fetch(key.c_str()) << endl;
                }
            }
            int end = clock();
            cout << double(end - start)/CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
        else if (cmd == "fetch"){
            string key;
            cin >> key;
            int start = clock();            
            if (key.size() > 9){
                cout << "Key is too long(no longer than 9)!" << endl;
            }
            else{
                char *val = db->fetch(key.c_str());
                if (!val){
                    cout << "Record Not Found" << endl;
                }
                else{
                    cout << val << endl;
                }
            }
            int end = clock();
            cout << double(end - start)/CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
        else if (cmd == "rewind"){
            db->rewind();
        }
        else if (cmd == "nextrec"){
            int start = clock();            
            char key[10], *val;
            val = db->nextrec(key);
            if (!val){
                cout << "No more records" << endl;
            }
            else{
                cout << key << ": " << val << endl;
            }
            int end = clock();
            cout << double(end - start)/CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
        else if (cmd == "modify"){
            string key, val;
            cin >> key >> val;
            int start = clock();
            if (key.size() > 9){
                cout << "Key is too long(no longer than 9)!" << endl;
            }
            else{             
                if (!db->modify(key.c_str(), val.c_str())){
                    cout << "Record Not Found" << endl;
                }
                else{
                    cout << "OK" << endl;
                }
            }
            int end = clock();
            cout << double(end - start)/CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
        else if (cmd == "remove"){
            string key;
            cin >> key;
            int start = clock();
            if (key.size() > 9){
                cout << "Key is too long(no longer than 9)!" << endl;
                continue;
            }
            else{
                if (!db->remove(key.c_str())){
                    cout << "Record Not Found" << endl;
                }
                else{
                    cout << "OK" << endl;
                }
            }
            int end = clock();
            cout << double(end - start)/CLOCKS_PER_SEC * 1000 << "ms" << endl;
        }
        else{
            cout << "commands:\n"
                 << "exit - exit database\n"
                 << "store [key] [value] - store a new record\n"
                 << "fetch [key] - fetch an existing record\n"
                 << "modify [key] [value] - modify an existing record\n"
                 << "remove [key] - remove an existing record\n"
                 << "rewind - reset cursor\n"
                 << "nextrec - get the record pointed by cursor"
                 << endl;
        }
    }

    delete db;
    return 0;
}