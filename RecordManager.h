
#ifndef RecordManager_hpp
#define RecordManager_hpp

#include <stdio.h>
#include "RecordManager.h"
#include "storeRecord.h"
#include "bufferManager.h"

extern bufferManager* buffer_manager;

class RecordManager{
public:
    //������
    void CreateTable(Table &tableIn);
    //ɾ����
    void DropTable(Table &tableIn);
    //����Ԫ��
    void Insert(Table &tableIn, Tuple &singleTuper);
    //ɾ��Ԫ��
    int Delete(Table &tableIn, vector<int> mask, vector<Where> w);
    //ѡ��&ͶӰ
    Table Select(Table &tableIn, vector<int> attrSelect, vector<int> mask, vector<Where> w);
    void CreateIndex( Table &tableIn , const char* indexName , const char* attrName );
private:
    bool isunique(Table& tableIn, Where w, int loca);
    Table SelectProject(Table &tableIn, vector<int> attrSelect);
};


#endif /* RecordManager_hpp */
