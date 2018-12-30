
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>

#include "basic.h"
#include "definitions.h"
#include "bplustree.h"
#include "node.h"


bplustree::bplustree(const char *indexName){
	std::string tmp(indexName);
	path="index_"+tmp;
	blockInfo* block=buffer_manager->getBlockMem(path,0);
	char* s=block->getBlockContent();
	int st=0;
	keytype=nextInt(s,st);
	root=nextInt(s,st);
	degree=nextInt(s,st);
	num=nextInt(s,st);
	num_r=nextInt(s,st);
	if (num_r*2<num){
		std::vector<Data_with_id> v_; v_.clear();
		for (int i=1;i<=num;i++){
			node now(path,i);
			if (now.isLeaf){
				for (int j=0;j<now.num;j++){
					Data_with_id tmp;
					tmp.x=now.keys[j];
					tmp.id=now.childs[j];
					v_.push_back(tmp);
				}
			}
		}
		build(v_);
	}
}

bplustree::~bplustree(){
	blockInfo* block=buffer_manager->getBlockMem(path,0);
	char* s=block->getBlockContent();
	s[0]='\0';
	sprintf(s,"%d|%d|%d|%d|%d|",keytype,root,degree,num,num_r);
	block->setDirty();
}

int bplustree::GetDegree(int type){
	int sz,szint=20;
	if (type<=0) sz=szint; else sz=256;
    int degree=(BLOCK_SIZE-BLOCK_HEADER-(szint+1)*4)/(sz+szint+2); 
    return degree;
}

bplustree::bplustree(const char *indexName,std::vector<Data> v,std::vector<int> v_id,int type){
	
	std::string tmp(indexName);
	path="index_"+tmp;
	buffer_manager->createNewFile(path,2,v.size()*2);
	keytype=type;
	
	degree=GetDegree(keytype);
	
	std::vector<Data_with_id> v_;
	for (int i=0;i<v.size();i++){
		Data_with_id tmp;
		tmp.x=v[i];
		tmp.id=v_id[i];
		v_.push_back(tmp);
	}
	build(v_);
}

void bplustree::build(std::vector<Data_with_id> v_){
	
	sort(v_.begin(),v_.end());
	
	int tou=1,wei=0,md=(degree+1)/2;int lmt=md;
	node now; now.isLeaf=1; now.path=path; now.type=keytype;
	for (int i=0;i<=v_.size();i++){
		if (now.num+v_.size()-i<=degree) lmt=degree;
		if (now.num==lmt||i==v_.size()){
			now.id=++wei;
			now.writeBack();
			now.clear(); now.isLeaf=1,now.path=path,now.type=keytype;
		}
		if (i==v_.size()) break;
		now.num++;
		now.keys.push_back(v_[i].x);
		now.childs.push_back(v_[i].id);
	}
	
	node treei;
	while (tou<wei){
		int WEI=wei;
		now.clear(); now.isLeaf=0; now.path=path; now.type=keytype; lmt=md;
		
		for (int i=tou;i<=WEI+1;i++){
			if (now.num+WEI+1-i<=degree) lmt=degree;
			if (now.num==lmt||i==WEI+1){
				now.id=++wei;
				now.writeBack();
				now.clear(); now.isLeaf=0,now.type=keytype,now.path=path;
			}
			if (i>WEI) break;
			treei.make(path,i); treei.father=wei+1; treei.writeBack();
			now.num++;
			now.keys.push_back(treei.keys[0]);
			now.childs.push_back(treei.id);
		}
		tou=WEI+1;
	}
	root=num=num_r=wei;
}

int bplustree::Find_id(Data v){
	int now_id=root;
	node now(path,now_id); bool flag;
	if (now.num==0) return root;
	while (!now.isLeaf){
		flag=0;
		for (int i=1;i<now.num;i++)
		if (v<now.keys[i]){
			now_id=now.childs[i-1];
			flag=1;
			break;
		}
		if (!flag) now_id=now.childs[now.num-1];
		now.make(path,now_id);
	}
	return now_id;
}

int bplustree::Find(Data v){
	int id=Find_id(v);
	if (id==-1) return -1;
	node now(path,id);
	for (int i=0;i<now.num;i++)
	if (now.keys[i]==v) return now.childs[i];
	return -1;
}

bool bplustree::Insert(Data v,int b_id){
	int now_id=Find_id(v);
	if (now_id<=0) return 0;
	node now(path,now_id);

	node newNode;
	newNode.id=b_id;
	newNode.num=1;
	newNode.type=keytype;
	newNode.keys.push_back(v);
	
	while (newNode.num>0){
		if (now.id<=0){
			now.make(path,root);
			node new_root;
			new_root.num=2; new_root.id=root=++num; new_root.type=keytype; num_r++;
			now.father=newNode.father=root;
			new_root.path=path;
			if (now.keys[0]>newNode.keys[0]) swap(now,newNode);
			new_root.keys.push_back(now.keys[0]);
			new_root.keys.push_back(newNode.keys[0]);
			new_root.childs.push_back(now.id);
			new_root.childs.push_back(newNode.id);
			break;
		}
		
		if (!now.isLeaf){
			node Son(path,now.childs[0]);
			now.keys[0]=Son.keys[0];
		}
		
		now.num++;
		now.keys.push_back(newNode.keys[0]);
		now.childs.push_back(newNode.id);

		for (int i=now.keys.size()-2;i>=0;i--)
		if (now.keys[i]>now.keys[i+1]){
			swap(now.keys[i],now.keys[i+1]);
			swap(now.childs[i],now.childs[i+1]);
		}
		
		if (now.num>degree){
			newNode.clear();
			newNode.isLeaf=now.isLeaf;
			newNode.path=path;
			newNode.id=++num; num_r++;
			newNode.father=now.father;
			newNode.type=keytype;
			std::vector<Data>::iterator it_keys=now.keys.begin()+now.num/2;
			std::vector<int>::iterator it_childs=now.childs.begin()+now.num/2;
    		for(;it_keys!=now.keys.end();){
	    		newNode.keys.push_back(*it_keys);
	    		newNode.childs.push_back(*it_childs);
	    		newNode.num++;
	    		now.num--;
	    		if (!now.isLeaf){
	    			node son(path,*it_childs);
	    			son.father=newNode.id;
	    		}
	            it_keys=now.keys.erase(it_keys);
	            it_childs=now.childs.erase(it_childs);
		    }
		    now.writeBack();
		    newNode.writeBack();
		    now.make(path,newNode.father);
		} else newNode.clear(),newNode.num=-1,now.writeBack();
	}
	
	node father;
	while (now.father>0){
		father.make(path,now.father);
		if (father.childs[0]==now.id&&!(father.keys[0]==now.keys[0])){
			father.keys[0]=now.keys[0];
			father.writeBack();
			now=father;
		}
		else break;
	}
		
	return 1;
}

