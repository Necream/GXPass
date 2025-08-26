#include <iostream>
#include <fstream>
#include "GXPass.hpp"
using namespace std;
int main(int argc,char* argv[]){
    if(argc==1){
        cout<<argv[0]<<" [File]"<<endl;
        cout<<argv[0]<<" [File] [OutputFile(.GXP)]"<<endl;
    }
    if(argc==2){
        cout<<GXPass::compile(argv[1]);
    }
    if(argc==3){
        string outputfile=argv[2];
        outputfile+=".GXP";
        ofstream fout;
        fout.open(outputfile);
        fout<<GXPass::compile(argv[1]);
        fout.close();
    }
    return 0;
}