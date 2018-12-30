
#ifndef RecordManager_hpp
#define RecordManager_hpp

#include <stdio.h>
#include "RecordManager.h"
#include "storeRecord.h"
#include "bufferManager.h"

extern bufferManager* buffer_manager;

class RecordManager{
public:
    //创建表
    void CreateTable(Table &tableIn);
    //删除表
    void DropTable(Table &tableIn);
    //插入元组
    void Insert(Table &tableIn, Tuple &singleTuper);
    //删除元组
    int Delete(Table &tableIn, vector<int> mask, vector<Where> w);
    //选择&投影
    Table Select(Table &tableIn, vector<int> attrSelect, vector<int> mask, vector<Where> w);
    void CreateIndex( Table &tableIn , const char* indexName , const char* attrName );
private:
    bool isunique(Table& tableIn, Where w, int loca);
    Table SelectProject(Table &tableIn, vector<int> attrSelect);
};


#endif /* RecordManager_hpp */
