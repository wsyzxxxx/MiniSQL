#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <iomanip>

#include "bufferManager.h"
#include "basic.h"
#include "exception.h"
#include "catalogManager.h"

void catalogManager::createTable(std::string name, Attribute Attr, int primary, Index index){
	string path="catalog_"+name;
	delete buffer_manager;
	buffer_manager=new bufferManager();
	remove(path.c_str());
	buffer_manager->createNewFile(path,2,2);
	blockInfo* block=buffer_manager->getBlockMem(path,0);
	char *s=block->getBlockContent(); s[0]='\0';
	sprintf(s,"%d|%d|",Attr.num,primary);
	for (int i=0;i<Attr.num;i++)
		sprintf(s+strlen(s),"%s|%d|%d|%d|",Attr.name[i].c_str(),Attr.type[i],Attr.unique[i],Attr.has_index[i]);
	block->setDirty();
	
	path="catalog_"+name+"_index";
	delete buffer_manager;
	buffer_manager=new bufferManager();
	remove(path.c_str());
	buffer_manager->createNewFile(path,2,2);
	block=buffer_manager->getBlockMem(path,0);
	s=block->getBlockContent(); s[0]='\0';
	sprintf(s,"%d|",index.num);
    for (int i=0;i<index.num;i++) {
        sprintf(s+strlen(s),"%d|%s|",index.location[i],index.indexname[i].c_str());
        buffer_manager->createNewFile("index_" + index.indexname[i], 2, 0);
    }
	block->setDirty();
}

void catalogManager::dropTable(std::string name){
    if(!hasTable(name)) throw table_not_exist();
    
    std::string path="catalog_"+name;
    delete buffer_manager;
	buffer_manager=new bufferManager();
	remove(path.c_str());
	
	path="catalog_"+name+"_index";
	delete buffer_manager;
	buffer_manager=new bufferManager();
	remove(path.c_str());
}

Attribute catalogManager::getAttribute(std::string name){
    if(!hasTable(name)) throw table_not_exist();
	string path="catalog_"+name;
	blockInfo* block=buffer_manager->getBlockMem(path,0);
	char *s=block->getBlockContent();
	int st=0;
	Attribute res;
	res.num=nextInt(s,st);
	res.primary_key=nextInt(s,st);
	for (int i=0;i<res.num;i++){
		res.name[i]=nextString(s,st);
		res.type[i]=nextInt(s,st);
		res.unique[i]=nextInt(s,st);
		res.has_index[i]=nextInt(s,st); 
	}
    return res;
}

bool catalogManager::hasAttribute(std::string table_name , std::string attr_name){
    if(!hasTable(table_name)) throw table_not_exist();
    Attribute find_attr=getAttribute(table_name);
    for(int index=0;index<find_attr.num;index++){
        if(attr_name==find_attr.name[index])
            return 1;
    }
    return 0;
}

std::string catalogManager::IndextoAttr(std::string table_name, std::string index_name){
    if(!hasTable(table_name)) throw table_not_exist();
    Index index_record=getIndex(table_name);
    int hasfind=-1;
    for(int i=0;i<index_record.num;i++){
        if(index_record.indexname[i]==index_name){
            hasfind=i;
            break;
        }
    }
    if(hasfind==-1)
        throw index_not_exist();
    Attribute attr_record=getAttribute(table_name);
    return attr_record.name[index_record.location[hasfind]];
}

void catalogManager::createIndex(std::string table_name,std::string attr_name,std::string index_name){
    if(!hasTable(table_name))
        throw table_not_exist();
    if(!hasAttribute(table_name, attr_name))
        throw attribute_not_exist();
    Index index_record=getIndex(table_name);
    if(index_record.num>=10)
        throw index_full();
    Attribute find_attr=getAttribute(table_name);
    for(int i=0;i<index_record.num;i++){
        if(index_record.indexname[i]==index_name)
            throw index_exist();
        if(find_attr.name[index_record.location[i]]==attr_name)
            throw index_exist();
    }
    index_record.indexname[index_record.num]=index_name;
    for(int index=0;index<find_attr.num;index++){
        if(attr_name==find_attr.name[index])
        {
            index_record.location[index_record.num]=index;
            find_attr.has_index[index]=1;
            break;
        }
    }
    index_record.num++;
    dropTable(table_name);
    createTable(table_name, find_attr, find_attr.primary_key, index_record);
}

