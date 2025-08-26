#include <iostream>
#include <string>
#include "GXPass.hpp"
using namespace std;
int main(int argc,char* argv[]){
    if(argc!=3) return 1;
    FILE* stream=fopen(argv[1],"rb");
    int c=0;
    string s="";
    while((c=fgetc(stream))!=-1){
        s+=char(c);
    }
    string s2=argv[2];
    if(GXPass::number2ABC(GXPass::compile(s))==s2){
        cout<<"True"<<endl;
    }else{
        cout<<"False"<<endl;
    }
    return 0;
}