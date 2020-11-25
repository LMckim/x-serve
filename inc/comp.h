#pragma once

#include <chrono>
#include <map>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#include "inc/std.h"

#define HEADER_BUF 8192
#define MAX_HTTP_LINE 512
#define FILE_READ_SIZE 4096
#define DEFAULT_RESPONSE_SIZE 4096

namespace compose{

    using std::map;
    using std::string;
    using std::vector;
    using h_clock = std::chrono::high_resolution_clock;

    class composer{
        public:
        enum METHOD{
            OPTIONS,
            GET,
            HEAD,
            POST,
            PUT,
            DELETE,
            TRACE,
            CONNECT
        };
        enum RESPONSE_CODE{
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

        composer(const string* _home_dir, const string* _index_path) 
            : home_dir{_home_dir}, index_path{_index_path} 
            {
            // https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
            // initialize our request header map
            // dont have all of them here, see above
            this->req_headers["METHOD"]                 = "";
            this->req_headers["FILE"]                   = "";
            this->req_headers["HTTP_VERSION"]           = "";

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
            this->resp_headers["Age"]                   = "0";
            this->resp_headers["Allow"]                 = "GET, POST";
            this->resp_headers["Cache-Control"]         = "no-cache";
            this->resp_headers["Connection"]            = "keep-open";
            this->resp_headers["Content-Dispostion"]    = "";
            this->resp_headers["Content-Encoding"]      = "";
            this->resp_headers["Content-Length"]        = "";
            this->resp_headers["Content-MD5"]           = "";
            this->resp_headers["Content-Type"]          = "text/html; charset=utf-8";
            this->resp_headers["Date"]                  = "";
            this->resp_headers["Expires"]               = "";
            this->resp_headers["Last-Modifed"]          = "";
            this->resp_headers["Link"]                  = "";
            this->resp_headers["Transfer-Encoding"]     = "";
            this->resp_headers["Warning"]               = "";
            this->resp_headers["WWW-Authenticate"]      = "";

            // initialize stuff
            this->response_content.reserve(DEFAULT_RESPONSE_SIZE);

        }
        void process_request(const char *in, string *response){
            auto s_time = h_clock::now();

            RESPONSE_CODE r_code;
            printf("Connection\n");
            // printf("IN: %s\n", in);
            this->process_headers(in);
            // fetch content
            string content_path = *this->home_dir;
            if(this->req_headers["FILE"] != ""){
                if(this->req_headers["FILE"] == "/"){
                    content_path.append(*this->index_path);
                    r_code = this->load_content(&content_path);
                }else{
                    content_path.append(this->req_headers["FILE"]);
                    r_code = this->load_content(&content_path);
                }
            }else{
                r_code = RESPONSE_CODE::BAD_REQUEST;
            }
            // build response headers
            this->build_response_headers(&r_code);
            // build response
            *response = this->response_header + this->response_content;
            this->response_header.clear();
            this->response_content.clear();

            std::chrono::nanoseconds diff = h_clock::now() - s_time;
            std::chrono::microseconds t = std::chrono::duration_cast<std::chrono::microseconds>(diff);
            printf("Operations took: %s microseconds\n", std::to_string(t.count()).c_str());
        }

        private:
        const string *home_dir;
        const string *index_path;
        string err_msg;

        string response_header;
        string response_content;

        map<string, string> resp_headers;
        map<string, string> req_headers;

