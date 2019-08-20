/* socket server
   accept new socket connect, send message and receive message from browser. 
*/

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "strparser.h"
#include "httprqstparser.h"

#define MAXDATASIZE 65536
typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr sockaddr;

class Server{
private:
    const char *port;
    addrinfo host_info;
    int listener;
    std::vector<char> recvAllRQST(int fd), recvAllData(int fd);
    int sendAll(int fd, const char* bug, size_t* len);

public:
    Server();
    Server(const char* p);
    ~Server();
    int acNewConnect();
    std::vector<char> basicRecv(int fd);
    std::vector<char> recvHTTPRequest(int fd);
    std::vector<char> recvData(int fd);
    void send(int fd, const std::vector<char>& msg);
};