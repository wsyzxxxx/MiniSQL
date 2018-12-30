
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
    void getInput();    //��ȡ������ַ�
    void parseInput();  //��һ����������
    void selectFromTable();     //����select����
    void dropTable();           //����drop table����
    void dropIndex();           //����drop index�Ĳ���
    void createTable();         //����create table����
    void createIndex();         //����create index�Ĳ���
    void insertIntoTable();     //����insert Table�Ĳ���
    void deleteFromTable();     //����delete����
    void showCreateTable();     //����鿴����Ϣ�Ĳ���
    void execFile();            //�����ȡ�ļ��Ĳ���
    void exit();                //�����˳��Ĳ���
private:
    void dealWithString();      //�����ַ���
    string sql_string;          //�ݴ��ַ�
    string getAWord(int pos, int &end_pos); //��ȡһ��word
    string changeToLowerCase(string str, int pos);  //ת��ΪСд
    string getRelationMark(int pos, int &end_pos);  //��ȡ��ϵ����
    int getType(int pos, int &end_pos);             //��ȡ����
    int getBits(int int_num);                       //����һ��������λ��
    int getBits(float float_num);                  //����һ����������λ��
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
