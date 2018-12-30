
#include "RecordManager.h"
#include "storeRecord.h"
#include "bufferManager.h"
#include "definitions.h"
#include "exception.h"
#include "indexManager.h"
#include "catalogManager.h"

void RecordManager::CreateTable( Table &tableIn ){
    string filename = "record_" + tableIn.getTitle();
    buffer_manager->createNewFile(filename, 1, RECORD_LENTH);
    
    //分配一个新的地址用于储存表单信息
}

void RecordManager::DropTable(Table &tableIn){
    string filename = "record_" + tableIn.getTitle();
    delete buffer_manager;
	buffer_manager=new bufferManager();
    remove(filename.c_str());
    
}

void RecordManager::Insert(Table &tableIn, Tuple &singleTuper){
    //检查插入的元组的各个属性是否合法
    for ( int i = 0 ; i < singleTuper.getData().size() ; i ++ ) {
        if ( singleTuper.getData()[i].type != tableIn.getAttr().type[i] )
            throw tuple_type_conflict();
    }
    //检查是否存在主键冲突
    if( tableIn.getAttr().primary_key >= 0 ){//存在主键
        Where w;
        w.data = singleTuper.getData()[tableIn.getAttr().primary_key];
        w.relation_character = EQUAL;
        if( isunique(tableIn, w, tableIn.getAttr().primary_key) == false )
            throw primary_key_conflict();
    }
    //检查是否存在重复冲突
    for( int i = 0 ; i < tableIn.getAttr().num ; i ++ ){
        if( tableIn.getAttr().unique[i] == true ){
            Where w;
            w.data = singleTuper.getData()[i];
            w.relation_character = EQUAL;
            if( isunique(tableIn, w, i) == false )
                throw unique_conflict();
        }
    }
    //异常检查完毕
    
    char *charTuper = Tuper2Char(tableIn,singleTuper);
    string filename = "record_" + tableIn.getTitle();
    int block_num = 0;
    
    blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
    char * content = bptr->getBlockContent();
    while( BLOCK_SIZE-BLOCK_HEADER-strlen(content) - 2000 <= strlen(charTuper) ){
        block_num++;
        bptr = buffer_manager->getBlockMem(filename, block_num);
        content = bptr->getBlockContent();
    }
    memcpy(content+strlen(content), charTuper, strlen(charTuper));
    bptr->setDirty(true);
    indexManager *in_manager = new indexManager;
    for( int i = 0 ; i < tableIn.getAttr().num ; i ++ ){
        if( tableIn.getAttr().has_index[i] == true ){
            int num;
            for( num = 0 ; num < tableIn.getIndex().num ; num ++ ){
                if( tableIn.getIndex().location[num] == i )
                    break;
            }
            in_manager->Insert( tableIn.getIndex().indexname[num].c_str(), singleTuper.getData()[i], block_num);
        }
    }
};

void RecordManager::CreateIndex( Table &tableIn , const char *indexName , const char *attrName ){
    int position;
    for( position = 0 ; position < tableIn.getAttr().num ; position ++ ){
        if( attrName == tableIn.getAttr().name[position] )//找到属性位置
            break;
    }
    int block_num = 0;
    string filename = "record_" + tableIn.getTitle();
    blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
    char * content = bptr->getBlockContent();
    vector<Data> dataOut;
    vector<int> block_id;
    int Outtype = tableIn.getAttr().type[position];
    while( content[0] != '\0' ){ //该块被写过->可能满了或可能还有存量
        bptr->setPin(true);
        for( int rowPos = 0 ; rowPos < strlen(content) ; rowPos = nextposrow(content, rowPos)){
            char *stringRow = SubString(content, rowPos, nextposrow(content, rowPos)-rowPos);
            if( stringRow[0] == '#' )//已删除
                continue;
            Tuple *tmp_tuper = Char2Tuper(tableIn, stringRow);
            dataOut.push_back(tmp_tuper->getData()[position]);//压入data
            block_id.push_back(block_num);
            delete tmp_tuper;
        }
        block_num++;
        bptr->setPin(false);//解锁
        bptr = buffer_manager->getBlockMem(filename, block_num);
        content = bptr->getBlockContent();
    }
    indexManager *in_manager = new indexManager;
    in_manager->CreateIndex(indexName, dataOut, block_id, Outtype);
}

bool RecordManager::isunique(Table& tableIn, Where w, int loca){ // loca为属性位置
    string filename = "record_" + tableIn.getTitle();
    int intype = tableIn.getAttr().type[loca]; //该属性的flag
    if( tableIn.getAttr().has_index[loca] == true ){ //该属性存在索引
        indexManager *in_manager = new indexManager;
        int location;
        for( location = 0 ; location < tableIn.getIndex().num ; location ++ ){
            if( tableIn.getIndex().location[location] == loca )
                break;
        }
        if( in_manager->Find(tableIn.getIndex().indexname[location].c_str(), w.data) != -1 ){
            return false;
        }
    }
    int block_num = 0;
    blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
    char * content = bptr->getBlockContent();
    while( content[0] != '\0' ){
        bptr->setPin(true);//锁定
        for( int rowPos = 0 ; rowPos < strlen(content) ; rowPos = nextposrow(content, rowPos)){
            char *stringRow = SubString(content, rowPos+1, nextposrow(content, rowPos)-rowPos-2);
            char *data;
            int attr_index = 0 , dataPos = 0;
            for ( ; attr_index < loca  ; attr_index++ , dataPos = nextposdata(stringRow, dataPos)+1 );
            data = SubString(stringRow, dataPos, nextposdata(stringRow, dataPos)-dataPos);
            if( intype == -1 ){ //int
                if( w.data.datai == atoi(data) )
                    return false;
            }
            else if( intype == 0 ){ //float
                if( w.data.dataf == atof(data) )
                    return false;
            }
            else {
                if( w.data.datas == data )
                    return false;
            }
        }
        block_num++;
        bptr->setPin(false);//解锁
        bptr = buffer_manager->getBlockMem(filename, block_num);
        content = bptr->getBlockContent();
    }
    return true;
}

