#include <iostream>
#include <string>
#include <cstdio>
#include "GXPass.hpp"
using namespace std;
int main(){
    string s;
    char str_c[1024];
    cin.getline(str_c,1024);
    s=str_c;
    cout<<s<<endl;
    FILE* stream=fopen(s.c_str(),"rb");
    int c;
    while((c=fgetc(stream))!=-1){
        s+=c;
    }
    fclose(stream);
    cout<<GXPass::number2ABC(GXPass::compile(s));
    return 0;
}