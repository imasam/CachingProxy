#include "httprqstparser.h"

// select port
std::string selectPort(std::string &msg) {
  size_t target;
  std::string port;
  if ((target = msg.find(':')) != std::string::npos) {
    port = msg.substr(target + 1);
    msg = msg.substr(0, target);
  }
  else
    port = "80";
  return port;
}

void HTTPRQSTParser::parseRequest(std::string request){
    method = strparser.getNextSeg(request);
    path = strparser.getNextSeg(request);
    protocol = strparser.getNextSeg(request, '/');
    version_major = stoi(strparser.getNextSeg(request, '.'));
    version_minor = stoi(strparser.getNextSeg(request));
}

void HTTPRQSTParser::parseHeader(std::string head){
    while(!head.empty()){
        std::string key = strparser.tolower(strparser.getNextSeg(head, ':'));
        std::string val = strparser.getNextSeg(head, '\r', 2);
        headers[key] = val;
    }
}

int HTTPRQSTParser::verifyHeader(){
    try{
        if(headers.count("host"))
            throw std::string("host");
    }catch(std::string e){
        errNum = 1;
        return -1;
    }
    return 0;
}

// generate the full path for a http request
std::string HTTPRQSTParser::updateHTTPath(std::string &path){
    strparser.getNextSeg(path);
    path = strparser.getNextSeg(path);
    if(path.find(host) != std::string::npos)
        path.insert(0, host);
    return path;
}

// delete port num, hostpath, http:// in the status line
std::string HTTPRQSTParser::updateHTTPRequest(std::string request){
    size_t target;
    size_t end = request.find("\r\n");
    if((target = request.find("http://")) != std::string::npos)
        request.replace(target, 7, "");
    if((target = request.find(host)) != std::string::npos && target < end)
        request.replace(target, host.size(), "");
    if((target = request.find(":" + port)) != std::string::npos)
        request.replace(target, 1 + port.size(), "");
    end = request.find("\r\n");
    path = request.substr(0, end);
    statusline = request.substr(0, end);
    path = updateHTTPath(path);
    return request;
}

HTTPRQSTParser::HTTPRQSTParser(const std::vector<char> &r){
    if(r.empty())
        throw std::string("empty request");
    HTTPRequest = r.data();
    errNum = 0;
    int target = HTTPRequest.find("\r\n");
    statusline = HTTPRequest.substr(0, target);
    parseRequest(statusline);
    int head_end = HTTPRequest.find("\r\n\r\n");
    std::string head = HTTPRequest.substr(target + 2, head_end - target - 2);
    parseHeader(head);
    if (verifyHeader() != -1) {
        port = smartPort(headers["host"]);
        host = headers["host"];
    }
    HTTPRequest = updateHTTPRequest(HTTPRequest);
}

HTTPRQSTParser::~HTTPRQSTParser(){};

int HTTPRQSTParser::errorDetection(){
    return errNum;
}

std::string HTTPRQSTParser::getHostName(){
    return host;
}

std::string HTTPRQSTParser::getHostPort(){
    return port;
}

std::vector<char> HTTPRQSTParser::getRequest(){
    return std::vector<char>(HTTPRequest.begin(), HTTPRequest.end());
}

std::string HTTPRQSTParser::getMethod(){
    return method;
}

std::string HTTPRQSTParser::getURL(){
    return path;
}

std::string HTTPRQSTParser::getStatusLine(){
    return statusline;
}

int HTTPRQSTParser::getContentLen(){
    if(headers.count("content-length"))
        return stoi(headers["content-length"]);
    return 0;
}

bool HTTPRQSTParser::needRevalidate(){
    std::string ctrlPolicy = headers["cache-control"];
    if(ctrlPolicy.find("must-revalidation") != std::string::npos || ctrlPolicy.find("proxy-revalidation") != std::string::npos)
        return true;
    return false;    
}

bool HTTPRQSTParser::good4Cache(){
    if(!headers.count("cache-control"))
        return true;
        std::string ctrlPolicy = headers["cache-control"];
    if (ctrlPolicy.find("private") != std::string::npos ||
        ctrlPolicy.find("no-cache") != std::string::npos ||
        ctrlPolicy.find("no-store") != std::string::npos)
        return false;
    return true;
}

std::string HTTPRQSTParser::Bad4CacheInfo(){
    std::string ctrlPolicy = headers["cache-control"];
    if (ctrlPolicy.find("private") != std::string::npos)
        return "private policy of http response";
    if (ctrlPolicy.find("no-cache") != std::string::npos)
        return "no-cache policy of http response";
    if (ctrlPolicy.find("no-store") != std::string::npos)
        return "no-store policy of http response";
    return "max cache time is 0";
}