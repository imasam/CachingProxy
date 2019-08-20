/* generate log */

#include <algorithm>
#include <arpa/inet.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "strparser.h"

#define NOTINCACHE 1
#define EXPIRED 2
#define NEEDVALIDATE 3
#define VALID 4
#define PATH "/var/log/erss"
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class Log{
private:
    int requestid;
    void save(std::string msg);

public:
    Log();
    Log(int id);
    ~Log();
    void newRequest(std::string statusLine, std::string clientIP);
    void checkCache(int status, std::string expiredTime);
    void reqFromServer(std::string statusLine, std::string serverName);
    void recvFromServer(std::vector<char> statusText, std::string serverName);
    void respond2Client(std::vector<char> statusText);
    void notCacheable(std::string reason);
    void cached(std::string expireDate);
    void cachedNeedRevalid();
    void closeTunnel();
};