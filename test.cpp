#include <iostream>
#include <string>
// #include <windows.h>
#include "GXPass.hpp"
#include "NCint.hpp"
using namespace std;
int main(){
    string filapath;
    string password;
    cout<<"Enter the file path to test:"<<endl;
    getline(cin,filapath);
    cout<<"Enter the password:"<<endl;
    getline(cin,password);
    GXPass::makeencryptfile(filapath,password,-1,true,true);
    GXPass::makedecryptfile(filapath+".GXF",password,true);

    return 0;
}