void catalogManager::dropIndex(std::string table_name,std::string index_name){
    if(!hasTable(table_name)){
        throw table_not_exist();
    }
    Index index_record=getIndex(table_name);
    Attribute attr_record=getAttribute(table_name);
    int hasindex=-1;
    for(int index=0;index<index_record.num;index++){
        if(index_record.indexname[index]==index_name){
            hasindex=index;
            break;
        }
    }
    if(hasindex==-1){
        throw index_not_exist();
    }
    attr_record.has_index[index_record.location[hasindex]]=0;
    index_record.indexname[hasindex]=index_record.indexname[index_record.num-1];
    index_record.location[hasindex]=index_record.location[index_record.num-1];
    index_record.num--;
    dropTable(table_name);
    createTable(table_name, attr_record, attr_record.primary_key, index_record);
}

void catalogManager::showTable(std::string table_name){
    //如果不存在该表，则异常
    if(!hasTable(table_name)){
        throw table_not_exist();
    }
    //打印表的名字
    std::cout<<"Table name:"<<table_name<<std::endl;
    Attribute attr_record=getAttribute(table_name);
    Index index_record=getIndex(table_name);
    //寻找最长的index_name的信息，在之后的打印表格中会用到
    int longest=-1;
    for(int index=0;index<attr_record.num;index++){
        if((int)attr_record.name[index].length()>longest)
            longest=(int)attr_record.name[index].length();
    }
    //打印属性
    std::string type;
    std::cout<<"Attribute:"<<std::endl;
    std::cout<<"Num|"<<"Name"<<std::setw(longest+2)<<"|Type"<<type<<std::setw(6)<<"|"<<"Unique|Primary Key"<<std::endl;
    for(int index_out=0;index_out<longest+35;index_out++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int index=0;index<attr_record.num;index++){
        switch (attr_record.type[index]) {
            case -1:
                type="int";
                break;
            case 0:
                type="float";
                break;
            default:
                type="char("+num2str(attr_record.type[index]-1, 3)+")";
                break;
        }
        std::cout<<index<<std::setw(3-index/10)<<"|"<<attr_record.name[index]<<std::setw(longest-(int)attr_record.name[index].length()+2)<<"|"<<type<<std::setw(10-(int)type.length())<<"|";
        if(attr_record.unique[index])
            std::cout<<"unique"<<"|";
        else
            std::cout<<std::setw(7)<<"|";
        if(attr_record.primary_key==index)
            std::cout<<"primary key";
        std::cout<<std::endl;
    }
    
    for(int index_out=0;index_out<longest+35;index_out++)
        std::cout<<"-";
    
    std::cout<<std::endl;
    
    //打印索引
    std::cout<<"Index:"<<std::endl;
    std::cout<<"Num|Location|Name"<<std::endl;
    longest=-1;
    for(int index_out=0;index_out<index_record.num;index_out++){
        if((int)index_record.indexname[index_out].length()>longest)
            longest=(int)index_record.indexname[index_out].length();
    }
    for(int index_out=0;index_out<((longest+14)>18?(longest+14):18);index_out++)
        std::cout<<"-";
    std::cout<<std::endl;
    for(int index_out=0;index_out<index_record.num;index_out++){
        std::cout<<index_out<<std::setw(3-index_out/10)<<"|"<<index_record.location[index_out]<<std::setw(8-index_record.location[index_out]/10)<<"|"<<index_record.indexname[index_out]<<std::endl;
    }
    for(int index_out=0;index_out<((longest+14)>18?(longest+14):18);index_out++)
        std::cout<<"-";
    std::cout<<std::endl<<std::endl;
}

//判断是否已有重名的表格
bool catalogManager::hasTable(std::string table_name){
	string path="catalog_"+table_name;
    fstream fs;
    fs.open(path);
    if (fs) {
        fs.close();
        return 1;
    } else {
        fs.close();
        return 0;
    }
    return 1;
}

Index catalogManager::getIndex(std::string table_name){
	string path="catalog_"+table_name+"_index";
	blockInfo* block=buffer_manager->getBlockMem(path,0);
	char *s=block->getBlockContent(); 
	int st=0;
	Index res;
	res.num=nextInt(s,st);
	for (int i=0;i<res.num;i++){
		res.location[i]=nextInt(s,st);
		res.indexname[i]=nextString(s,st);
	}
	return res;
}

std::string catalogManager::num2str(int num,short bit){
    std::string str="";
    if(num<0){
        num=-num;
        str+="-";
    }
    int divisor=pow(10,bit-1);
    for(int i=0;i<bit;i++){
        str+=(num/divisor%10+'0');
        divisor/=10;
    }
    return str;
}

int catalogManager::str2num(std::string str){
    return atoi(str.c_str());
}

