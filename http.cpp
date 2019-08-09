#include "http.h"

std::vector<char> http200(){
    std::string str = "HTTP/1.1 200 OK\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}

std::vector<char> http400(){
    std::string str = "HTTP/1.1 400 Bad Request\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}

std::vector<char> http502(){
    std::string str = "HTTP/1.1 502 Bad Gateway\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}