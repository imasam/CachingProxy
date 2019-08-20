// store/read GET response to/from local file system
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "strparser.h"

class Cache{
private:
    StrParser strparser;
    std::string cachename;
    std::string parseURL(std::string url);
    void createIndex(std::string path);

public:
    Cache();
    void store(const std::string &url, const std::vector<char> &msg);
    bool check(const std::string &url);
    std::vector<char> read(const std::string &url);
};