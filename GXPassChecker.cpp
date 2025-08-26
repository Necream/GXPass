#include <iostream>
#include <string>
#include <fstream>
using namespace std;
int main(int argc,char *argv[]){
    if(argc==3){
        string tempfilename="tempfile.GXP";
        string passwordfilename=argv[2];
        if(passwordfilename==tempfilename){
            tempfilename="tempfile2.GXP";
        }
        string cmd="GXPassCompiler.exe ";
        cmd+=argv[1];
        cmd+=" "+tempfilename;
        system(cmd.c_str());
        FILE *stream1,*stream2;
        stream1=fopen(tempfilename.c_str(),"r");
        stream2=fopen(argv[2],"r");
        if(!stream1||!stream2){
            return 1;
        }
        int c1,c2;
        while(((c1=fgetc(stream1))!=-1)&&((c2=fgetc(stream2))!=-1)){
            if(c1!=c2) return -1;
        }
    }else return 1;
    return 0;
}