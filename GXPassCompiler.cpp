#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "GXPass.hpp"
using namespace std;
int main(int argc,char *argv[]){
    if(argc>=2){
        if(argc==2){
            stringstream ss;
            ss<<GXPass::compile(argv[1]);
            string str;
            ss>>str;
            cout<<str;
        }else if(argc==3){
            ofstream fout;
            fout.open(argv[2]);
            if(!fout) return 1;
            fout<<GXPass::compile(argv[1]);
            fout.close();
        }
    }else return 1;
    return 0;
}