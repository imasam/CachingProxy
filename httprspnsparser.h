/* parse http response to get PROTOCOL, VERSION, STATUS CODE, STATUS TEXT, HEADERS and BODY */

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <time.h>
/* parse http response to get PROTOCOL,VERSION, STATUS CODE, STATUS TEXT, HEADERS and BODY */

#include <vector>

#include "strparser.h"

class HTTPRSPNSParser{
private:
    StrParser strparser;
    std::string HTTPResponse;
    std::vector<char> HTTPResponse_char;
    std::string protocol;
    unsigned short version_major;
    unsigned short version_minor;
    size_t status_code;
    std::string status_text;
    std::map<std::string, std::string> headers;
    void parseStatus(std::string statusline);
    void parseHeader(std::string head);
    size_t getAge();
    size_t getMaxAge();
    void updateAgeField();

public:
    HTTPRSPNSParser(std::vector<char> response);
    size_t getStatusCode();
    bool mustRevalidate();
    bool not_expire();
    std::string expiresAt();
    bool stillfresh();
    std::vector<char> getResponse();
    std::string getLastModified();
    std::string getETag();
    std::vector<char> getStatusText();
    bool good4Cache();
    std::string whyBad4Cache();
};