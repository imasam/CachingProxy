#include "server.h"

// need double check here
std::vector<char> Server::recvAllRQST(int fd){
	std::vector<char> msg;
	size_t index = 0;
	int contentlen = 0;
	StrParser strparser;
	while(!strparser.containNewLine(msg)){
		if(msg.size() < index + MAXDATASIZE)
		msg.resize(index + MAXDATASIZE);
		int nbytes;
		if((nbytes = recv(fd, &msg.data()[index], MAXDATASIZE - 1, 0)) <= 0)
			return std::vector<char>();
		else
			index += nbytes;
	}
	msg.resize(index);
	HTTPRQSTParser httprqstparser(msg);
	contentlen = httprqstparser.getContentLen();
	if(contentlen != 0){
		std::vector<char> pattern{'\r', '\n', '\r', '\n'};
		auto it = std::search(msg.begin(), msg.end(), pattern.begin(), pattern.end()) + 4;
		while(it != msg.end()){
			--contentlen;
			++it;
		}
		while(contentlen){
			if(msg.size() < index + MAXDATASIZE)
				msg.resize(index + MAXDATASIZE);
			int nbytes;
			if((nbytes = recv(fd, &msg.data()[index], MAXDATASIZE - 1, 0)) <= 0)
				break;
			else{
				index += nbytes;
				contentlen -= nbytes;
			}
		}
	}
	msg.resize(index);
	return msg;
}

std::vector<char> Server::recvAllData(int fd){
    // set timeout for receive
    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)))
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

// send all data to socket
int Server::sendAll(int fd, const char *buf, size_t *len){
	size_t total = 0; // already sent
  	int left = *len;  // need to send
	int n;
	while (total < left){
		if (n = send(fd, buf + total, left, 0) == -1) // problem here
		break;
		total += n;
		left -= n;
	}
	*len = total;            // return already sent
	return n == -1 ? -1 : 0; // -1 : failure, 0 : success
}

Server::Server(){
	signal(SIGPIPE, SIG_IGN);
	listener = -1;
}

Server::~Server(){
	if (listener != -1)
		close(listener);
}

Server::Server(const char *p){
	port = p;
	signal(SIGPIPE, SIG_IGN);
	const char* hostname = NULL;
	addrinfo* host_info_list;

	memset(&host_info, 0, sizeof(host_info));	
	host_info.ai_family = AF_UNSPEC;
	host_info.ai_socktype = SOCK_STREAM;
	host_info.ai_flags = AI_PASSIVE;
	try{
		if(getaddrinfo(hostname, port, &host_info,&host_info_list))
            throw std::string("get addr info");
        listener = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
        if(listener == -1)
            throw std::string("socket");

		int yes = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		// set recv timeout
		if(bind(listener, host_info_list->ai_addr, host_info_list->ai_addrlen) == -1)
			throw std::string("bind");
		if(listen(listener, 100) == -1)
			throw std::string("listen");
	}catch(std::string e){
    	freeaddrinfo(host_info_list);
    	throw std::string("server init");
	}
	freeaddrinfo(host_info_list);
}

int Server::acNewConnect(){
	sockaddr_storage socket_addr;
	socklen_t socket_addr_len = sizeof(socket_addr);
	int newfd = accept(listener, (sockaddr *)&socket_addr, &socket_addr_len);
	if (newfd == -1)
		throw std::string("accept failed");
	return newfd;
}

std::vector<char> Server::basicRecv(int fd){
	int index = 0;
	std::vector<char> msg;
	msg.resize(MAXDATASIZE);
	index = recv(fd, &msg.data()[index], MAXDATASIZE - 1, 0);
	if (index == -1)
		throw std::string("receive failed");
	msg.resize(index);
	return msg;
}

std::vector<char> Server::recvHTTPRequest(int fd){
	return recvAllRQST(fd);
}

std::vector<char> Server::recvData(int fd){
 	 return recvAllData(fd);
}

void Server::send(int fd, const std::vector<char> &msg){
	size_t sent = 0;
	size_t len = msg.size();
	size_t max = msg.size();
	while (sent < len){
		sent = len - sent;
		len = sent;
		if (sendAll(fd, &msg.data()[max - len], &sent) == -1)
		throw std::string("send failed");
	}
}