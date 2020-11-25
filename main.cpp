#include "inc/conn.h"
#include "inc/comp.h"
#include <csignal>

#define PORT 7070

using serve_sys::sys_sock;
using compose::composer;

sys_sock *sock;

void handler(int signum){
    printf("Signal recieved: %i\n", signum);
    delete sock;
    exit(signum);
}
int main(){
    signal(SIGINT, handler);
    signal(SIGSEGV, handler);

    const string path = "/home/luke/webserve";
    const string index = "/assets/html/response.html";

    sock = new sys_sock(PORT);
    composer comp = composer(&path, &index);
    sock->open();
    sock->listen_to(&comp);
    delete sock;
    return 0;
}