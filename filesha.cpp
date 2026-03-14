#include <iostream>
#include <string>
#include <fstream>
#include "GXPass.hpp"
using namespace std;
int main(){
    string file;
    cin>>file;
    string filedata;
    ifstream fin(file);
    if(fin){
        string line;
        while(getline(fin,line)){
            filedata+=line;
        }
    }
    fin.close();
    cout << GXPass::number2ABC(GXPass::compile(filedata)) << endl;
    cin>>file;
    return 0;
}