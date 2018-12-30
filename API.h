
#ifndef API_h
#define API_h

#include "basic.h"
#include "catalogManager.h"
#include "RecordManager.h"

extern bufferManager* buffer_manager;

class API {
public:
    Table selectRecord(string table_name, vector<int> target_attr, vector<Where> where);//
    int deleteRecord(string table_name, vector<int> target_attr , vector<Where> where);//
    void deleteRecordAll(string table_name);//
    void insertRecord(string table_name, Tuple& tuple);//
    void createTable(string table_name, Attribute attribute, int primary, Index index);//
    void dropTable(string table_name);//
    void createIndex(string table_name, string index_name, string attr_name);//
    void dropIndex(string table_name, string index_name);//
};

#endif /* API_h */
