#include "log.h"

// save log to local file
void Log::save(std::string msg){
    std::ofstream ofs;
    ofs.open("/var/log/erss/proxy.log", std::ios::out | std::ios::app);
    ofs << msg;
    ofs.close();
}

Log::Log(){
    requestid = -1;
}

Log::Log(int id){
    requestid = id;
}

Log::~Log(){}

void Log::newRequest(std::string statusLine, std::string clientIP){
    auto now = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(now);
    char* time = ctime(&t);
    std::string msg = std::to_string(requestid) + ": " + statusLine + " from " + clientIP + " @ " +std::string(time) + "\n";
    save(msg);
}

void Log::checkCache(int status, std::string expiredTime){
    std::string msg = std::to_string(requestid) + ": ";
    switch (status)
    {
    case NOTINCACHE:
        msg += "not in cache\n";
        break;
    case EXPIRED:
        msg += "in cache, but already expired at" + expiredTime;
        break;
    case NEEDVALIDATE:
        msg += "in cache, but need validate\n";
        break;
    case VALID:
        msg += "in cache and is valid";
        break;
    default:
        break;
    }
    save(msg);
}

void Log::reqFromServer(std::string statusLine, std::string serverName){
    std::string msg = std::to_string(requestid) + ": requesting" + statusLine + " from " + serverName + "\n";
    save(msg);
}

void Log::recvFromServer(std::vector<char> statusText, std::string serverName){
    std::string msg = std::to_string(requestid) + ": received" + std::string(statusText.begin(), statusText.end()) + " from " + serverName + "\n";
    save(msg);    
}

void Log::respond2Client(std::vector<char> statusText){
    if(statusText.empty())
        return;
    std::vector<char> pattern{'\r', '\n', '\r', '\n'};
    auto it = std::search(statusText.begin(), statusText.end(), pattern.begin(), pattern.end());
    if(it != statusText.end())
        statusText.erase(it, it + 4);
    std::string msg = std::to_string(requestid) + ": respongding to client " + std::string(statusText.begin(), statusText.end()) + "\n";
    save(msg); 
}

void Log::notCacheable(std::string reason){
    std::string msg = std::to_string(requestid) + ": not cacheable because of " + reason + "\n";
    save(msg);
}

void Log::cached(std::string expireDate){
    std::string msg = std::to_string(requestid) + ": cached, will expire at " + expireDate + "\n";
    save(msg);
}

void Log::cachedNeedRevalid(){
    std::string msg = std::to_string(requestid) + ": cached, but need revalidation\n";
    save(msg);
}

void Log::closeTunnel(){
    std::string msg = std::to_string(requestid) + ": close tunnel\n";
    save(msg);
}