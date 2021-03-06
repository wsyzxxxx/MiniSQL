
#ifndef definitions_h
#define definitions_h

//每个block的size
#define BLOCK_SIZE 4096
//内存中最多的block数量
#define MAX_BLOCK_NUM 4000
//定义block的header的大小，实际可用容量为BLOCK_SIZE - BLOCK_HEADER
#define BLOCK_HEADER 80

#define RECORD_LENTH 300
/* 文件名命名规则:
        每个类型文件存放在单独一个文件(catalog, record, index)
            catalog文件：catalog_表名,
            record文件： record_表名，
            index文件:   index_key名_表名
 */


#endif /* definitions_h */
