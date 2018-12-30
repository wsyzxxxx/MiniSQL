#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <cstring>
#include <stdlib.h>


#include "basic.h"
#include "definitions.h"
#include "bufferManager.h"

extern bufferManager* buffer_manager;

struct node{
	int num,father,nextNode,id,type;
	bool isLeaf;
	std::string path;
	std::vector<Data> keys;
	std::vector<int> childs;
		
	void clear(){
		num=father=nextNode=id=type=isLeaf=0;
		path="";
		keys.clear();
		childs.clear();
	}
	
	node(){
		clear();
	}
	
	void make(std::string path_,int block_id){
		path=path_;
		blockInfo* block=buffer_manager->getBlockMem(path,block_id);
		char *s=block->getBlockContent();
		int st=0;
		id=block_id;
		num=nextInt(s,st);
		father=nextInt(s,st);
		nextNode=nextInt(s,st);
		isLeaf=nextInt(s,st)>0;
		type=nextInt(s,st);
		keys.clear(); childs.clear();
		for (int i=0;i<num;i++){
			Data tmp;
			tmp.type=type;
			if (type==-1) tmp.datai=nextInt(s,st); else
			if (type==0)  tmp.dataf=nextFloat(s,st); else
						  tmp.datas=nextString(s,st);
			keys.push_back(tmp);
		}
		for (int i=0;i<num;i++) childs.push_back(nextInt(s,st));
	}
	
	node(std::string path_,int block_id){
		make(path_,block_id);
	}
	
	void writeBack(){
		if (id<=0) return;
		blockInfo* block=buffer_manager->getBlockMem(path,id);
		char* s=block->getBlockContent();
		s[0]='\0';
		sprintf(s,"%d|%d|%d|%d|%d|",num,father,nextNode,isLeaf,type);
		for (int i=0;i<keys.size();i++){
			if (keys[i].type==-1) sprintf(s+strlen(s),"%d|",keys[i].datai); else
			if (keys[i].type==0)  sprintf(s+strlen(s),"%.5f|",keys[i].dataf); else
								  sprintf(s+strlen(s),"%s|",keys[i].datas.c_str());
		}
		for (int i=0;i<childs.size();i++) sprintf(s+strlen(s),"%d|",childs[i]);
		block->setDirty();
	}
	
	~node(){
		writeBack();
		path.clear();
		keys.clear();
		childs.clear();
	}
	
};

#endif
