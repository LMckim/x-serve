#include <inc/std.h>
#include <iostream>

using std::string;
using std::vector;

int main(){
    printf("STD TESTING:\n");
    string test = "Hello,My,Friend\n";

    printf("StringOps:\n");
    vector<string> split = StringOps::splitString(&test,',');
    for(auto i : split){
        printf("\t%s\n",i.c_str());
    }
    
    printf("WebServeException Testing:\n");
    try{
        throw WebServeException("Exception Sent");
    }catch(const WebServeException &e){
        printf("\tCaught: %s\n", e.what());
    }
    
    
    printf("STD TESTING COMPLETED\n");
    return 0;
}