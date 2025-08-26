#include <iostream>
#include <string>
using namespace std;
int main(int argc,char *argv[]){
    if(argc==1){
        cout<<argv[0]<<" [Password]"<<endl;
        cout<<argv[0]<<" [File1] [File2]\t:when have any different return -1,if dont\'t return 0"<<endl;
        return 1;
    }
    if(argc==2){
        string cmd="GXPassCompiler.exe ";
        cmd+=argv[1];
        cmd+=" CompilationResult.GXP";
        system(cmd.c_str());
    }
    if(argc==3){
        if(1){
            string cmd="GXPassCompiler.exe ";
            cmd+=argv[1];
            cmd+=" CompileTemp1.GXP";
            system(cmd.c_str());
        }
        if(1){
            string cmd="GXPassCompiler.exe ";
            cmd+=argv[2];
            cmd+=" CompileTemp2.GXP";
            system(cmd.c_str());
        }
        FILE *stream1,*stream2;
        stream1=fopen("CompileTemp1.GXP","r");
        stream2=fopen("CompileTemp2.GXP","r");
        if(!stream1||!stream2) return 1;
        int c1,c2;
        while(((c1=fgetc(stream1))!=-1)&&((c2=fgetc(stream2))!=-1)){
            if(c1!=c2) return -1;
        }
    }
    return 0;
}