#include <iostream>
#include <string>
#include "GXPass.hpp"
using namespace std;
int main(){
    string input;
    char Input[1024];
    cin.getline(Input,1024);
    input=Input;
    cout<<GXPass::number2ABC(GXPass::compile(input))<<endl;
    return 0;
}