bool bplustree::Remove(Data v){
	int now_id=Find_id(v),lmt=(degree+1)/2;
	node now(path,now_id),father;
	std::vector<Data>::iterator it=find(now.keys.begin(),now.keys.end(),v),it_f,it_F;
	if (it==now.keys.end()) return -1;
	
	while (1){
		
		if (now.num==2&&now.father<=0&&!now.isLeaf){
			root=now.childs[0];
			now.make(path,root);
			now.father=0;
			num_r--;
			break;
		}

		if (now.father>0){
			father.make(path,now.father);
			it_F=father.keys.begin()+(find(father.childs.begin(),father.childs.end(),now.id)-father.childs.begin());
		}
		
		std::vector<int>::iterator it_id=now.childs.begin()+(it-now.keys.begin());
		now.keys.erase(it);
		now.childs.erase(it_id);
		now.num--;
		
		if (now.father>0){
			*it_F=now.keys[0];
		}
		
		if (now.num<lmt&&now.father>0){
			node siblingl; 
			bool sbl=0;
			if (it_F!=father.keys.begin()){
				siblingl.make(path,father.childs[(it_F-father.keys.begin())-1]);
				it_f=it_F;
				sbl=1;
			}
			if (!sbl&&it_F!=father.keys.end()-1){
				siblingl=now;
				now.make(path,father.childs[it_F-father.keys.begin()+1]);
				it_f=it_F+1;
				sbl=1; 
			}
			if (sbl){
				if (siblingl.num+now.num<=degree){
					num_r--;
					siblingl.num+=now.num;
					for (int i=0;i<now.keys.size();i++){
						if (!now.isLeaf){
							node Son(path,now.childs[i]);
							Son.father=siblingl.id;
						}
						siblingl.keys.push_back(now.keys[i]);
						siblingl.childs.push_back(now.childs[i]);
					}
					now.isLeaf=0; now.writeBack();
					now=father;
					it=now.keys.begin()+(it_f-father.keys.begin());
				} else{
					if (siblingl.num>now.num){
						if (!now.isLeaf){
							node Son(path,siblingl.childs[siblingl.num-1]);
							Son.father=now.id;
						}
						*it_f=*(siblingl.keys.end()-1);
						now.keys.insert(now.keys.begin(),*(siblingl.keys.end()-1));
						siblingl.keys.erase(siblingl.keys.end()-1);
						now.childs.insert(now.childs.begin(),*(siblingl.childs.end()-1));
						siblingl.childs.erase(siblingl.childs.end()-1);
						siblingl.num--; now.num++;
					} else{
						if (!now.isLeaf){
							node Son(path,now.childs[0]);
							Son.father=siblingl.id;
						}
						*it_f=now.keys[1];
						siblingl.keys.push_back(*now.keys.begin());
						now.keys.erase(now.keys.begin());
						siblingl.childs.push_back(*now.childs.begin());
						now.childs.erase(now.childs.begin());
						siblingl.num++; now.num--;
					}
					father.writeBack();
					break;
				}
			} //else return 0;
		} else break;
	}
	now.writeBack();
	
	if (now.father>0)
	while (it_F==father.keys.begin()){
		father.writeBack();
		now=father;
		if (now.father<=0) break;
		father.make(path,now.father);
		it_F=father.keys.begin()+(find(father.childs.begin(),father.childs.end(),now.id)-father.childs.begin());
		if (!((*it_F)==now.keys[0])) *it_F=now.keys[0];
		else break;
	}
	
	return 1;
}

bool bplustree::dfs(int x){
	node now(path,x),child;
	bool rec=1;
	
	if (now.keys.size()!=now.num){
		printf("keys.size()!=now.num!\n");
		rec=0;
	}
	
	if (now.childs.size()!=now.num){
		printf("childs.size()!=now.num!\n");
		rec=0;
	}
	
	for (int i=0;i<now.num;i++){
		if (i>0&&now.keys[i].datai<=now.keys[i-1].datai){
			printf("Keys not in order!\n");
			rec=0;
		}
		if (!now.isLeaf){
			child.make(path,now.childs[i]);
			if (!(child.keys[0]==now.keys[i])){
				printf("Keys[0] Error!\n");
				rec=0;
			}
			if (child.father!=x){
				printf("father Error!\n");
				rec=0;
			}
		}
	}
	
	if (!now.isLeaf){
		for (int i=0;i<now.num;i++){
			if (!dfs(now.childs[i])) rec=0;
		}
	} 
	return rec;
}

bool bplustree::Show(){
	printf("num=%d root=%d\n\n",num,root);
	return dfs(root);
}


