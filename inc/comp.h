#pragma once

#include <chrono>
#include <map>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#define HEADER_BUF 8192

namespace compose{

    using std::map;
    using std::string;
    using std::vector;
    using h_clock = std::chrono::high_resolution_clock;

    class composer{
        public:
        enum METHODS{
            OPTIONS,
            GET,
            HEAD,
            POST,
            PUT,
            DELETE,
            TRACE,
            CONNECT
        };
        enum RESPONSE_CODES{
          CONTINUE              = 100,//  ; Section 10.1.1: Continue
          SWITCH_PROTO          = 101,//  ; Section 10.1.2: Switching Protocols
          OK                    = 200,//  ; Section 10.2.1: OK
          CREATED               = 201,//  ; Section 10.2.2: Created
          ACCEPTED              = 202,//  ; Section 10.2.3: Accepted
          NON_AUTH              = 203,//  ; Section 10.2.4: Non-Authoritative Information
          NO_CONTENT            = 204,//  ; Section 10.2.5: No Content
          RESET_CONT            = 205,//  ; Section 10.2.6: Reset Content
          PARTIAL_CONT          = 206,//  ; Section 10.2.7: Partial Content
          MULTIPLE_CHOICE       = 300,//  ; Section 10.3.1: Multiple Choices
          MOVED_PERM            = 301,//  ; Section 10.3.2: Moved Permanently
          FOUND                 = 302,//  ; Section 10.3.3: Found
          SEE_OTHER             = 303,//  ; Section 10.3.4: See Other
          NOT_MODIFIED          = 304,//  ; Section 10.3.5: Not Modified
          USE_PROXY             = 305,//  ; Section 10.3.6: Use Proxy
          TEMP_REDIRECT         = 307,//  ; Section 10.3.8: Temporary Redirect
          BAD_REQUEST           = 400,//  ; Section 10.4.1: Bad Request
          UNAUTH                = 401,//  ; Section 10.4.2: Unauthorized
          PAY_REQ               = 402,//  ; Section 10.4.3: Payment Required
          FORBIDDEN             = 403,//  ; Section 10.4.4: Forbidden
          NOT_FOUND             = 404,//  ; Section 10.4.5: Not Found
          METHOD_NOT_ALLOWED    = 405,//  ; Section 10.4.6: Method Not Allowed
          NOT_ACCEPTABLE        = 406,//  ; Section 10.4.7: Not Acceptable
          PROXY_AUTH_REQ        = 407,//  ; Section 10.4.8: Proxy Authentication Required
          REQ_TIMEOUT           = 408,//  ; Section 10.4.9: Request Time-out
          CONFLICT              = 409,//  ; Section 10.4.10: Conflict
          GONE                  = 410,//  ; Section 10.4.11: Gone
          LENGTH_REQ            = 411,//  ; Section 10.4.12: Length Required
          PRECOND_FAILED        = 412,//  ; Section 10.4.13: Precondition Failed
          REQ_ENT_TOO_LARGE     = 413,//  ; Section 10.4.14: Request Entity Too Large
          REQ_URI_TOO_LARGE     = 414,//  ; Section 10.4.15: Request-URI Too Large
          UNSUPPORTED_MEDIA     = 415,//  ; Section 10.4.16: Unsupported Media Type
          REQ_RANGE_NS          = 416,//  ; Section 10.4.17: Requested range not satisfiable
          EXPECTATION_FAILED    = 417,//  ; Section 10.4.18: Expectation Failed
          INTERNAL_SERVER_ERR   = 500,//  ; Section 10.5.1: Internal Server Error
          NOT_IMPLEMENTED       = 501,//  ; Section 10.5.2: Not Implemented
          BAD_GATEWAY           = 502,//  ; Section 10.5.3: Bad Gateway
          SERVICE_UNAVAIL       = 503,//  ; Section 10.5.4: Service Unavailable
          GATEWAY_TIMEOUT       = 504,//  ; Section 10.5.5: Gateway Time-out
          HTTP_VER_NS           = 505,//  ; Section 10.5.6: HTTP Version not supported
        };

