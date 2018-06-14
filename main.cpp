//
//  main.cpp
//  miniSQL
//
//  Created by 薛伟 on 2018/6/11.
//  Copyright © 2018年 Will. All rights reserved.
//

#include <iostream>
#include "bufferManager.h"

using namespace std;

bufferManager* buffer_manager;

int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Hello, World!" << endl;
    cout << "Start testing!" << endl;
    
    buffer_manager = new bufferManager();
    
    buffer_manager->createNewFile("catalog_test", 0, 80);
    buffer_manager->createNewFile("record_test", 1, 100);
    buffer_manager->createNewFile("index_indexoftest_test", 2, 50);
    
    blockInfo* catalog_block2 = buffer_manager->getBlockMem("catalog_test", 1);
    char* cata2 = catalog_block2->getBlockContent();
    cout << cata2 << endl;
    sprintf(cata2, "Hello world! This is a cata of block 1!\n");
    catalog_block2->setDirty();
    
    blockInfo *catalog_block = buffer_manager->getBlockMem("catalog_test", 0);
    char* cata = catalog_block->getBlockContent();
    cout << cata << endl;
    sprintf(cata, "Hello world! This is a cata of block 0!\n");
    catalog_block->setDirty();
    
    blockInfo* record_block = buffer_manager->getBlockMem("record_test", 0);
    char* record = record_block->getBlockContent();
    cout << record << endl;
    sprintf(record, "Hello world! This is a record of block 0!\n");
    record_block->setDirty();
    
    blockInfo* index_block = buffer_manager->getBlockMem("index_indexoftest_test", 0);
    char* index = index_block->getBlockContent();
    cout << index << endl;
    sprintf(index, "Hello world! This is a index of block 0!\n");
    index_block->setDirty();
    
    delete buffer_manager;
    
    return 0;
}
