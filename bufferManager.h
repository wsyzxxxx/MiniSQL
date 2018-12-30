
/*************************************************
 BufferManagerʹ�÷���:
 1.����һ��ȫ�ֵ�bufferManager* buffer_manager;�ڸ��Ե�ģ��ʹ��extern���ü���
 2.�����ļ�����ʱ�������ȴ����ļ�������buffer_manager->createNewFile���ļ�����ʽ�ɲο�definitions.h
 3.�����ļ��󣬵���getBlockMem����ļ��ڵ�block��Ĭ�ϴ�block_idΪ0��ʼ����һ���������ٻ�ȡ�ڶ������밴˳��ʹ��
 4.���block�󼴿ɶԵ�ǰblock���в���������ǰʹ��setPin()������block����
 5.Ȼ��ʹ��block->getBlockContent()�ɻ�ȡ��ǰblock��char*���ַ������ݣ���������Ʊ��Ŵ洢�������СΪBLOCK_SIZE-BLOCK_HEADER
 6.����block��belong_to_file�����һЩ�ļ���Ϣ��ÿ���ļ���block_idΪ0��block���Ϊ׼
 7.������block��ǵ�setPin(false)ȡ��������������޸�������setDirty()��ҳ����Ϊ�Ѿ��޸�
 8.�����Ҫ���Ե���buffer_manager��writeBackBlock��ĳ����д�ص��ļ�������������һ������Ҫ�ֶ���ɣ�buffermanager����������ʱ���ڴ������п�ֱ��д���ļ��������Լ����Ե�ʱ��ע���ֶ�д�ػ���ȷ��buffer_manager����������������˳�����
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
    int file_type;               //�ļ����ͣ�catalog�ļ�file_type = 1, record�ļ�file_type = 2, index�ļ�file_type = 3;
    string file_name;            //�ļ���
    int record_length;           //�ļ���ÿ����¼�ĳ���
    int block_num;               //�ļ���block����
    int first_deletion_offset;   //�ļ��е�һ��ɾ���ļ�¼��ƫ����
    int record_num;              //�ļ��м�¼������
    //blockInfo* first_block;      //ָ���ļ���ʼblock��ָ��
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
    bool is_dirty;              //�Ƿ��޸Ĺ�
    bool is_pinned;        //��pin�Ĵ���(���ܴ��ڶ������pinͬһ��ҳ�����)
    //int block_offset;         //block��ƫ����
    char block_mem [BLOCK_SIZE - BLOCK_HEADER];//block�ڵ�����

public:
    int block_id;               //block��id����file�е�id����0��ʼ��0��1��2��3...
    fileInfo belong_to_file;    //�����ĸ�file
    
    blockInfo(char* block_content, fileInfo file_info, int block_id);    //���캯��
    ~blockInfo();                                          //��������
    void setDirty(bool dirty_byte = true);                 //����orȡ��ĳblock��dirty״̬
    bool isDirty();                                        //��ȡblock��dirty״̬
    void setPin(bool pinned = true);                       //����or����ĳblock��pin����
    bool isPinned();                                       //��ȡĳblock��pin״̬
    char* getBlockContent();                               //��ȡblock���ڴ�����
};

class bufferManager {
private:
    list<blockInfo*> block_pool;   //block�أ��洢�����ڴ��е�block
    unordered_map<LRUKey, list<blockInfo*>::iterator, LRUKey_HASH, LRUKey_CMP> block_map; // ����LRU�����map������ʱ�临�Ӷ�
    
    blockInfo* readBlockIntoMemory(string file_name, int block_id);//��ȡһ��Block���ڴ���
    void writeBlockBackToDisk(blockInfo *block);                //���ڴ��е�һ����д��������
    void initFileBlock(string file_name, int block_id, int file_type, int record_length);         //�³�ʼ��һ���ļ���block
    //blockInfo openFile(string file_name);                     //��ʱ����
    //void closeFile(string file_name);
    blockInfo* LRUForGetBlock(LRUKey block);                    //LRU��block�滻
    void LRUForPutBlock(LRUKey block_key, blockInfo *block);    //LRU��block�滻
    
public:
    bufferManager();                //���캯��
    ~bufferManager();               //��������
    blockInfo* getBlockMem(string file_name, int block_id);     //��ȡĳblock���ڴ��еľ��
    void createNewFile(string file_name, int file_type, int record_length);        //����һ���µ��ļ����ļ�����0:catalog, 1:record, 2:index
    void writeBackBlock(blockInfo *block);                      //��blockд�ش��̣�һ�㲻��Ҫ�ֶ�д��
    
};

#endif /* _BUFFERMANAGER_H_ */
