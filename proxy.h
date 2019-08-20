/* handle(respond to) all GET, POST, CONNECT requests */

#include "cache.h"
#include "client.h"
#include "http.h"
#include "httprqstparser.h"
#include "httprspnsparser.h"
#include "log.h"
#include "server.h"
#include "strparser.h"

#define NOTINCACHE 1
#define EXPIRED 2
#define NEEDVALIDATE 3
#define VALID 4

typedef struct sockaddr sockaddr;

class Proxy{
private:
    StrParser strparser;
    Server server;
    Log log;
    std::vector<char> handlebyCache(Cache &cache, HTTPRQSTParser &httprqstparser);
    std::vector<char> getNewResponse(Cache &cache, HTTPRQSTParser &httprqstparser, const std::vector<char> &request);
    std::vector<char> getRevalidRequest(HTTPRSPNSParser &httprspnsparer, HTTPRQSTParser &httprqstparser);
    std::vector<char> revalidation(HTTPRSPNSParser &httprspnsparser, HTTPRQSTParser &httprqstparser, Cache &cache);
    std::string getclientIP(int newfd);
    bool checkComplete(HTTPRQSTParser &httprqstparser, std::vector<char> &HTTPRequest);
    void GET_handler(HTTPRQSTParser &httprqstparser, int fd);
    void POST_handler(HTTPRQSTParser &httprqstparser, int fd);
    void CONNECT_handler(HTTPRQSTParser &httprqstparser, int fd);
    

public:
    Proxy(int requestid);
    Proxy(const char *port);
    ~Proxy();
    int acNewRequest();
    void handler(int fd);
};