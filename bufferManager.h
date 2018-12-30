
/*************************************************
 BufferManager使用方法:
 1.存在一个全局的bufferManager* buffer_manager;在各自的模块使用extern引用即可
 2.进行文件操作时，必须先创建文件，调用buffer_manager->createNewFile，文件名格式可参考definitions.h
 3.创建文件后，调用getBlockMem获得文件内的block，默认从block_id为0开始，第一个不够可再获取第二个，请按顺序使用
 4.获得block后即可对当前block进行操作，操作前使用setPin()，将该block锁定
 5.然后使用block->getBlockContent()可获取当前block的char*的字符流内容，可自行设计编排存储。这个大小为BLOCK_SIZE-BLOCK_HEADER
 6.另外block的belong_to_file存放了一些文件信息，每个文件以block_id为0的block存放为准
 7.操作完block后记得setPin(false)取消锁定，如果有修改内容则setDirty()将页面标记为已经修改
 8.如果需要可以调用buffer_manager的writeBackBlock将某个块写回到文件。不过最终这一步不需要手动完成，buffermanager会在析构的时候将内存中所有块直接写回文件。所以自己测试的时候注意手动写回或者确保buffer_manager完成了析构函数再退出程序。
**************************************************/

#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include <string>
#include <vector>
#include <time.h>
#include <list>
#include <unordered_map>
#include "definitions.h"

using namespace std;

struct fileInfo {
    int file_type;               //文件类型：catalog文件file_type = 1, record文件file_type = 2, index文件file_type = 3;
    string file_name;            //文件名
    int record_length;           //文件中每条记录的长度
    int block_num;               //文件中block数量
    int first_deletion_offset;   //文件中第一个删除的记录的偏移量
    int record_num;              //文件中记录的数量
    //blockInfo* first_block;      //指向文件起始block的指针
    fileInfo() {};
    fileInfo(int t, string n, int l, int b, int o, int r):
        file_type(t),
        file_name(n),
        record_length(l),
        block_num(b),
        first_deletion_offset(o),
        record_num(r) {};
};

struct LRUKey {
    string file_name;
    int block_id;
    LRUKey(string file_name, int block_id): file_name(file_name), block_id(block_id) {};
};

struct LRUKey_HASH {
    size_t operator()(const LRUKey& key) const {
        hash<string> hash_string;
        hash<int>    hash_int;
        hash<size_t> hash_sizet;
        return hash_sizet(hash_string(key.file_name) + hash_int(key.block_id));
    }
};

struct LRUKey_CMP {
    bool operator() (const LRUKey& key1, const LRUKey& key2) const {
        return (key1.block_id == key2.block_id && key1.file_name == key2.file_name);
        
    }
};

class blockInfo {
private:
    bool is_dirty;              //是否被修改过
    bool is_pinned;        //被pin的次数(可能存在多个操作pin同一个页的情况)
    //int block_offset;         //block的偏移量
    char block_mem [BLOCK_SIZE - BLOCK_HEADER];//block内的内容

public:
    int block_id;               //block的id，在file中的id，从0开始，0，1，2，3...
    fileInfo belong_to_file;    //属于哪个file
    
    blockInfo(char* block_content, fileInfo file_info, int block_id);    //构造函数
    ~blockInfo();                                          //析构函数
    void setDirty(bool dirty_byte = true);                 //设置or取消某block的dirty状态
    bool isDirty();                                        //获取block的dirty状态
    void setPin(bool pinned = true);                       //增加or减少某block的pin次数
    bool isPinned();                                       //获取某block的pin状态
    char* getBlockContent();                               //获取block的内存内容
};

class bufferManager {
private:
    list<blockInfo*> block_pool;   //block池，存储所有内存中的block
    unordered_map<LRUKey, list<blockInfo*>::iterator, LRUKey_HASH, LRUKey_CMP> block_map; // 用于LRU计算的map，降低时间复杂度
    
    blockInfo* readBlockIntoMemory(string file_name, int block_id);//读取一个Block到内存中
    void writeBlockBackToDisk(blockInfo *block);                //将内存中的一个块写到磁盘上
    void initFileBlock(string file_name, int block_id, int file_type, int record_length);         //新初始化一个文件的block
    //blockInfo openFile(string file_name);                     //暂时不用
    //void closeFile(string file_name);
    blockInfo* LRUForGetBlock(LRUKey block);                    //LRU的block替换
    void LRUForPutBlock(LRUKey block_key, blockInfo *block);    //LRU的block替换
    
public:
    bufferManager();                //构造函数
    ~bufferManager();               //析构函数
    blockInfo* getBlockMem(string file_name, int block_id);     //获取某block在内存中的句柄
    void createNewFile(string file_name, int file_type, int record_length);        //创建一个新的文件，文件类型0:catalog, 1:record, 2:index
    void writeBackBlock(blockInfo *block);                      //将block写回磁盘，一般不需要手动写回
    
};

#endif /* _BUFFERMANAGER_H_ */
