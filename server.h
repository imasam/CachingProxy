#include<sys/socket.h>

#define MAXDATASIZE 65536
typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr sockaddr;

class Server{
private:
    const char *port;

public:
    Server();
    Server(const char* p);
    ~Server();

};