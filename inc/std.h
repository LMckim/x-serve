#pragma once

#include <exception>
#include <string>
#include <vector>

using std::string;
using std::vector;

class WebServeException : public std::exception {
    public:
    std::string msg;
    WebServeException(const char* msg){
        this->msg.append("Exception: ");
        this->msg.append(msg);
        this->msg.append("\n");
    }

    virtual const char* what() const throw() {
        return this->msg.c_str();
    }

};

class StringOps{
    public:
    static vector<string> splitString(const string* str, char delimiter){
        vector<string> split;
        size_t pos1 = 0, pos2;
        while((pos2 = str->find(delimiter, pos1)) != string::npos){
            split.push_back(str->substr(pos1,pos2 - pos1));
            pos1 = pos2 + 1;
        }
        // add the last section
        split.push_back(str->substr(pos1,pos2 - pos1));
        return split;
    } 
    private:
    StringOps() {}
};