// includes funcations to parse string

#include <string>
#include <map>
#include <vector>

class StrParser{
public:
    int wday2Num(std::string wday);
    int mon2Num(std::string mon);

    std::string tolower(std::string& msg);
    std::string delHeadSpace(std::string& msg); 
    std::string getNextSeg(std::string& msg, char flag = ' ', size_t substrlen = 1);
    bool containNewLine(const std::vector<char> &str);
    std::vector<char> deleteLine(std::vector<char> &msg, std::vector<char>::iterator begin);

    struct tm str2Tm(std::string date);
    size_t HTTPAge(std::string date);
    double HTTPTimeRange2Num(std::string end, std::string start);
};