#ifndef _INDEX_MANAGER_H_
#define _INDEX_MANAGER_H_

#include "basic.h"

class indexManager{
	public:
    	// 在索引中查找key值，返回block_id，失败则返回-1 
    	int Find(const char* indexName,    //索引名称 
				 Data key				   //key值 
				);


    	// 插入<key,block_id>到索引中，成功则返回1 
    	bool Insert(const char* indexName, //索引名称 
					Data key, 			   //key值 
					int b_id			   //block_id
				   );


	    // 从索引中删除key值，成功则返回1 
	    bool Remove(const char* indexName, //索引名称
					Data key);			   //key值 


    	// 创建index，成功则返回1 
	    bool CreateIndex(const char* indexName,  //索引名称 
						 std::vector<Data> v,    //该表用来建立索引的key值集 无需排序 
						 std::vector<int> v_id,	 //与key值对应的block_id
						 int type				 //key值的type 
						);
	
	
	    // 删除索引，成功则返回1 
	    bool DropIndex(const char* indexName     //索引名称 
					  );
		
		// 展示b+树，成功则返回1 
		bool Show(const char* indexName);		 //索引名称 
};

#endif

