#include "HTTP.h"

std::vector<char> HTTP200(){
    std::string str = "HTTP/1.1 200 OK\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}

std::vector<char> HTTP400(){
    std::string str = "HTTP/1.1 400 Bad Request\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}

std::vector<char> HTTP502(){
    std::string str = "HTTP/1.1 502 Bad Gateway\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}

std::vector<char> HTTP503(){
    std::string str = "HTTP/1.1 503 Service Unavailable\n";
    std::vector<char> info(str.begin(), str.end());
    return info;
}