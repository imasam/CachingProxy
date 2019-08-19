#include "cache.h"

std::string Cache::parseURL(std::string url){      
    size_t target;
    std::string path = cachename;
    if(target = url.find("://") != std::string::npos)
        url = url.substr(target + 3);
    int root = 1;
    while(!url.empty()){
        std::string dir = strparser.getNextSeg(url, '/');
        path += "/" + dir;
        if(url.empty() && dir.find('.') == std::string::npos)
            path += "/index.html";
        if (root && url.empty())
            path += "/index.html";
        root = 0;
    }
    return path;
}

void createIndex(std::string path){
    std::string dir;
    while(!path.empty()){
        dir += strparser.getNextSeg(path, '/');
        if(!path.empty()){
            mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            dir += "/";
        }
    }
}

Cache::Cache() {
  cachename = "/var/cache/proxycache";
  mkdir(cachename.c_str(),
        S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); // ignore fail here
}

void Cache::store(const std::string &url, const std::vector<char> &msg){
    std::ofstream ofs;
    std::string path = parseURL(url);
    createIndex(path);
    ofs.open(path, std::ofstream::out | std::ofstream::trunc);
    ofs << msg.data();
    ofs.close();
}

bool Cache::check(const std::string &url){
    std::string path = parseURL(url);
    struct stat buf;
    return stat(path.c_str(), &buf) == 0;
}

std::vector<char> Cache::read(const std::string &url){ 
    std::string path = parseURL(url);
    struct stat s;
    if(stat(path.c_str(), &s) == 0){
        if(s.st_mode & S_IFDIR)
            path += "/index.html";
    }
    std::ifstream ifs(path, std::ios::binary);
    std::vector<char> msg(std::istream_iterator<char>{ifs}, {});
    return msg;
}