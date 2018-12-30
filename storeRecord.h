
#ifndef storeRecord_hpp
#define storeRecord_hpp

#include <stdio.h>
#include "storeRecord.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "basic.h"
#include "stdlib.h"
#include <math.h>
using namespace std;

//�洢��������
char * SubString(char *str, int StartPostion, int SubstringLength);
int nextposdata( char * row, int startpos );
int nextposrow( char * content, int startpos );
char * SubString(char *str, int StartPostion, int SubstringLength);

//�����ַ����ĸ�ʽΪ-xxx xxx xxx-
//����Ԫ��ת��Ϊ�ø�ʽ
//char2Tuper ���ַ���ת��ΪԪ��
//Tuper2char ��Ԫ��ת��Ϊ�ַ���
char* Tuper2Char(Table& tableIn, Tuple& singleTuper);
Tuple* Char2Tuper(Table& tableIn, char* stringRow);
vector<Tuple> Content2Tuple( Table &tableIn, char * content ,int startPos);

//�жϸ�������
bool isSatisfied(Table& tableIn, Tuple &row, vector<int> mask, vector<Where> w);
bool isunique(Table& tableIn, Where w, int loca);
void deleteRecord( char *content,int rowPos );

#endif /* storeRecord_hpp */
