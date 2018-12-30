
#include "interpreter.h"
#include "API.h"
#include <vector>
#include <fstream>
#include "exception.h"

using namespace std;

Interpreter::Interpreter() {}
Interpreter::~Interpreter() {}

void Interpreter::getInput()
{
    string temp_str;
    do {
        cout << "MiniSQL > ";
        getline(cin, temp_str);
        while (temp_str[0] == '#')
            getline(cin, temp_str);
        sql_string += temp_str;
        sql_string += ' ';
    } while(temp_str[temp_str.length() - 1] != ';');
    sql_string[sql_string.length() - 2] = '\0';
    dealWithString();
}

void Interpreter::parseInput()
{
    try {
        if (sql_string.substr(0, 6) == "select")
            selectFromTable();
        else if (sql_string.substr(0, 4) == "drop") {
            sql_string = changeToLowerCase(sql_string, 5);
            if (sql_string.substr(5, 5) == "table")
                dropTable();
            else if (sql_string.substr(5, 5) == "index")
                dropIndex();
        }
        else if (sql_string.substr(0, 6) == "insert")
            insertIntoTable();
        else if (sql_string.substr(0, 6) == "create") {
            sql_string = changeToLowerCase(sql_string, 7);
            if (sql_string.substr(7, 5) == "table")
                createTable();
            else if (sql_string.substr(7, 5) == "index")
                createIndex();
        }
        else if (sql_string.substr(0, 6) == "delete")
            deleteFromTable();
        else if (sql_string.substr(0, 4) == "desc")
            showCreateTable();
        else if (sql_string.substr(0, 4) == "quit" && sql_string[5] == '\0')
            exit();
        else if (sql_string.substr(0, 8) == "execfile")
            execFile();
        else if (sql_string.substr(0, 1) == "#")
            throw string("#");
        else
            throw string("Input format error!");
        
    } catch (string error) {
        if (error == "exit") {
            delete buffer_manager;
            std::exit(0);
        }
        else if (error[0] == '#')
            return;
        else {
            cout << error << endl;
        }
    } catch(table_exist error){
        std::cout<<"Error: Table has existed!"<<std::endl;
    }
    catch(table_not_exist error) {
        std::cout<<"Error: Table not exist!"<<std::endl;
    }
    catch(attribute_not_exist error) {
        std::cout<<"Error: Attribute not exist!"<<std::endl;
    }
    catch(index_exist error) {
        std::cout<<"Error: Index has existed!"<<std::endl;
    }
    catch(index_not_exist error) {
        std::cout<<"Error: Index not existed!"<<std::endl;
    }
    catch(tuple_type_conflict error) {
        std::cout<<"Error: Tuple type conflict!"<<std::endl;
    }
    catch(primary_key_conflict error) {
        std::cout<<"Error: Primary key conflict!"<<std::endl;
    }
    catch(data_type_conflict error) {
        std::cout<<"Error: data type conflict!"<<std::endl;
    }
    catch(index_full error) {
        std::cout<<"Error: Index full!"<<std::endl;
    }
    catch(unique_conflict error) {
        std::cout<<"Error: unique conflict!"<<std::endl;
    }
    catch (...) {
        cout << "ERROR...." << endl;
    }
}