        composer(const char* _home_dir) : home_dir{_home_dir} {
            // https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
            // initialize our request header map
            // dont have all of them here, see above
            this->req_headers["Accept"]                 = "";
            this->req_headers["Accept-Charset"]         = "";
            this->req_headers["Accept-Encoding"]        = "";
            this->req_headers["Accept-Language"]        = "";
            this->req_headers["Authorization"]          = "";
            this->req_headers["Expect"]                 = "";
            this->req_headers["From"]                   = "";
            this->req_headers["Host"]                   = "";
            this->req_headers["If-Match"]               = "";
            this->req_headers["If-Modified-Since"]      = "";
            this->req_headers["If-None-Match"]          = "";
            this->req_headers["If-Range"]               = "";
            this->req_headers["If-Unmodified-Since"]    = "";
            this->req_headers["Max-Forwards"]           = "";
            this->req_headers["Proxy-Authorization"]    = "";
            this->req_headers["Range"]                  = "";
            this->req_headers["Referer"]                = "";
            this->req_headers["TE"]                     = "";
            this->req_headers["User-Agent"]             = "";

            // dont have all of them here, see above
            this->resp_headers["HTTP_VERSION"]          = "HTTP/1.1";
            this->resp_headers["SERVER_NAME"]           = "XSERVE";
            this->resp_headers["Age"]                   = "";
            this->resp_headers["Allow"]                 = "";
            this->resp_headers["Cache-Control"]         = "";
            this->resp_headers["Connection"]            = "";
            this->resp_headers["Content-Dispostion"]    = "";
            this->resp_headers["Content-Encoding"]      = "";
            this->resp_headers["Content-Length"]        = "";
            this->resp_headers["Content-MD5"]           = "";
            this->resp_headers["Content-Type"]          = "";
            this->resp_headers["Date"]                  = "";
            this->resp_headers["Expires"]               = "";
            this->resp_headers["Last-Modifed"]          = "";
            this->resp_headers["Link"]                  = "";
            this->resp_headers["Transfer-Encoding"]     = "";
            this->resp_headers["Warning"]               = "";
            this->resp_headers["WWW-Authenticate"]      = "";

        }
        void process_request(const char *in, char *out, size_t outSize){
            printf("Connection\n");
            // printf("IN: %s\n", in);
            this->process_headers(in, out, outSize);
        }

        private:
        const char *home_dir;
        const string ver = "HTTP/1.1";
        const string serverName = "luke/v0.1";
        string err_msg;

        map<string, string> resp_headers;
        map<string, string> req_headers;

        void process_headers(const char *in, char *out, size_t outSize){
            // track execution time
            auto s_time = h_clock::now();
            size_t pos = 0, ppos = 0;
            string headers(in);
            string header;
            vector<string> headlines;
            while((pos = headers.find('\n', ppos)) != string::npos){
                header = headers.substr(ppos, pos - ppos);
                headlines.push_back(header);
                ppos = pos + 1;

                if(header.find("GET") != string::npos){
                    size_t fpos, fepos;
                    if((fpos = header.find("/")) != string::npos){
                        fepos = header.find(" ",fpos);
                        // string GET_file(this->home_dir);
                        string GET_file = header.substr(fpos,fepos-fpos);
                        if(GET_file == "/"){
                            printf("Sending base req\n");
                            FILE *fp = fopen("assets/html/response.html","r");
                            fread(out, sizeof(char), outSize, fp);
                            fclose(fp);
                        }else{
                            GET_file = this->home_dir + GET_file;
                            FILE *fp = fopen(GET_file.c_str(),"r");
                            if(fp == 0){
                                this->err_msg = "File not found" + GET_file + "\n";
                                printf("%s\n", this->err_msg.c_str());
                                strcpy(out,"HTTP/1.1 404 File Not Found\n\n");
                            }else{
                                fread(out, sizeof(char), outSize, fp);
                                fclose(fp);
                            }
                        }
                        
                    }

                }
            }
            auto diff = h_clock::now() - s_time;
            auto  t = std::chrono::duration_cast<std::chrono::microseconds>(diff);
            printf("Operations took: %s microseconds\n", std::to_string(t.count()).c_str());
        }

        vector<string> process_headers(const char *in){

        }
    };
}