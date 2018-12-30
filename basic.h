
#ifndef _BASIC_H_
#define _BASIC_H_ 1
#include <iostream>
#include <vector>


typedef enum{
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    GREATER_OR_EQUAL,
    GREATER,
    NOT_EQUAL
} WHERE;


int nextInt(const char *s,int &st);
float nextFloat(const char *s,int &st);
std::string nextString(const char *s,int &st);

struct Data{
    int type;
    int datai;  //-1
    float dataf;//0
    std::string datas;//1-255
    bool operator<(const Data v) const{
    	if (type==-1) return datai<v.datai;
    	if (type==0) return dataf<v.dataf;
    	return datas<v.datas;
    }
    bool operator==(const Data v)const {
		if (type==-1) return datai==v.datai;
    	if (type==0) return dataf==v.dataf;
    	return datas==v.datas;
    }
    bool operator<=(const Data v)const {
		if (type==-1) return datai<=v.datai;
    	if (type==0) return dataf<=v.dataf;
    	return datas<=v.datas;
    }
    bool operator>(const Data v)const {
    	if (type==-1) return datai>v.datai;
    	if (type==0) return dataf>v.dataf;
    	return datas>v.datas;
    }
    Data(){
    	type=datai=dataf=0;
    	datas="";
    }
};

struct Data_with_id{
	Data x;
	int id;
	bool operator<(const Data_with_id x_)const {return x<x_.x;}
};

struct Where{
    Data data; 
    WHERE relation_character; 
};

struct Attribute{
    int num = 0;  
    std::string name[32] = {""};
    short type[32] = {-2};
    bool unique[32] = {false};
    short primary_key = -1;
    bool has_index[32] = {false};
};

struct Index{
    int num;  
    short location[10];
    std::string indexname[10];  
};

class Tuple{
private:
    bool isDeleted_;
public:
	std::vector<Data> data_;  
    Tuple() : isDeleted_(false) {};
    Tuple(const Tuple &tuple_in); 
    void addData(Data data_in); 
    std::vector<Data> getData() const;  
    int getSize();  
    bool isDeleted();
    void setDeleted();
    void showTuple();  
};

class Table{
public:
	std::string title_;
    std::vector<Tuple> tuple_; 
    Index index_;  
    Attribute attr_; 
    Table(){};
    Table(std::string title,Attribute attr);
    Table(std::string title,Attribute attr,Index index);
    Table(const Table &table_in);
    int setIndex(short index,std::string index_name);  //插入索引，输入要建立索引的Attribute的编号，以及索引的名字，成功返回1失败返回0
    int dropIndex(std::string index_name);  //删除索引，输入建立的索引的名字，成功返回1失败返回0

    std::string getTitle();
    Attribute getAttr();
    std::vector<Tuple>& getTuple();
    Index getIndex();
    //short gethasKey();

    void showTable(); //显示table的部分数据
    void showTable(int limit);
};




#endif
