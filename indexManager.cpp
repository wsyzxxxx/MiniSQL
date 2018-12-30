#include <iostream>
#include <string>

#include "basic.h"
#include "definitions.h"
#include "bplustree.h"
#include "indexManager.h"
#include "bufferManager.h"


int indexManager::Find(const char* indexName, Data key){
	bplustree bpt(indexName);
	return bpt.Find(key);
}

bool indexManager::Insert(const char* indexName, Data key, int b_id){
	bplustree bpt(indexName);
	return bpt.Insert(key,b_id);
}

bool indexManager::Remove(const char* indexName, Data key){
	bplustree bpt(indexName);
	return bpt.Remove(key);
}

bool indexManager::CreateIndex(const char* indexName, 
							   std::vector<Data> v, 
							   std::vector<int> v_id,
							   int type
							  )
{
	bplustree bpt(indexName,v,v_id,type);
	return 1;
}

bool indexManager::DropIndex(const char* indexName){
	std::string path(indexName);
	path="index_"+path;
	delete buffer_manager;
	buffer_manager=new bufferManager();
	return remove(path.c_str())==0;
}

bool indexManager::Show(const char* indexName){
	bplustree bpt(indexName);
	return bpt.Show();
}
