//
//  bufferManager.cpp
//  miniSQL
//
//  Created by 薛伟 on 2018/6/11.
//  Copyright © 2018年 Will. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "bufferManager.h"

using namespace std;

blockInfo::blockInfo(char* block_content, fileInfo file_info, int block_id)
{
    belong_to_file = file_info;
    memcpy(block_mem, block_content, BLOCK_SIZE - BLOCK_HEADER);
    this->block_id = block_id;
    this->is_dirty = false;
    this->is_pinned = false;
}

blockInfo::~blockInfo() {}

void blockInfo::setDirty(bool dirty_byte)
{
    if (dirty_byte) this->is_dirty = true;
    else this->is_dirty = false;
}

bool blockInfo::isDirty()
{
    return this->is_dirty;
}

void blockInfo::setPin(bool pinned)
{
    if (pinned) {
        if (this->is_pinned)
            cout << "ERROR: The page has been pinned" << endl;
        this->is_pinned = true;
    }
    else
        this->is_pinned = false;
}

bool blockInfo::isPinned()
{
    return is_pinned;
}

char* blockInfo::getBlockContent()
{
    return block_mem;
}


bufferManager::bufferManager()
{
    block_pool.clear();
    block_map.clear();
}

bufferManager::~bufferManager()
{
    block_map.clear();
    
    for (auto index_of_block = block_pool.begin(); index_of_block != block_pool.end(); ) {
        if ((*index_of_block)->isPinned()) {
            cout << "ERROR: The block is still pinned!" << endl;
            //MARK: 错误处理
        } else if ((*index_of_block)->isDirty())
            writeBlockBackToDisk(*index_of_block);
        
        index_of_block = block_pool.erase(index_of_block);
    }
    
    if (block_pool.size() != 0) {
        cout << "ERROR: clear vector error!" << endl;
        //MARK: 错误处理
    }
}

void bufferManager::createNewFile(string file_name, int file_type, int record_length)
{
    ofstream new_file;
    new_file.open("./" + file_name, ios::in);
    //检查文件是否存在
    if (new_file) {
        cout << "ERROR: The file has existed!" << endl;
        //MARK: 错误处理
        return;
    } else
        new_file.open("./" + file_name, ios::trunc);
    new_file.close();
    initFileBlock(file_name, 0, file_type, record_length);
}

void bufferManager::initFileBlock(string file_name, int block_id, int file_type, int record_length)
{
//    ofstream new_file;
//    new_file.open("./" + file_name, ios::out);
//    new_file.seekp(BLOCK_SIZE * block_id, ios::beg);
//    //填满文件一个空BLOCK的大小
//    char temp_str[BLOCK_SIZE];
//    memset(temp_str, 0, sizeof(temp_str));
//    new_file.write(temp_str, BLOCK_SIZE);
//    //回到文件开头
//    new_file.seekp(BLOCK_SIZE * block_id, ios::beg);
//    //对应fileInfo里面的五类
//    new_file << file_type << ' ' << record_length << ' ' << 0 << ' ' << 0 << ' ' << 0 << '\n';
//    cout << "current pos: " << new_file.tellp() << endl;
//    new_file.close();
    FILE* new_block_file = fopen(file_name.data(), "r+");
    fseek(new_block_file, BLOCK_SIZE * block_id, SEEK_SET);
    char temp_str[BLOCK_SIZE];
    memset(temp_str, 0, sizeof(temp_str));
    fwrite(temp_str, BLOCK_SIZE, 1, new_block_file);
    fseek(new_block_file, BLOCK_SIZE * block_id, SEEK_SET);
    sprintf(temp_str, "%d %d 0 0 0\n", file_type, record_length);
    fwrite(temp_str, strlen(temp_str), 1, new_block_file);
    fclose(new_block_file);
}

blockInfo* bufferManager::readBlockIntoMemory(string file_name, int block_id)
{
    if (block_id > 0) {
        blockInfo *first_block = getBlockMem(file_name, 0);
        if (first_block->belong_to_file.block_num < block_id) {
            first_block->belong_to_file.block_num = block_id;
            first_block->setDirty();
            initFileBlock(file_name, block_id, first_block->belong_to_file.file_type, first_block->belong_to_file.record_length);
        }
    }
    ifstream read_file;
    read_file.open("./" + file_name, ios::in);
    //判断文件是否存在
    if (!read_file) {
        cout << "ERROR: The file does not exist!" << endl;
        //MARK:: 错误处理
    }
    //读取block的header内容
    read_file.seekg(BLOCK_SIZE * block_id, ios::beg);
    int block_type, record_length, block_num, first_deletion_offset, record_num;
    read_file >> block_type >> record_length >> block_num >> first_deletion_offset >> record_num;
    fileInfo read_file_info(block_type, file_name, record_length, block_num, first_deletion_offset, record_num);
    //读取记录内容
    read_file.seekg(BLOCK_SIZE * block_id + BLOCK_HEADER, ios::beg);
    char temp_conent[BLOCK_SIZE - BLOCK_HEADER];
    read_file.read(temp_conent, BLOCK_SIZE - BLOCK_HEADER);
    //read_file >> temp_conent;
    read_file.close();
    //构造新的blockInfo
    blockInfo* new_block = new blockInfo(temp_conent, read_file_info, block_id);
    //加入LRU队列
    LRUForPutBlock(LRUKey(file_name, block_id), new_block);
    
    return new_block;
}

