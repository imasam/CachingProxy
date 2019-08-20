#include "client.h"

// get ip from hostname
const char* Client::getHost(const char *hostname) {
	struct hostent *he = gethostbyname(hostname);
  	if (he == nullptr)
    	throw std::string("no host");
  	struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
  	return inet_ntoa(*addr_list[0]);
}
  
// send all data to socket
int Client::sendAll(int fd, const char *buf, size_t *len){
    size_t total = 0; // already sent
    int left = *len; // need to send
    int n;
    while(total < left){
        if(n = send(fd, buf + total, left, 0) == -1) // problem here
            break;
        total += n;
        left -= n;
    }
    *len = total; // return already sent
    return n == -1 ? -1 : 0; // -1 : failure, 0 : success
}

std::vector<char> Client::basicRecv(){
    int index = 0;
    std::vector<char> msg;
    msg.resize(MAXDATASIZE);
    index = recv(sockfd, &msg.data()[index], MAXDATASIZE - 1, 0);
    if(index == -1)
         throw std::string("receive failed");       
    msg.resize(index);
    return msg;
}

// receive all data from socket
std::vector<char> Client::recvAll(int fd){
    // set timeout for receive
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)))
        throw std::string("set sockopt");

    std::vector<char> msg;
    size_t index = 0;
    int nbytes;
    while(true){
        if(msg.size() < index + MAXDATASIZE)
            msg.resize(index + MAXDATASIZE);
        nbytes = recv(fd, &msg.data()[index], MAXDATASIZE - 1, 0);
        // need to simplify here 
        if(msg.empty() && nbytes == -1)
            break;
        else if(nbytes <= 0)
            break;
        else
            index += nbytes;
    }
    msg.resize(index);
    return msg;
}

Client::Client(const char* h, const char* p){
    port = p;
    hostname = getHost(h);
    error = 0;
    addrinfo host_info;
    addrinfo* host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    try{
        if(getaddrinfo(hostname, port, &host_info,&host_info_list))
            throw std::string("get addr info");
        sockfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
        if(sockfd == -1)
            throw std::string("socket");
        if(connect(sockfd, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1)
            throw std::string("connect");
    }catch(std::string e){
        error = 1;
    }
    freeaddrinfo(host_info_list);
}

Client::~Client(){
    close(sockfd);
}

int Client::getError(){
    return error;
}

int Client::getFD(){
    return sockfd;
}

std::vector<char> Client::recvServerResponse(){
    return recvAll(sockfd);
}

// check send all
void Client::send(const std::vector<char> &msg){
    size_t sent = 0;
    size_t len = msg.size();
    size_t max = msg.size();
    while (sent < len) {
        sent = len - sent;
        len = sent;
        if (sendAll(sockfd, &msg.data()[max - len], &sent) == -1)
            throw std::string("send failed");
    }
}