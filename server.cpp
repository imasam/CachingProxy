#include "server.h"
using namespace std;

Server::Server() {
  signal(SIGPIPE, SIG_IGN);
  listener = -1;
}
Server::~Server() {
  if (listener != -1)
    close(listener);
}

Server::Server(const char* p) : port(p){

};