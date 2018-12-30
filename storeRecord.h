
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

//存储辅助函数
char * SubString(char *str, int StartPostion, int SubstringLength);
int nextposdata( char * row, int startpos );
int nextposrow( char * content, int startpos );
char * SubString(char *str, int StartPostion, int SubstringLength);

//传入字符串的格式为-xxx xxx xxx-
//传入元组转换为该格式
//char2Tuper 将字符串转换为元组
//Tuper2char 将元组转换为字符串
char* Tuper2Char(Table& tableIn, Tuple& singleTuper);
Tuple* Char2Tuper(Table& tableIn, char* stringRow);
vector<Tuple> Content2Tuple( Table &tableIn, char * content ,int startPos);

//判断辅助函数
bool isSatisfied(Table& tableIn, Tuple &row, vector<int> mask, vector<Where> w);
bool isunique(Table& tableIn, Where w, int loca);
void deleteRecord( char *content,int rowPos );

#endif /* storeRecord_hpp */
