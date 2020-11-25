#include <exception>
#include <string>

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