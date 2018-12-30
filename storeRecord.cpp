
#include "storeRecord.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include "basic.h"
#include "stdlib.h"
#include "definitions.h"
#include <math.h>
using namespace std;


char* Tuper2Char(Table& tableIn, Tuple& singleTuper){
    char* ptrRes = NULL ;
    int pos = 1;//第一个位置做判断是否有效
    ptrRes = new char[RECORD_LENTH];//1位有效位、1位0
    memcpy(ptrRes,"|",sizeof(char));//未删除的元组前是|，删除为#
    for (int i = 0; i < tableIn.getAttr().num ; i++){
        if (tableIn.getAttr().type[i] == -1){ //int
            string value = to_string(singleTuper.getData()[i].datai);
            memcpy(ptrRes + pos, value.c_str(), value.length());
            pos += value.length();
            if( i != tableIn.getAttr().num - 1 ){
                memcpy(ptrRes + pos, "~", sizeof(char));
                pos ++;
            }
        }
        else if (tableIn.getAttr().type[i] == 0){
            string value = to_string(singleTuper.getData()[i].dataf);
            memcpy(ptrRes + pos, value.c_str(), value.length());
            pos += value.length();
            if( i != tableIn.getAttr().num - 1 ){
                memcpy(ptrRes + pos, "~", sizeof(char));
                pos ++;
            }
        }
        else{ //string
            string value = singleTuper.getData()[i].datas;
            int strLen = value.length();
            memcpy(ptrRes + pos, value.c_str() , strLen);
            pos += strLen;
            if( i != tableIn.getAttr().num - 1 ){
                memcpy(ptrRes + pos, "~", sizeof(char));
                pos ++;
            }
        }
    }
    ptrRes[pos++] = '|';
    ptrRes[pos] = '\0';
    return ptrRes;
}


int nextposrow( char * content, int startpos ){
    //从传入的-开始，到下一个-为止
    for( int i = startpos; i < strlen(content) ; i ++ ){
        //   cout << sizeof(content);
        if( content[i] == '|' && content[i+1] == '#'  )//可能下一个已删除
            return i+1;
        else if( content[i] == '|' && content[i+1] == '|'  )//下一个未删除
            return i+1;
    }
    return strlen(content);//表示最后一个位置
}

int nextposdata( char * row, int startpos ){
    //从传入的-开始，到下一个-为止
    for( int i = startpos; i < strlen(row) ; i ++ ){
        if( row[i] == '~' )
            return i;
    }
    return strlen(row);//表示最后一个位置
}

char * SubString(char *str, int StartPostion, int SubstringLength){
    int stringlen = 0;
    int i = 0;
    int x = 0;
    char *tmp;
    stringlen = (int)strlen(str);
    tmp = (char *)malloc(sizeof(char)*(SubstringLength + 1));
    if ((StartPostion < 0) || (SubstringLength <= 0) || (stringlen == 0) || (StartPostion >= stringlen))
    {
        
        strcpy(tmp, "\0");
        return tmp;
    }
    for (i = StartPostion; ((i < stringlen) && (x < SubstringLength)); i++)
    {
        tmp[x] = str[i];
        x++;
    }
    tmp[x] = '\0';
    return tmp;
}

Tuple* Char2Tuper(Table& tableIn, char* stringRow)
{
    Tuple* temp_tuper;
    temp_tuper = new Tuple;
    if (stringRow[0] == '#'){
        temp_tuper->setDeleted();
        return temp_tuper;
    }//字符串是空（删除）的
    
    stringRow = SubString(stringRow, 1, nextposrow(stringRow, 1)-2 );
    for (int attr_index = 0 , rowPos = 0;
         attr_index < tableIn.getAttr().num;
         attr_index++ , rowPos = nextposdata(stringRow, rowPos)+1){
        Data *newData = new Data;
        if (tableIn.getAttr().type[attr_index] == -1) {//int
            newData->type = -1;
            newData->datai = atoi(SubString(stringRow, rowPos, nextposdata(stringRow, rowPos)-rowPos));
            temp_tuper->addData(*newData);
        }
        else if (tableIn.getAttr().type[attr_index] == 0) {//float
            newData->type = 0;
            newData->dataf = atof(SubString(stringRow, rowPos, nextposdata(stringRow, rowPos)-rowPos));
            temp_tuper->addData(*newData);
        }
        else {
            newData->type = tableIn.getAttr().type[attr_index];
            newData->datas = SubString(stringRow, rowPos, nextposdata(stringRow, rowPos)-rowPos);
            temp_tuper->addData(*newData);
        }
    }
    return temp_tuper;
}

