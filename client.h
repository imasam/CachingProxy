/* 
works as a socket client
connect server, send message to server and receive message from server. 
*/

#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define MAXDATASIZE 65536

typedef struct addrinfo addrinfo;
typedef struct timeval timeval;

class Client{
private:
  const char *hostname;
  const char *port;
  int error;
  int sockfd;
  const char *getHost(const char *hostname);
  int sendAll(int fd, const char *buf, size_t* len);
  std::vector<char> recvAll(int fd);

public:
  Client(const char *h, const char *p);
  ~Client();
  int getError();
  int getFD();
  std::vector<char> recvServerResponse();
  std::vector<char> basicRecv();
  void send(const std::vector<char> &msg);
};