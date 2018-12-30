
#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <iostream>
#include <string>
#include "bufferManager.h"

extern bufferManager* buffer_manager;

class bplustree{
	public:
		bplustree(const char *indexName);
		bplustree(const char *indexName,std::vector<Data> v,std::vector<int> v_id,int type);
		~bplustree();
		int Find(Data v);
		bool Insert(Data v,int b_id);
		bool Remove(Data v);
		bool Show();
	private:
		bool dfs(int x);
		void build(std::vector<Data_with_id> v_);
		int Find_id(Data v);
		int GetDegree(int type);
		std::string path;
		int keytype,root,degree,num,num_r;
};

#endif