int RecordManager::Delete(Table &tableIn, vector<int> mask, vector<Where> w){
    string filename = "record_" + tableIn.getTitle();
    //获取文件block
    int block_num = 0;
    int delet_count = 0;

    blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
    char * content = bptr->getBlockContent();
    while( content[0] != '\0' ){
        bptr->setPin(true);
        for( int rowPos = 0 ; rowPos < strlen(content) ; rowPos = nextposrow(content, rowPos)){
            char *stringRow = SubString(content, rowPos, nextposrow(content, rowPos)-rowPos);
            if( stringRow[0] == '#' )
                continue;
            Tuple *tmp_tuper = Char2Tuper(tableIn, stringRow);
            if( isSatisfied(tableIn, *tmp_tuper, mask, w) == true ){
                content[rowPos] = '#';
                delet_count++;
                bptr->setDirty(true);
            }
        }
        block_num++;
        bptr->setPin(false);//解锁
        bptr = buffer_manager->getBlockMem(filename, block_num);
        content = bptr->getBlockContent();
    }
    return delet_count;
}

Table RecordManager::Select(Table &tableIn, vector<int> attrSelect, vector<int> mask, vector<Where> w){
    string filename = "record_" + tableIn.getTitle();
    if( mask.size() == 0 ){//无选择
        int block_num = 0;
        blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
        char * content = bptr->getBlockContent();
        while( content[0] != '\0' ){ //该块被写过->可能满了或可能还有存量
            for( int rowPos = 0 ; rowPos < strlen(content) ; rowPos = nextposrow(content, rowPos)){
                char *stringRow = SubString(content, rowPos, nextposrow(content, rowPos)-rowPos);
                if( stringRow[0] == '#' )//已删除
                    continue;
                Tuple *tmp_tuper = Char2Tuper(tableIn, stringRow);
                tableIn.getTuple().push_back(*tmp_tuper);
            }
            block_num++;
            bptr = buffer_manager->getBlockMem(filename, block_num);
            content = bptr->getBlockContent();
        }
        return SelectProject(tableIn, attrSelect);
    }
    //获取文件block
    int block_num = 0;
    //通过索引搜索
    blockInfo *bptr = buffer_manager->getBlockMem(filename, block_num);
    char * content = bptr->getBlockContent();
    while( content[0] != '\0' ){ //该块被写过->可能满了或可能还有存量
        for( int rowPos = 0 ; rowPos < strlen(content) ; rowPos = nextposrow(content, rowPos)){
            char *stringRow = SubString(content, rowPos, nextposrow(content, rowPos)-rowPos);
            if( stringRow[0] == '#' )//已删除
                continue;
            Tuple *tmp_tuper = Char2Tuper(tableIn, stringRow);
            if( isSatisfied(tableIn, *tmp_tuper, mask, w) == true ){
                tableIn.getTuple().push_back(*tmp_tuper);
            }
            else delete tmp_tuper;
        }
        block_num++;
        bptr = buffer_manager->getBlockMem(filename, block_num);
        content = bptr->getBlockContent();
    }
    return SelectProject(tableIn, attrSelect);
}

Table RecordManager::SelectProject(Table &tableIn, vector<int> attrSelect){
    Attribute attrOut;

    attrOut.num = (int)attrSelect.size(); //选择的属性数和堆的大小一样
    for ( int i = 0 ; i < attrSelect.size() ; i ++ ){
        attrOut.type[i] = tableIn.getAttr().type[attrSelect[i]];
        attrOut.name[i] = tableIn.getAttr().name[attrSelect[i]];
        attrOut.unique[i] = tableIn.getAttr().unique[attrSelect[i]];
    }
    Table tableOut(tableIn.getTitle(),attrOut);
    for ( int i = 0 ; i < tableIn.getTuple().size() ; i ++ ){ //遍历tableIn的元组
        Tuple tmp_tuper;//要压进新table的tuper
        for ( int j = 0 ; j < attrSelect.size() ; j ++ ){
            int idx = attrSelect[j] ;
            Data resadd ;//要压进新tuper的data
            if ( tableIn.getTuple()[i].getData()[idx].type == -1 ){ // int
                resadd.type = -1;
                resadd.datai = tableIn.getTuple()[i].getData()[idx].datai;
            }
            else if ( tableIn.getTuple()[i].getData()[idx].type == 0 ){ // int
                resadd.type = 0;
                resadd.dataf = tableIn.getTuple()[i].getData()[idx].dataf;
            }
            else if( tableIn.getTuple()[i].getData()[idx].type > 0 ){ // int
                resadd.type = tableIn.getTuple()[i].getData()[idx].type;
                resadd.datas = tableIn.getTuple()[i].getData()[idx].datas;
            }
            tmp_tuper.addData(resadd);
        }
        tableOut.getTuple().push_back(tmp_tuper);//tuper压入table
    }
    return tableOut;
}
