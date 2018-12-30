
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <iostream>
#include <string>
#include <sstream>
#include "basic.h"
#include "catalogManager.h"

using namespace std;

class Interpreter {
public:
    Interpreter();
    ~Interpreter();
    void getInput();    //读取输入的字符
    void parseInput();  //第一步处理输入
    void selectFromTable();     //处理select操作
    void dropTable();           //处理drop table操作
    void dropIndex();           //处理drop index的操作
    void createTable();         //处理create table操作
    void createIndex();         //处理create index的操作
    void insertIntoTable();     //处理insert Table的操作
    void deleteFromTable();     //处理delete操作
    void showCreateTable();     //处理查看表信息的操作
    void execFile();            //处理读取文件的操作
    void exit();                //处理退出的操作
private:
    void dealWithString();      //处理字符串
    string sql_string;          //暂存字符
    string getAWord(int pos, int &end_pos); //获取一个word
    string changeToLowerCase(string str, int pos);  //转换为小写
    string getRelationMark(int pos, int &end_pos);  //获取关系符号
    int getType(int pos, int &end_pos);             //获取类型
    int getBits(int int_num);                       //返回一个整数的位数
    int getBits(float float_num);                  //返回一个浮点数的位数
};

template <class T>
T stringToNum(const string &str)
{
    istringstream iss(str);
    T num;
    iss >> num;
    return num;
}

#endif /* _INTERPRETER_H_ */
