
#ifndef definitions_h
#define definitions_h

//ÿ��block��size
#define BLOCK_SIZE 4096
//�ڴ�������block����
#define MAX_BLOCK_NUM 4000
//����block��header�Ĵ�С��ʵ�ʿ�������ΪBLOCK_SIZE - BLOCK_HEADER
#define BLOCK_HEADER 80

#define RECORD_LENTH 300
/* �ļ�����������:
        ÿ�������ļ�����ڵ���һ���ļ�(catalog, record, index)
            catalog�ļ���catalog_����,
            record�ļ��� record_������
            index�ļ�:   index_key��_����
 */


#endif /* definitions_h */