void bufferManager::writeBlockBackToDisk(blockInfo *block)
{
    /*
    //打开文件
    string write_file_name = block->belong_to_file.file_name;
    ofstream write_file;
    write_file.open("./" + write_file_name, ios::out);
    //写入文件信息
    write_file.seekp(BLOCK_SIZE * block->block_id, ios::beg);
    write_file << block->belong_to_file.file_type << ' ' << block->belong_to_file.record_length << ' ' << block->belong_to_file.block_num << ' ' << block->belong_to_file.first_deletion_offset << ' ' << block->belong_to_file.record_num << '\n';
    //写入文件内容
    write_file.seekp(BLOCK_SIZE * block->block_id + BLOCK_HEADER, ios::beg);
    write_file.write(block->getBlockContent(), BLOCK_SIZE - BLOCK_HEADER);
    //write_file << block->getBlockContent();
    write_file.close();
     */
    //打开文件
    string write_file_name = block->belong_to_file.file_name;
    FILE* new_block_file = fopen(write_file_name.data(), "r+");
    fseek(new_block_file, BLOCK_SIZE * block->block_id, SEEK_SET);
    //写入header
    char temp_str[BLOCK_SIZE];
    memset(temp_str, 0, sizeof(temp_str));
    sprintf(temp_str, "%d %d %d %d %d\n", block->belong_to_file.file_type, block->belong_to_file.record_length, block->belong_to_file.block_num, block->belong_to_file.first_deletion_offset, block->belong_to_file.record_num);
    fwrite(temp_str, strlen(temp_str), 1, new_block_file);
    //写入内容
    fseek(new_block_file, BLOCK_SIZE * block->block_id + BLOCK_HEADER, SEEK_SET);
    fwrite(block->getBlockContent(), BLOCK_SIZE - BLOCK_HEADER, 1, new_block_file);
    fclose(new_block_file);
}

blockInfo* bufferManager::LRUForGetBlock(struct LRUKey block_key)
{
    if (block_map.find(block_key) == block_map.end())
        return nullptr;
    else {
        block_pool.splice(block_pool.begin(), block_pool, block_map[block_key]);
        block_map[block_key] = block_pool.begin();
        return *(block_pool.begin());
    }
}

void bufferManager::LRUForPutBlock(struct LRUKey block_key, blockInfo *block)
{
    if (block_map.find(block_key) != block_map.end()) {
        block_pool.splice(block_pool.begin(), block_pool, block_map[block_key]);
        block_map[block_key] = block_pool.begin();
    } else if (block_pool.size() == MAX_BLOCK_NUM) {
        list<blockInfo*>::iterator itor;
        itor = block_pool.end(); itor--;
        while (itor != block_pool.begin() && !((*itor)->isPinned())) itor--;
        if (itor == block_pool.begin() && (*itor)->isPinned()) {
            cout << "ERROR: No buffer is available!" << endl;
            //MARK: 错误处理
        } else {
//            if (itor->isDirty())
//                writeBlockBackToDisk(*itor);
//            block_map.erase(LRUKey(itor->belong_to_file.file_name, itor->block_id));
//            block_pool.erase(itor);
            writeBackBlock(*itor);
            block_pool.push_front(block);
            block_map[block_key] = block_pool.begin();
        }
    } else {
        block_pool.push_front(block);
        block_map[block_key] = block_pool.begin();
    }
}

blockInfo* bufferManager::getBlockMem(string file_name, int block_id)
{
    //判断是否存在
    blockInfo* get_block = LRUForGetBlock(LRUKey(file_name, block_id));
    //不存在则读入
    if (!get_block) {
        blockInfo* block_content = readBlockIntoMemory(file_name, block_id);
        //LRUForPutBlock(LRUKey(file_name, block_id), block_content);
        return block_content;
    }
    
    return get_block;
}

void bufferManager::writeBackBlock(blockInfo* block)
{
    if (block->isPinned()) {
        cout << "ERROR: The block is still pinned!" << endl;
        //MARK: 错误处理
        return;
    }
    if (block->isDirty())
        //写回磁盘
        writeBlockBackToDisk(block);
    //清除队列
    block_pool.erase(block_map[LRUKey(block->belong_to_file.file_name, block->block_id)]);
    block_map.erase(LRUKey(block->belong_to_file.file_name, block->block_id));
    delete block;
}
