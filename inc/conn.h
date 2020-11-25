#pragma once

#include <netinet/in.h>
#include <stddef.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include <unistd.h>

#include "inc/comp.h"
#include "inc/std.h"

#define BACKLOG 32
#define BUF_RECV_SIZE 8192

#define BUF_SEND_SIZE 8192

namespace serve_sys{
    using std::string;

    class sys_sock{

        public:

        sys_sock(const size_t port) : _port{port} {
            // allocate room for our basic buffer
            memset(this->inbuf,'\0',BUF_RECV_SIZE);
            memset(this->outbuf,'\0',BUF_RECV_SIZE);
            

        }

        ~sys_sock(){
            printf("Closing Socket\n");
            shutdown(this->sockfd, 2);
            close(this->sockfd);
        }

        void open(){
            printf("Opening on port: %lu\n", this->_port);
            this->addr.sin_family = AF_INET;
            this->addr.sin_addr.s_addr = INADDR_ANY;
            this->addr.sin_port = htons(this->_port);

            this->addlen = sizeof(this->addr);

            this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if(this->sockfd == 0){
                this->result = this->sockfd;
                throw WebServeException("Could not create socket");
            }

            // forcefully bind to socket
            setsockopt(
                this->sockfd, 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                &this->opt, 
                sizeof(this->opt)
            );

            this->result = bind(
                this->sockfd, 
                (struct sockaddr *)&this->addr,
                this->addlen
            );

            if(this->result != 0){
                throw WebServeException("Could not bind to port");
            }

        }

        void listen_to(compose::composer *composer){
            string response;
            this->result = listen(this->sockfd, BACKLOG);
            if(this->result != 0){
                throw WebServeException("Could not initiate listen");
            }
            while(true){
                this->active_sock = accept(
                    this->sockfd,
                    (struct sockaddr *)&this->addr,
                    (socklen_t *)&this->addlen
                );
                this->result = recv(
                    this->active_sock,
                    &this->inbuf,
                    sizeof(this->inbuf),
                    0
                );
                // send our request
                composer->process_request(this->inbuf, &response);
                this->result = send(
                    this->active_sock,
                    response.c_str(),
                    response.length(),
                    0
                );
                response.clear();
                shutdown(this->active_sock, 2);
                close(this->active_sock);
                memset(this->inbuf,'\0',BUF_RECV_SIZE);
                memset(this->outbuf,'\0',BUF_RECV_SIZE);
            }
        }
        private:

        char *_host;
        size_t _port;

        int sockfd;
        int active_sock;

        int result;
        int opt = true;
        int addlen;

        struct sockaddr_in addr;

        char inbuf[BUF_RECV_SIZE];
        char outbuf[BUF_SEND_SIZE];

    };

}