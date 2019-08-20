#include <iostream>
#include <thread>

#include "proxy.h"

// copy from class Cache
void createIndex(std::string path){
    std::string dir;
    StrParser strparser;
    while(!path.empty()){
        dir += strparser.getNextSeg(path, '/');
        if(!path.empty()){
            mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            dir += "/";
        }
    }
}

void proxyFunc(std::pair<int, int> *args){
    int newfd = args->first;
    int requestid = args->second;
    Proxy proxy(requestid);
    proxy.handler(newfd);
    close(newfd);
}



/* main function */
int main(int argc, char **argv){
    if (argc != 2){
        std::cerr << "Usage: HTTPCachingProxy <port>\n";
        exit(EXIT_FAILURE);
    }
    int requestid = 0;
    Proxy proxy(requestid);
    createIndex(PATH);
    // become a daemon
    daemon(0, 0);
    umask(0);
    pid_t pid = fork();
    if(pid < 0)
        std::cerr << "fail to fork" << std::endl;
    while(true){
        int newfd = proxy.acNewRequest();
        std::pair<int, int> *args = new std::pair<int, int>(newfd, requestid++);
        std::thread thrd = std::thread(proxyFunc, args);
        thrd.detach();
    }
    return EXIT_SUCCESS;
}