/* parse http request to get METHOD, PATH, PROTOCOL, VERSION and HEADERS */

#include <map>
#include <string>
#include <vector>

#include "strparser.h"

class HTTPRQSTParser {
private:
  StrParser strparser;
  int errNum;
  std::string HTTPRequest;
  std::vector<char> HTTPRequest_char;
  std::string method;
  std::string protocol;
  std::string path;
  std::string statusline;
  unsigned short version_major;
  unsigned short version_minor;
  std::string host;
  std::string port;
  std::map<std::string, std::string> headers;
  void parseRequest(std::string request);
  void parseHeader(std::string head);
  int verifyHeader();
  std::string updateHTTPath(std::string &path);
  std::string updateHTTPRequest(std::string request);

public:
  HTTPRQSTParser(const std::vector<char> &r);
  ~HTTPRQSTParser();
  int errorDetection();
  std::string getHostName();
  std::string getHostPort();
  std::vector<char> getRequest();
  std::string getMethod();
  std::string getURL();
  std::string getStatusLine();
  int getContentLen();
  bool needRevalidate();
  bool good4Cache();
  std::string Bad4CacheInfo();
};