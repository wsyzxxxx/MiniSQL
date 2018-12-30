
#include "API.h"
#include "RecordManager.h"
#include "indexManager.h"
#include "basic.h"
#include <vector>

using namespace std;

Table API::selectRecord(string table_name, vector<int> target_attr, vector<Where> where)
{
    RecordManager* record_manager = new RecordManager;
    catalogManager* catalog_manager = new catalogManager();
    
    Attribute attr=catalog_manager->getAttribute(table_name);
    vector<int> as; as.clear();
    for (int i=0;i<attr.num;i++) as.push_back(i); 
    Index index=catalog_manager->getIndex(table_name);
    Table table(table_name,attr,index);
    return record_manager->Select(table, as, target_attr, where);
}

int API::deleteRecord(string table_name, vector<int> target_attr , vector<Where> where)
{

    RecordManager* record_manager = new RecordManager;
    catalogManager* catalog_manager = new catalogManager();
    
    Attribute attr=catalog_manager->getAttribute(table_name);
    Index index=catalog_manager->getIndex(table_name);
    Table table(table_name,attr,index);
    return record_manager->Delete(table,target_attr,where);
}

void API::deleteRecordAll(string table_name){
	catalogManager cat;
	RecordManager rm;
	indexManager im;
	Attribute attr=cat.getAttribute(table_name);
	Index index=cat.getIndex(table_name);
	int primary=attr.primary_key;
	cat.dropTable(table_name);
	Table table(table_name,attr,index);
	rm.DropTable(table);
	for (int i=0;i<index.num;i++){
		im.DropIndex(index.indexname[i].c_str());
		vector<Data>v; v.clear();
		vector<int>v_id; v_id.clear();
		im.CreateIndex(index.indexname[i].c_str(),v,v_id,attr.type[index.location[i]]);
	}
	createTable(table_name,attr,primary,index);
}

void API::insertRecord(string table_name, Tuple& tuple){
	catalogManager cm;
	RecordManager rm;
	Attribute attr=cm.getAttribute(table_name);
	Index index=cm.getIndex(table_name);
	Table table(table_name,attr,index);
	rm.Insert(table,tuple);
}

void API::createTable(string table_name, Attribute attribute, int primary, Index index){
	catalogManager cm;
	RecordManager rm;
    indexManager im;
	Table table(table_name,attribute);
	rm.CreateTable(table);
	cm.createTable(table_name,attribute,primary,index);
    for (int i = 0; i < index.num; i++)
        im.CreateIndex(index.indexname[i].c_str(), vector<Data>(), vector<int>(), attribute.type[index.location[i]]);
}

void API::dropTable(string table_name){
	catalogManager cm;
	RecordManager rm;
	indexManager im;
	Attribute attr=cm.getAttribute(table_name);
	Index index=cm.getIndex(table_name);
	Table table(table_name,attr,index);
	rm.DropTable(table);
	cm.dropTable(table_name);
	for (int i=0;i<index.num;i++) im.DropIndex(index.indexname[i].c_str());
	
}

void API::createIndex(string table_name,string index_name, string attr_name){

	RecordManager rm;
	catalogManager cm;
	Attribute attr=cm.getAttribute(table_name);
	
	Table table(table_name,attr);
	
	rm.CreateIndex(table,index_name.c_str(),attr_name.c_str());
	cm.createIndex(table_name,attr_name,index_name);
}

void API::dropIndex(string table_name, string index_name){
	indexManager im;
	catalogManager cm;
	cm.dropIndex(table_name,index_name);
	im.DropIndex(index_name.c_str());

}
