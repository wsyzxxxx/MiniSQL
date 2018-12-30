#include<bits/stdc++.h>

using namespace std;

struct Data{
    int type;
    int datai;
    float dataf;
    std::string datas;
    bool operator<(Data v){
    	if (type==-1) return datai<v.datai;
    	if (type==0) return dataf<v.dataf;
    	return datas<v.datas;
    }
};

class check{
	public:
		check(){
		}
		~check(){
			printf("!!!\n");
		}
};

//vector<Data>v;
int main(){
	/*sort(v.begin(),v.end());
	check x;
	printf("@@@\n");
	Data pp;*/
	
	vector<int> v; v.clear();
	vector<int>::iterator it=find(v.begin(),v.end(),10);
	if (it!=v.end()) printf("%d\n",*it); else printf("Not found!\n");
	
	v.push_back(10);
	it=find(v.begin(),v.end(),10);
	if (it!=v.end()) printf("%d\n",*it); else printf("Not found!\n");
	
	cout<<v.end()-v.begin()<<endl;
	
	v.push_back(5);
	v.push_back(15);
	v.erase(v.begin()+1);
	
	for (int i=0;i<v.size();i++) printf("%d ",v[i]); cout<<endl;
	
	Insert(v.begin(),15);
	
	for (int i=0;i<v.size();i++) printf("%d ",v[i]); cout<<endl;	
}