void Interpreter::selectFromTable()
{
    
    catalogManager* catalog_manager = new catalogManager();
    
    string table_name;
    vector<string> attr_name;
    vector<int> target_index;
    vector<Where> where_select;
    string temp_target, temp_value;
    Where temp_where;
    string relations;
    Table result_table;
    
    //char op = 0;
    int current_pos;
    int flag = 0;
    
    if (getAWord(7, current_pos) == "*") {
        flag = 1;
        current_pos++;
    } else {
        current_pos = 7;
        while (1) {
            attr_name.push_back(getAWord(current_pos, current_pos));
            if (sql_string[++current_pos] != ',')
                break;
            else
                current_pos += 2;
        }
    }
    
    if (changeToLowerCase(sql_string, current_pos).substr(current_pos, 4) != "from")
        throw string("input error");
    current_pos += 5;
    table_name = getAWord(current_pos, current_pos);
    if (!catalog_manager->hasTable(table_name))
        throw string("table not exist!");
    Attribute temp_attr = catalog_manager->getAttribute(table_name);
    if (flag) {
        for (int i = 0; i < temp_attr.num; i++)
            attr_name.push_back(temp_attr.name[i]);
    } else {
        for (int i = 0; i < attr_name.size(); i++) {
            if (!catalog_manager->hasAttribute(table_name, attr_name[i]))
                throw string("attribute not exist!");
        }
    }
    current_pos++;
    if (sql_string[current_pos] == '\0') {
        API api;
        vector<int> target;
        vector<Where> where;
        result_table = api.selectRecord(table_name, target, where);
    } else {
        if (changeToLowerCase(sql_string, current_pos).substr(current_pos, 5) != "where")
            throw string("input format wrong!");
        current_pos += 6;
        while (1) {
            temp_target = getAWord(current_pos, current_pos);
            if (!catalog_manager->hasAttribute(table_name, temp_target))
                throw string("arribute not exist!");
            //target_name.push_back(temp_target);
            relations = getRelationMark(current_pos+1, current_pos);
            if (relations == "<")
                temp_where.relation_character = LESS;
            else if (relations == "< =")
                temp_where.relation_character = LESS_OR_EQUAL;
            else if (relations == "=")
                temp_where.relation_character = EQUAL;
            else if (relations == "> =")
                temp_where.relation_character = GREATER_OR_EQUAL;
            else if (relations == ">")
                temp_where.relation_character = GREATER;
            else if (relations == "! =")
                temp_where.relation_character = NOT_EQUAL;
            else
                throw string("input format error!");
            temp_value = getAWord(current_pos+1, current_pos);
            for (int i = 0; i < temp_attr.num; i++) {
                if (temp_target == temp_attr.name[i]) {
                    target_index.push_back(i);
                    temp_where.data.type = temp_attr.type[i];
                    switch (temp_where.data.type) {
                        case -1:
                            try {
                                temp_where.data.datai = stringToNum<int>(temp_value);
                            } catch (...) {
                                throw string("data conversion error!");
                            }
                            break;
                        case 0:
                            try {
                                temp_where.data.dataf = stringToNum<float>(temp_value);
                            } catch (...) {
                                throw string("data conversion error!");
                            }
                            break;
                        default:
                            try {
                                while (!(temp_value[0] == '\'' && temp_value[temp_value.length() - 1] == '\'') && !(temp_value[0] == '"' && temp_value[temp_value.length() - 1] == '"')) {
                                    temp_value += " " + getAWord(current_pos+1, current_pos);
                                }
                                    temp_where.data.datas = temp_value.substr(1, temp_value.length() - 2);
                            }
                            catch (...) {
                                throw string("input format error!");
                            }
                            break;
                    }
                    
                    break;
                }
            }
            
            where_select.push_back(temp_where);
 
            if (sql_string[current_pos+1] == '\0')
                break;
            else if (changeToLowerCase(sql_string, current_pos+1).substr(current_pos+1, 3) != "and")
                throw string("Input format error!");
            getAWord(current_pos+1, current_pos);
            current_pos++;
        }
        
        API api;
        result_table = api.selectRecord(table_name, target_index, where_select);
    }
    
    Attribute attr_record = result_table.attr_;
    int use[32] = {0};
    if (attr_name.size() == 0) {
        for (int i = 0; i < attr_record.num; i++)
            use[i] = i;
    } else {
        for (int i = 0; i < attr_name.size(); i++) {
            for (int j = 0; j < attr_record.num; j++) {
                if (attr_record.name[j] == attr_name[i]) {
                    use[i] = j;
                    break;
                }
            }
        }
    }
    vector<Tuple> result_tuple = result_table.getTuple();
    int longest = -1;
    for (int i = 0; i < attr_name.size(); i++) {
        if ((int)attr_record.name[use[i]].length() > longest)
            longest = (int)attr_record.name[use[i]].length();
    }
    for(int i = 0; i < attr_name.size(); i++) {
        int type = attr_record.type[use[i]];
        if (type == -1) {
            for (int j = 0; j < result_tuple.size(); j++) {
                if (longest < getBits(result_tuple[j].getData()[use[i]].datai)) {
                    longest = getBits(result_tuple[i].getData()[use[i]].datai);
                }
            }
        }
        if (type == 0) {
            for (int j = 0; j < result_tuple.size(); j++) {
                if (longest < getBits(result_tuple[j].getData()[use[i]].dataf)) {
                    longest =  getBits(result_tuple[j].getData()[use[i]].dataf);
                }
            }
        }
        if (type > 0) {
            for (int j = 0; j < result_tuple.size(); j++) {
                if (longest < result_tuple[j].getData()[use[i]].datas.length()) {
                    longest = (int)result_tuple[j].getData()[use[i]].datas.length();
                }
            }
        }
    }
    longest += 1;
    for (int i = 0; i < attr_name.size(); i++) {
        if (i != attr_name.size() - 1) {
            for (int j = 0; j < (longest - attr_record.name[use[i]].length()) / 2; j++)
                printf(" ");
            printf("%s", attr_record.name[use[i]].c_str());
            for (int j = 0; j < longest - (longest-attr_record.name[use[i]].length()) / 2 - attr_record.name[use[i]].length(); j++)
                printf(" ");
            printf("|");
        } else{
            for (int j = 0; j < (longest - attr_record.name[use[i]].length()) / 2; j++)
                printf(" ");
            printf("%s", attr_record.name[use[i]].c_str());
            for (int j = 0; j < longest - (longest - attr_record.name[use[i]].length()) / 2 - attr_record.name[use[i]].length(); j++)
                printf(" ");
            printf("\n");
        }
    }
    for (int i = 0; i < attr_name.size() * (longest + 1); i++) {
        printf("-");
    }
    printf("\n");
    for (int index=0; index < result_tuple.size(); index++) {
        for(int j = 0; j < attr_name.size(); j++) {
            switch (result_tuple[index].getData()[use[j]].type) {
                case -1:
                    if (j != attr_name.size() - 1) {
                        int len = result_tuple[index].getData()[use[j]].datai;
                        len = getBits(len);
                        for (int k = 0; k < (longest - len) / 2; k++)
                            printf(" ");
                        printf("%d", result_tuple[index].getData()[use[j]].datai);
                        for (int k = 0; k < longest - (longest - len) / 2 - len;k++)
                            printf(" ");
                        printf("|");
                    } else {
                        int len = result_tuple[index].getData()[use[j]].datai;
                        len = getBits(len);
                        for (int k = 0; k < (longest - len) / 2; k++)
                            printf(" ");
                        printf("%d", result_tuple[index].getData()[use[j]].datai);
                        for (int k = 0; k < longest - (longest - len) / 2 - len; k++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                case 0:
                    if (j != attr_name.size() - 1) {
                        float num = result_tuple[index].getData()[use[j]].dataf;
                        int len = getBits(num);
                        for (int k = 0; k < (longest - len) / 2; k++)
                            printf(" ");
                        printf("%.2f", result_tuple[index].getData()[use[j]].dataf);
                        for (int k = 0; k < longest - (longest - len) / 2 - len; k++)
                            printf(" ");
                        printf("|");
                    } else {
                        float num = result_tuple[index].getData()[use[j]].dataf;
                        int len = getBits(num);
                        for (int k = 0; k < (longest - len) / 2; k++)
                            printf(" ");
                        printf("%.2f", result_tuple[index].getData()[use[j]].dataf);
                        for (int k = 0; k < longest - (longest - len) / 2 - len; k++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
                default:
                    string tmp = result_tuple[index].getData()[use[j]].datas;
                    if (j != attr_name.size() - 1) {
                        for (int i = 0; i < (longest - tmp.length()) / 2; i++)
                            printf(" ");
                        printf("%s", tmp.c_str());
                        for (int i = 0; i < longest - (longest - (int)tmp.length()) / 2 - (int)tmp.length(); i++)
                            printf(" ");
                        printf("|");
                    }
                    else{
                        string tmp = result_tuple[index].getData()[j].datas;
                        for (int i = 0; i < (longest - tmp.length()) / 2; i++)
                            printf(" ");
                        printf("%s", tmp.c_str());
                        for (int i = 0; i < longest - (longest - (int)tmp.length()) / 2 - (int)tmp.length(); i++)
                            printf(" ");
                        printf("\n");
                    }
                    break;
            }
        }
    }
}

void Interpreter::dropTable()
{
    string table_name;
    int current_pos;
    table_name = getAWord(11, current_pos);
    if (sql_string[current_pos+1] != '\0')
        throw string("input format error!");
    
    API api;
    api.dropTable(table_name);
    cout << "MiniSQL > Drop table " << table_name << " SUCCESS!!" << endl;
}

void Interpreter::dropIndex()
{
    string table_name;
    string index_name;
    int current_pos;
    index_name = getAWord(11, current_pos);
    current_pos++;
    if (changeToLowerCase(sql_string, current_pos).substr(current_pos, 2) != "on")
        throw string("input format error!");
    table_name = getAWord(current_pos+3, current_pos);
    if (sql_string[current_pos+1] != '\0')
        throw string("input format error!");
    
    API api;
    api.dropIndex(table_name, index_name);
    cout << "MiniSQL > Drop index " << index_name << " on table " << table_name << " SUCCESS!!" << endl;
}

void Interpreter::createTable()
{
    string table_name;
    int current_pos;
    Index new_index;
    Attribute new_attr;
    string new_attr_name;
    
    table_name = getAWord(13, current_pos);
    new_index.num = 0;
    int primary_key = -1;
    int attr_num = 0;
    
    catalogManager cm;
    if (cm.hasTable(table_name)) {
        throw string("Table has existed!");
    }
    
    while (1) {
        current_pos += 3;
        if (sql_string[current_pos] == '\0') {
            if (sql_string[current_pos - 2] == '\0')
                throw string("Input format error!");
            else
                break;
        }
        
        new_attr_name = getAWord(current_pos, current_pos);
        string temp_check = new_attr_name;
        temp_check = changeToLowerCase(temp_check, 0);
        if (temp_check == "primary") {
            int temp_end = current_pos;
            string temp_key = getAWord(temp_end+1, temp_end);
            if (temp_key != "key") {
                new_attr.name[attr_num] = new_attr_name;
                break;
            } else {
                current_pos = temp_end + 3;
                string unique_name = getAWord(current_pos, current_pos);
                bool is_set = true;
                for (int i = 0; i < new_attr.num; i++) {
                    if (new_attr.name[i] == unique_name) {
                        new_attr.has_index[i] = true;
                        new_index.location[new_index.num] = i;
                        new_index.indexname[new_index.num] = unique_name;
                        new_index.num++;
                        is_set = false;
                        primary_key = i;
                        new_attr.unique[i] = true;
                        current_pos += 2;
                        break;
                    }
                }
                if (is_set) {
                    throw string("primary key set error!");
                }
                continue;
            }
        } else {
            new_attr.name[attr_num] = new_attr_name;
        }
        current_pos++;
        new_attr.type[attr_num] = getType(current_pos, current_pos);
        new_attr.unique[attr_num] = false;
        if (sql_string[current_pos+1] == 'u' || sql_string[current_pos+1] == 'U') {
            sql_string = changeToLowerCase(sql_string, 0);
            if (getAWord(current_pos+1, current_pos) == "unique") {
                new_attr.unique[attr_num] = true;
            }
            else
                throw string("input format error!");
        }
        attr_num++;
        new_attr.num = attr_num;
    }
    
    API api;
    api.createTable(table_name, new_attr, primary_key, new_index);
    cout << "MiniSQL > Create table " << table_name << " SUCCESS!!" << endl;
}

void Interpreter::createIndex()
{
    catalogManager* catalog_manager = new catalogManager();
    
    string table_name, index_name, attr_name;
    int current_pos;
    
    index_name = getAWord(13, current_pos);
    current_pos++;
    
    if (changeToLowerCase(sql_string, current_pos).substr(current_pos, 2) != "on")
        throw string("input format error!");
    table_name = getAWord(current_pos+3, current_pos);
    if (!catalog_manager->hasTable(table_name))
        throw string("Table not exist!");
    if (sql_string[current_pos+1] != '(')
        throw string("input format error!");
    attr_name = getAWord(current_pos+3, current_pos);
    if (sql_string[current_pos+1] != ')' || sql_string[current_pos+3] != '\0')
        throw string("input format error!");
    
    API api;
    api.createIndex(table_name, index_name, attr_name);
    cout << "MiniSQL > Create index " << index_name << " on table " << table_name << "(" << attr_name << ")" << " SUCCESS!!" << endl;
}

void Interpreter::insertIntoTable()
{
    catalogManager* catalog_manager = new catalogManager();
    
    string table_name;
    int current_pos;
    Tuple new_tuple;
    Attribute insert_attr;
    
    if (changeToLowerCase(sql_string, 7).substr(7, 4) != "into")
        throw string("Input format error!");
    table_name = getAWord(12, current_pos);
    if (changeToLowerCase(sql_string, current_pos+1).substr(current_pos+1, 6) != "values")
        throw string("Input format error!");
    current_pos += 8;
    if (sql_string[current_pos] != '(')
        throw string("Input format error!");
    if (!catalog_manager->hasTable(table_name))
        throw string("Table not exist!");
    insert_attr = catalog_manager->getAttribute(table_name);
    current_pos--;
    int insert_num = 0;
    
    while (sql_string[current_pos+1] != '\0' && sql_string[current_pos+1] != ')') {
        if (insert_num >= insert_attr.num)
            throw string("Attributes num not match!");
        current_pos += 3;
        string insert_content = getAWord(current_pos, current_pos);
        Data insert_data;
        insert_data.type = insert_attr.type[insert_num];
        switch (insert_data.type) {
            case -1:
                try {
                    insert_data.datai = stringToNum<int>(insert_content);
                } catch (...) {
                    throw string("data conversion error!");
                }
                break;
            case 0:
                try {
                    insert_data.dataf = stringToNum<float>(insert_content);
                } catch (...) {
                    throw string("data conversion error!");
                }
                break;
            default:
                try {
                    while (!(insert_content[0] == '\'' && insert_content[insert_content.length() - 1] == '\'')  && !(insert_content[0] == '\"' && insert_content[insert_content.length() - 1] == '\"')) {
                        insert_content += " " + getAWord(current_pos+1, current_pos);
                    }
                        //throw string("data conversion error!");
                    if (insert_content.length() - 1 > insert_attr.type[insert_num])
                        throw string("data length over the limit!");
                    insert_data.datas = insert_content.substr(1, insert_content.length() - 2);
                } catch(string content) {
                    throw content;
                }
                break;
        }
        new_tuple.addData(insert_data);
        insert_num++;
    }
    
    if (sql_string[current_pos+1] == '\0')
        throw string("input format error!");
    if (insert_num != insert_attr.num)
        throw string("Attribute num error!");
    
    API api;
    api.insertRecord(table_name, new_tuple);
    cout << "MiniSQL > Insert into " << table_name << " SUCCESS!!" << endl;
}

void Interpreter::deleteFromTable()
{
    catalogManager* catalog_manager = new catalogManager();
    
    vector<int> target_attr;
    vector<Where> where_delete;
    
    Where delete_where;
    int current_pos;
    string table_name;
    string attr_name;
    string relations;
    
    if (changeToLowerCase(sql_string, 7).substr(7, 4) != "from")
        throw string("Input format error!");
    table_name = getAWord(12, current_pos);
    if (!catalog_manager->hasTable(table_name))
        throw string("table not exist!");
    
    if (sql_string[current_pos+1] == '\0') {
        attr_name = "";
        API api;
        api.deleteRecordAll(table_name);
        cout << "MiniSQL > Delete all records from " << table_name << " SUCCESS!!" << endl;
        return;
    }
    
    if (changeToLowerCase(sql_string, current_pos+1).substr(current_pos+1, 5) != "where")
        throw string("input format error!");
    current_pos += 7;
    while(1) {
        attr_name = getAWord(current_pos, current_pos);
        if (!catalog_manager->hasAttribute(table_name, attr_name))
            throw string("Attribute not exist!");
        relations = getAWord(current_pos+1, current_pos);
        if (relations == "<")
            delete_where.relation_character = LESS;
        else if (relations == "< =")
            delete_where.relation_character = LESS_OR_EQUAL;
        else if (relations == "=")
            delete_where.relation_character = EQUAL;
        else if (relations == "> =")
            delete_where.relation_character = GREATER_OR_EQUAL;
        else if (relations == ">")
            delete_where.relation_character = GREATER;
        else if (relations == "! =")
            delete_where.relation_character = NOT_EQUAL;
        else
            throw string("Input format error!");
        
        string delete_value = getAWord(current_pos+1, current_pos);
        
        Attribute temp_attr = catalog_manager->getAttribute(table_name);
        for (int i = 0; i < temp_attr.num; i++) {
            if (attr_name == temp_attr.name[i]) {
                target_attr.push_back(i);
                delete_where.data.type = temp_attr.type[i];
                switch (delete_where.data.type) {
                    case -1:
                        try {
                            delete_where.data.datai = stringToNum<int>(delete_value);
                        } catch (...) {
                            throw string("data conversion error!");
                        }
                        break;
                    case 0:
                        try {
                            delete_where.data.dataf = stringToNum<float>(delete_value);
                        } catch (...) {
                            throw string("data conversion error!");
                        }
                    default:
                        try {
                            while (!(delete_value[0] == '\'' && delete_value[delete_value.length() - 1] == '\'') && !(delete_value[0] == '"' && delete_value[delete_value.length() - 1] == '"'))
                            {
                                delete_value += " " + getAWord(current_pos+1, current_pos);
                            }
                                //throw string("Input format error!");
                            delete_where.data.datas = delete_value.substr(1, delete_value.length() - 2);
                        } catch(...) {
                            throw string("Input format error!");
                        }
                        break;
                }
                break;
            }
        }
        where_delete.push_back(delete_where);
        
        if (sql_string[current_pos+1] == '\0')
            break;
        else if (changeToLowerCase(sql_string, current_pos+1).substr(current_pos+1, 3) != "and")
            throw string("Input format error!");
        getAWord(current_pos+1, current_pos);
        current_pos++;
        
    }
    
    API api;
    api.deleteRecord(table_name, target_attr, where_delete);
    cout << "MiniSQL > Delete records from " << table_name << " SUCCESS!!" << endl;
}

void Interpreter::showCreateTable()
{
    catalogManager* catalog_manager = new catalogManager();
    
    string table_name;
    int current_pos;
    
    getAWord(0, current_pos);
    table_name = getAWord(current_pos+1, current_pos);
    
    if (sql_string[current_pos+1] != '\0')
        throw string("input format error!");
    catalog_manager->showTable(table_name);
}

void Interpreter::execFile()
{
    int current_pos = 0;
    int begin = 0;
    string file_path = getAWord(9, current_pos);
    string temp;
    
    if (sql_string[current_pos+1] != '\0')
        throw string("Input format error!");
    string::iterator it;
    fstream fs(file_path);
    stringstream ss;
    ss << fs.rdbuf();
    temp = ss.str();
    
    current_pos = 0;
    
    do {
        if (temp[current_pos] == '#') {
            while (temp[current_pos] != '\n') {
                current_pos++;
            }
            current_pos++;
            begin = current_pos;
        }
        while (temp[current_pos] != ';' && temp[current_pos] != '\0')
            current_pos++;
        if (temp[current_pos] == '\0')
            break;
        sql_string = temp.substr(begin, current_pos - begin);
        current_pos++;
        begin = current_pos;
        sql_string += '\0';
        sql_string += ' ';
        //sql_string[sql_string.length() - 2] = '\0';
        dealWithString();
        cout << "MiniSQL > EXECUTE FROM FILE : " << sql_string << endl;
        parseInput();
    } while (temp[current_pos] != '\0');
    
}

void Interpreter::exit()
{
    throw string("exit");
}

void Interpreter::dealWithString()
{
    for (int i = 0; i < sql_string.length(); i++) {
        if (sql_string[i] == '*' || sql_string[i] == '=' || sql_string[i] == ',' || sql_string[i] == '(' || sql_string[i] == ')' || sql_string[i] == '>' || sql_string[i] == '<') {
            if (sql_string[i - 1] != ' ')
                sql_string.insert(i++, " ");
            if (sql_string[i + 1] != ' ')
                sql_string.insert(++i, " ");
        }
    }
    
    sql_string.insert(sql_string.length() - 2, " ");
    
    string::iterator it;
    bool flag = false;
    for (string::iterator it = sql_string.begin(); it < sql_string.end();) {
        if (!flag && (*it == ' ' || *it == '\t')) {
            flag = true;
            it++;
            continue;
        }
        if (flag && (*it == ' ' || *it == '\t')) {
            sql_string.erase(it);
            if (it != sql_string.begin())
                it--;
            it++;
            continue;
        }
        if (*it != ' ' && *it != '\t') {
            if (*it == '\r' || *it == '\n') {
                sql_string.erase(it);
                if (it != sql_string.begin())
                    it--;
            } else
                it++;
            flag = false;
            continue;
        }
    }
    
    if (sql_string[0] == ' ')
        sql_string.erase(sql_string.begin());
    sql_string = changeToLowerCase(sql_string, 0);
}

int Interpreter::getType(int pos, int &end_pos)
{
    string type = getAWord(pos, end_pos);
    if (type == "int")
        return -1;
    else if (type == "float")
        return 0;
    else if (type == "char") {
        end_pos += 3;
        string length = getAWord(end_pos, end_pos);
        end_pos += 2;
        return atoi(length.c_str()) + 1;
    }
    throw string("Input format error!");
}

int Interpreter::getBits(int int_num)
{
    int bit = 0;
    if (int_num == 0)
        return 1;
    if (int_num < 0) {
        bit++;
        int_num = -int_num;
    }
    while (int_num != 0) {
        int_num /= 10;
        bit++;
    }
    return bit;
}

int Interpreter::getBits(float float_num)
{
    int bit = 0;
    if ((int)float_num == 0)
        return 4;
    if (float_num < 0) {
        bit++;
        float_num = -float_num;
    }
    int int_part = float_num;
    while (int_part != 0) {
        int_part /= 10;
        bit++;
    }
    return bit + 3;
}

string Interpreter::getAWord(int pos, int &end_pos)
{
    string word = "";
    for (int i = pos; i < sql_string.length(); i++) {
        if (sql_string[i] == ' ' || sql_string[i] == '\0') {
            word = sql_string.substr(pos, i-pos);
            end_pos = i;
            return word;
        }
    }
    return word;
}

string Interpreter::changeToLowerCase(string str, int pos)
{
    for(int i = pos; ;i++){
        if(str[i] == ' ' || str[i] == '\0')
            break;
        else if(str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 32;
    }
    return str;
}

string Interpreter::getRelationMark(int pos, int &end_pos)
{
    string mark = "";
    for (int i = pos; i < sql_string.length(); i++) {
        if (sql_string[i] == ' ')
            continue;
        if (sql_string[i] != '<' && sql_string[i] != '>' && sql_string[i] != '=' && sql_string[i] != '!') {
            mark = sql_string.substr(pos, i - pos - 1);
            end_pos = i - 1;
            return mark;
        }
    }
    return mark;
}
