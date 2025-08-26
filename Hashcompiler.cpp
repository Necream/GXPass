#include <iostream>
#include "GXPass.hpp"
using namespace std;
int main(int argc,char* argv[]){
    if(argc!=2) return 1;
    FILE* stream=fopen(argv[1],"rb");
    int c=0;
    string s="";
    while((c=fgetc(stream))!=-1){
        s+=char(c);
    }
    cout<<GXPass::number2ABC(GXPass::compile(s))<<endl;
    return 0;
}