vector<Tuple> Content2Tuple( Table &tableIn, char * content ,int startPos){
    vector<Tuple> TupleOut;
    char * row;
    for( int i = startPos; i < strlen(content) ; i = nextposrow(content, i) ){
        row = SubString(content, i, nextposrow(content, i)-i);//整行
        Tuple *tmp_tuper = new Tuple;
        tmp_tuper = Char2Tuper(tableIn,row);
        if(!tmp_tuper->isDeleted())
            TupleOut.push_back(*tmp_tuper);
    }
    return TupleOut;
}

bool isSatisfied(Table &tableIn, Tuple &row, vector<int> mask, vector<Where> w){
    for (int i = 0; i < mask.size();i++){
         if ( mask[i] == -1 ){ //该where已通过索引
         continue;
         }
        if (row.getData().size() == 0 )
            return false;
        if ( row.getData()[mask[i]].type == -1 ) { //int
            switch (w[i].relation_character) {
                case EQUAL:
                    if (!(row.getData()[mask[i]].datai == w[i].data.datai))
                        return false;
                    break;
                case LESS_OR_EQUAL:
                    if (!(row.getData()[mask[i]].datai <= w[i].data.datai))
                        return false;
                    break;
                case LESS:
                    if (!(row.getData()[mask[i]].datai < w[i].data.datai))
                        return false;
                    break;
                case GREATER_OR_EQUAL:
                    if (!(row.getData()[mask[i]].datai >= w[i].data.datai))
                        return false;
                    break;
                case GREATER:
                    if (!(row.getData()[mask[i]].datai > w[i].data.datai))
                        return false;
                    break;
                case NOT_EQUAL:
                    if (!(row.getData()[mask[i]].datai != w[i].data.datai))
                        return false;
                    break;
                default: ;
            }
        }
        else if (row.getData()[mask[i]].type == 0) { //Float
            switch (w[i].relation_character) {
                case EQUAL:
                    if (!(fabs(row.getData()[mask[i]].dataf - w[i].data.dataf) < 0.0001) )
                        return false;
                    break;
                case LESS_OR_EQUAL:
                    if (!(row.getData()[mask[i]].dataf <= w[i].data.dataf))
                        return false;
                    break;
                case LESS:
                    if (!(row.getData()[mask[i]].dataf < w[i].data.dataf))
                        return false;
                    break;
                case GREATER_OR_EQUAL:{ 
                    if (!(row.getData()[mask[i]].dataf >= w[i].data.dataf))
                        return false;
                    break;} 
                case GREATER:
                    if (!(row.getData()[mask[i]].dataf > w[i].data.dataf))
                        return false;
                    break;
                case NOT_EQUAL:
                    if (!(row.getData()[mask[i]].dataf != w[i].data.dataf))
                        return false;
                    break;
                default: ;
            }
        }
        else if (row.getData()[mask[i]].type > 0){ //string
            switch (w[i].relation_character) {
                case EQUAL:
                    if (!(row.getData()[mask[i]].datas == w[i].data.datas) )
                        return false;
                    break;
                case LESS_OR_EQUAL:
                    if (!(row.getData()[mask[i]].datas <= w[i].data.datas))
                        return false;
                    break;
                case LESS:
                    if (!(row.getData()[mask[i]].datas < w[i].data.datas))
                        return false;
                    break;
                case GREATER_OR_EQUAL:
                    if (!(row.getData()[mask[i]].datas >= w[i].data.datas))
                        return false;
                    break;
                case GREATER:
                    if (!(row.getData()[mask[i]].datas > w[i].data.datas))
                        return false;
                    break;
                case NOT_EQUAL:
                    if (!(row.getData()[mask[i]].datas != w[i].data.datas))
                        return false;
                    break;
                default: ;
            }
        }
    }
    return true;
}

void deleteRecord( char *content,int rowPos ){
    content[rowPos] = '#';
}