        void process_headers(const char *req){
            // track execution time
            vector<string> split;
            string headers(req), line;
            line.reserve(MAX_HTTP_LINE);

            size_t s_pos = 0, el_pos;
            while((el_pos = headers.find("\n", s_pos)) != string::npos){
                line = headers.substr(s_pos, el_pos - s_pos);
                // first line, get the request method, req file and http version
                if(s_pos == 0){
                    split = StringOps::splitString(&line,' ');
                    this->req_headers["METHOD"] = split[0];   
                    this->req_headers["FILE"] = split[1];   
                    this->req_headers["HTTP_VERSION"] = split[2];   
                }else{
                    split = StringOps::splitString(&line, ':');
                    if(split.size() < 2){
                        if(line != "\n") printf("Unidentified header: %s\n", line.c_str());
                    }else{
                        this->req_headers[ split[0] ] = split[1];
                    }
                }
                s_pos = el_pos + 1;
            }
        }
        void build_response_headers(RESPONSE_CODE *r_code){
            this->response_header = 
                this->resp_headers["HTTP_VERSION"] + 
                " " +
                std::to_string(static_cast<int>(*r_code)) +
                " " +
                this->get_response_text(r_code) + 
                "\n";

            for(auto i : this->resp_headers){
                if(i.second == "") continue;
                this->response_header.append(i.first);
                this->response_header.append(": ");
                this->response_header.append(i.second);
                this->response_header.append("\n");
                
            }
            this->response_header += "\n";
        }
        RESPONSE_CODE load_content(const string *path){
            FILE *fp = fopen(path->c_str(),"r");
            if(fp == 0) return RESPONSE_CODE::NOT_FOUND;
            // check we didnt
            char read_buf[DEFAULT_RESPONSE_SIZE] = {'\0'};
            while(!feof(fp)){
                fread(read_buf, sizeof(char), DEFAULT_RESPONSE_SIZE, fp);
                this->response_content.append(read_buf);
            }
            fclose(fp);
            this->resp_headers["Content-Length"] = std::to_string(this->response_content.length());
            return RESPONSE_CODE::OK;
        }
        string get_response_text(RESPONSE_CODE *code){
            string text;
            switch (*code){
                case RESPONSE_CODE::CONTINUE:
                    text = "Continue";
                break;
                case RESPONSE_CODE::SWITCH_PROTO:
                    text = "Switching Protocols";
                break;
                case RESPONSE_CODE::OK:
                    text = "OK";
                break;
                case RESPONSE_CODE::CREATED:
                    text = "Created";
                break;
                case RESPONSE_CODE::ACCEPTED:
                    text = "Accepted";
                break;
                case RESPONSE_CODE::NON_AUTH:
                    text = "Non-Authoritative Information";
                break;
                case RESPONSE_CODE::NO_CONTENT:
                    text = "No Content";
                break;
                case RESPONSE_CODE::RESET_CONT:
                    text = "Reset Content";
                break;
                case RESPONSE_CODE::PARTIAL_CONT:
                    text = "Partial Content";
                break;
                case RESPONSE_CODE::MULTIPLE_CHOICE:
                    text = "Multiple Choices";
                break;
                case RESPONSE_CODE::MOVED_PERM:
                    text = "Moved Permanently";
                break;
                case RESPONSE_CODE::FOUND:
                    text = "Found";
                break;
                case RESPONSE_CODE::SEE_OTHER:
                    text = "See Other";
                break;
                case RESPONSE_CODE::NOT_MODIFIED:
                    text = "Not Modified";
                break;
                case RESPONSE_CODE::USE_PROXY:
                    text = "Use Proxy";
                break;
                case RESPONSE_CODE::TEMP_REDIRECT:
                    text = "Temporary Redirect";
                break;
                case RESPONSE_CODE::BAD_REQUEST:
                    text = "Bad Request";
                break;
                case RESPONSE_CODE::UNAUTH:
                    text = "Unauthorized";
                break;
                case RESPONSE_CODE::PAY_REQ:
                    text = "Payment Required";
                break;
                case RESPONSE_CODE::FORBIDDEN:
                    text = "Forbidden";
                break;
                case RESPONSE_CODE::NOT_FOUND:
                    text = "Not Found";
                break;
                case RESPONSE_CODE::METHOD_NOT_ALLOWED:
                    text = "Method Not Allowed";
                break;
                case RESPONSE_CODE::NOT_ACCEPTABLE:
                    text = "Not Acceptable";
                break;
                case RESPONSE_CODE::PROXY_AUTH_REQ:
                    text = "Proxy Authentication Required";
                break;
                case RESPONSE_CODE::REQ_TIMEOUT:
                    text = "Request Time-out";
                break;
                case RESPONSE_CODE::CONFLICT:
                    text = " Conflict";
                break;
                case RESPONSE_CODE::GONE:
                    text = " Gone";
                break;
                case RESPONSE_CODE::LENGTH_REQ:
                    text = " Length Required";
                break;
                case RESPONSE_CODE::PRECOND_FAILED:
                    text = " Precondition Failed";
                break;
                case RESPONSE_CODE::REQ_ENT_TOO_LARGE:
                    text = " Request Entity Too Large";
                break;
                case RESPONSE_CODE::REQ_URI_TOO_LARGE:
                    text = " Request-URI Too Large";
                break;
                case RESPONSE_CODE::UNSUPPORTED_MEDIA:
                    text = " Unsupported Media Type";
                break;
                case RESPONSE_CODE::REQ_RANGE_NS:
                    text = " Requested range not satisfiable";
                break;
                case RESPONSE_CODE::EXPECTATION_FAILED:
                    text = " Expectation Failed";
                break;
                case RESPONSE_CODE::INTERNAL_SERVER_ERR:
                    text = "Internal Server Error";
                break;
                case RESPONSE_CODE::NOT_IMPLEMENTED:
                    text = "Not Implemented";
                break;
                case RESPONSE_CODE::BAD_GATEWAY:
                    text = "Bad Gateway";
                break;
                case RESPONSE_CODE::SERVICE_UNAVAIL:
                    text = "Service Unavailable";
                break;
                case RESPONSE_CODE::GATEWAY_TIMEOUT:
                    text = "Gateway Time-out";
                break;
                case RESPONSE_CODE::HTTP_VER_NS:
                    text = "HTTP Version not supported";
                break;
            
                default:
                    text = "Internal server Error";
                }
            return text;
        }
    };
}