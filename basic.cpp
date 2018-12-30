
#include <iostream>
#include <cstdio>

#include "basic.h"
#include "definitions.h"

int nextInt(const char *s,int &st){
	char ch=s[st]; 
	int f=1,x=0;
	while ((ch<'0'||ch>'9')&&st<BLOCK_SIZE){
		if (ch=='-') f=-1;
		ch=s[++st];
	}
	while (ch>='0'&&ch<='9'&&st<BLOCK_SIZE){
		x=x*10+ch-48;
		ch=s[++st];
	}
	return x*f;
}

float nextFloat(const char *s,int &st){
	char ch=s[st];
	float f=1,x=0,bt=10;
	bool fg=0;
	while ((ch<'0'||ch>'9')&&st<BLOCK_SIZE){
		if (ch=='-') f=-1;
		ch=s[++st];
	}
	while ((ch>='0'&&ch<='9'||ch=='.')&&st<BLOCK_SIZE){
		if (ch!='.'){
			if (!fg) x=x*10+ch-48; else{
				x=x+1.*(ch-48)/bt;
				bt/=10;
			}
		} else fg=1;
		ch=s[++st];
	}
	return x*f;	
}

std::string nextString(const char *s,int &st){
	char ch=s[st];
	std::string ret="";
	while (ch=='|') ch=s[++st];
	while (ch!='|'&&st<BLOCK_SIZE){
		ret+=ch;
		ch=s[++st];
	}
	return ret;
}

Tuple::Tuple(const Tuple &tuple_in){
    for(int index=0;index<tuple_in.data_.size();index++)
    {
        this->data_.push_back(tuple_in.data_[index]);
    }
}

inline int Tuple::getSize(){
    return (int)data_.size();
}

//新增数据
void Tuple::addData(Data data_in){
    this->data_.push_back(data_in);
}

bool Tuple::isDeleted() {
    return isDeleted_;
}

void Tuple::setDeleted() {
    isDeleted_ = true;
}

//得到元组中的数据
std::vector<Data> Tuple::getData() const{
    return this->data_;
}

void Tuple::showTuple(){
    for(int index=0;index<getSize();index++){
        if(data_[index].type==-1)
            std::cout<<data_[index].datai<<'\t';
        else if(data_[index].type==0)
            std::cout<<data_[index].dataf<<'\t';
        else
            std::cout<<data_[index].datas<<'\t';
    }
    std::cout<<std::endl;
}


Table::Table(std::string title,Attribute attr){
    this->title_=title;
    this->attr_=attr;
    this->index_.num=0;
}

Table::Table(std::string title,Attribute attr,Index index){
    this->title_=title;
    this->attr_=attr;
    this->index_=index;
}


//table的构造函数，拷贝用
Table::Table(const Table &table_in){
    this->attr_=table_in.attr_;
    this->index_=table_in.index_;
    this->title_=table_in.title_;
    for(int index=0;index<tuple_.size();index++)
        this->tuple_.push_back(table_in.tuple_[index]);
}

//插入索引
int Table::setIndex(short index,std::string index_name){
    short tmpIndex;
    for(tmpIndex=0;tmpIndex<index_.num;tmpIndex++){
        if(index==index_.location[tmpIndex])  //当该元素已经有索引时，报错
        {
            std::cout<<"Illegal Set Index: The index has been in the table."<<std::endl;
            return 0;
        }
    }
    for(tmpIndex=0;tmpIndex<index_.num;tmpIndex++){
        if(index_name==index_.indexname[tmpIndex])  //当该元素已经有索引时，报错
        {
            std::cout<<"Illegal Set Index: The name has been used."<<std::endl;
            return 0;
        }
    }
    index_.location[index_.num]=index;  //插入索引位置和索引名字，最后把索引个数加一
    index_.indexname[index_.num]=index_name;
    index_.num++;
    return 1;
}

int Table::dropIndex(std::string index_name){
    short tmpIndex;
    for(tmpIndex=0;tmpIndex<index_.num;tmpIndex++){
        if(index_name==index_.indexname[tmpIndex])  //当该元素已经有索引时，跳出
            break;
    }
    if(tmpIndex==index_.num)
    {
        std::cout<<"Illegal Drop Index: No such a index in the table."<<std::endl;
        return 0;
    }

    //交换最后的索引位置和名字，来达到删除的效果
    index_.indexname[tmpIndex]=index_.indexname[index_.num-1];
    index_.location[tmpIndex]=index_.location[index_.num-1];
    index_.num--;
    return 1;
}

std::string Table::getTitle(){
    return title_;
}
Attribute Table::getAttr(){
    return attr_;
}
std::vector<Tuple>& Table::getTuple(){
    return tuple_;
}
Index Table::getIndex(){
    return index_;
}

void Table::showTable(){
    for(int index=0;index<attr_.num;index++)
        std::cout<<attr_.name[index]<<'\t';
    std::cout<<std::endl;
    for(int index=0;index<tuple_.size();index++)
        tuple_[index].showTuple();
}

void Table::showTable(int limit) {
    for(int index=0;index<attr_.num;index++)
        std::cout<<attr_.name[index]<<'\t';
    std::cout<<std::endl;
    for(int index=0;index<limit&&index<tuple_.size();index++)
        tuple_[index].showTuple();
}

