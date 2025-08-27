#include <iostream>
#include <string>
#include <windows.h>
#include "GXPass.hpp"
using namespace std;
int main(){
    string input;
    cin>>input;
    cout<<GXPass::fullsafe(input)<<endl;
    return 0;
}