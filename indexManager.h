#ifndef _INDEX_MANAGER_H_
#define _INDEX_MANAGER_H_

#include "basic.h"

class indexManager{
	public:
    	// �������в���keyֵ������block_id��ʧ���򷵻�-1 
    	int Find(const char* indexName,    //�������� 
				 Data key				   //keyֵ 
				);


    	// ����<key,block_id>�������У��ɹ��򷵻�1 
    	bool Insert(const char* indexName, //�������� 
					Data key, 			   //keyֵ 
					int b_id			   //block_id
				   );


	    // ��������ɾ��keyֵ���ɹ��򷵻�1 
	    bool Remove(const char* indexName, //��������
					Data key);			   //keyֵ 


    	// ����index���ɹ��򷵻�1 
	    bool CreateIndex(const char* indexName,  //�������� 
						 std::vector<Data> v,    //�ñ���������������keyֵ�� �������� 
						 std::vector<int> v_id,	 //��keyֵ��Ӧ��block_id
						 int type				 //keyֵ��type 
						);
	
	
	    // ɾ���������ɹ��򷵻�1 
	    bool DropIndex(const char* indexName     //�������� 
					  );
		
		// չʾb+�����ɹ��򷵻�1 
		bool Show(const char* indexName);		 //�������� 
};

#endif

