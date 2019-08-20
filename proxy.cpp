#include "proxy.h"

// use reponse in cache
std::vector<char> Proxy::handlebyCache(Cache &cache, HTTPRQSTParser &httprqstparser){
    std::string url = httprqstparser.getURL();
    if(!cache.check(url)){
        log.checkCache(NOTINCACHE, "");
        return std::vector<char>();
    }
    std::vector<char> HTTPResponse = cache.read(url);
    HTTPRSPNSParser httprspnsparser(HTTPResponse);
    bool needRevalidate = false;
    // need revalidate
    if(httprspnsparser.needRevalidate() || !httprspnsparser.good4Cache() || httprqstparser.needRevalidate()){
        log.checkCache(needRevalidate, "");
        needRevalidate = true;
    }
    // expire
    else if(!httprspnsparser.not_expire()){
        log.checkCache(EXPIRED, httprspnsparser.expiresAt());
        needRevalidate = true;
    }
    if(needRevalidate)
        return revalidation(httprspnsparser, httprqstparser, cache);
    // valid
    log.checkCache(VALID, "");
    return httprspnsparser.getResponse();
}

// get response from original server
std::vector<char> Proxy::getNewResponse(Cache &cache, HTTPRQSTParser &httprqstparser, const std::vector<char> &request){
    std::string url = httprqstparser.getURL();
    std::string hostname = httprqstparser.getHostName();
    std::string port = httprqstparser.getHostPort();
    std::string statusLine = httprqstparser.getStatusLine();

    Client client(hostname.c_str(), port.c_str()); // !!! have to check success or not, if failed, return 503,important
    if(client.getError == 1)
        return HTTP503();
    client.send(request);
    log.reqFromServer(statusLine, hostname);

    std::vector<char> HTTPResponse = client.recvServerResponse();
    HTTPRSPNSParser httprspnsparser(HTTPResponse);
    log.recvFromServer(httprspnsparser.getStatusText(), hostname);

    if(httprspnsparser.getStatusCode == 200 && httprspnsparser.good4Cache() && httprqstparser.good4Cache()){
        cache.store(url, HTTPResponse);
        if (httprspnsparser.needRevalidate())
            log.cachedNeedRevalid();
        else
            log.cached(httprspnsparser.expiresAt());
    }
    else{
        if(!httprspnsparser.good4Cache())
            log.notCacheable(httprspnsparser.Bad4CacheInfo());
        else if(!httprqstparser.good4Cache())
            log.notCacheable(httprqstparser.Bad4CacheInfo());
    }

    if(httprspnsparser.getStatusCode() == 304){
        std::vector<char> HTTPResponse = cache.read(url);
        HTTPRSPNSParser respinCache(HTTPResponse);
        return respinCache.getResponse();
    }
    return HTTPResponse;
}


// add this function
std::vector<char> Proxy::getRevalidRequest(HTTPRSPNSParser &httprspnsparer, HTTPRQSTParser &httprqstparser){
    std::string msg = httprqstparser.getStatusLine() + "\r\n";
    msg = msg + "Host: " + httprqstparser.getHostName() + "\r\n";
    msg = msg + "If-Modified-Since: " + httprspnsparer.getLastModified() + "\r\n";
    std::string tmp = httprspnsparer.getETag();
    if(tmp.size() != 0)
        msg = msg + "If-None-Match: " + tmp + "\r\n";
    msg += "\r\n";
    std::vector<char> request(msg.begin(), msg.end());
    return request;
}

std::vector<char> Proxy::revalidation(HTTPRSPNSParser &httprspnsparser, HTTPRQSTParser &httprqstparser, Cache &cache){
    std::vector<char> request = getRevalidRequest(httprspnsparser, httprqstparser);
    return getNewResponse(cache, httprqstparser, request);
}

std::string Proxy::getclientIP(int newfd){
  sockaddr_storage addr;
  socklen_t len = sizeof addr;
  char ip[INET_ADDRSTRLEN];
  try{
    if(getpeername(newfd, (struct sockaddr *)&addr, &len) == -1)
      throw std::string("getpeername");
    sockaddr_in *s = (sockaddr_in *)&addr;
    if(inet_ntop(AF_INET, &s->sin_addr, ip, sizeof ip) == NULL)
      throw std::string("inet_ntop");
  }catch (std::string e) {}

  return std::string(ip);
}

void Proxy::GET_handler(HTTPRQSTParser &httprqstparser, int fd);

void Proxy::POST_handler(HTTPRQSTParser &httprqstparser, int fd);

void Proxy::CONNECT_handler(HTTPRQSTParser &httprqstparser, int fd);

Proxy::Proxy(int requestid){
    log = requestid;
}

Proxy::Proxy(const char *port){
    server = port;
}

Proxy::~Proxy(){}

int Proxy::acNewRequest(){
    int newfd = server.acNewConnect();
    return newfd;
}

// 
bool Proxy::checkComplete(HTTPRQSTParser &httprqstparser, std::vector<char> &HTTPRequest){
  StrParser strparser;
  if(httprqstparser.getMethod() != "POST" && (!strparser.containNewLine(HTTPRequest) || httprqstparser.errorDetection() == 1))
    return true;
  return false;
}

// summerize all request handlers
void Proxy::handler(int fd){
    try{
        std::vector<char> HTTPRequest = server.recvHTTPRequest(fd);
        HTTPRQSTParser httprqstparser(HTTPRequest);

        if (checkComplete(httprqstparser, HTTPRequest)) {
        server.send(fd, HTTP400());
        log.respond2Client(HTTP400());
        return;
        }
        log.newRequest(httprqstparser.getStatusLine(), getclientIP(fd));

        if (httprqstparser.getMethod() == "GET")
        GET_handler(httprqstparser, fd);
        else if (httprqstparser.getMethod() == "CONNECT")
        CONNECT_handler(httprqstparser, fd);
        else if (httprqstparser.getMethod() == "POST")
        POST_handler(httprqstparser, fd);

    }catch(std::string e){}
}