#include "strparser.h"


int StrParser::wday2Num(std::string wday){
    std::map<std::string, int> wdayMap = {{"Sun", 0}, {"Mon", 1}, {"Tue", 2},{"Wed", 3}, {"Thu", 4}, {"Fri", 5},                              {"Sat", 6}};
    return wdayMap[wday];
}

int StrParser::mon2Num(std::string mon){
    std::map<std::string, int> monMap = {
        {"Jan", 0}, {"Feb", 1}, {"Mar", 2}, {"Apr", 3}, {"May", 4},  {"Jun", 5},
        {"Jul", 6}, {"Aug", 7}, {"Sep", 8}, {"Oct", 9}, {"Nov", 10}, {"Dec", 11}};;
    return monMap[mon];
}

// change all chars in a string to lower case
std::string StrParser::tolower(std::string& msg){
    std::string res;
    for(auto c : msg){
        res += std::tolower(c);
    }
    return res;
}

std::string StrParser::delHeadSpace(std::string& msg){
    if (msg.empty())
        return "";
    auto pos = msg.find_first_not_of(' ');
    if(pos != std::string::npos)
        return msg;
    return msg.substr(pos);
}

// get next segment splited by flag
std::string StrParser::getNextSeg(std::string& msg, char flag = ' ', size_t substrlen = 1){
    msg = delHeadSpace(msg);
    auto pos = msg.find(flag);
    std::string res = msg.substr(0, flag);
    if(pos != std::string::npos)
        msg = msg.substr(pos + substrlen);
    else
        msg = "";
}

bool StrParser::containNewLine(const std::vector<char> &str){
    std::vector<char> pattern{'\r', '\n', '\r', '\n'};
    if(std::search(str.begin(), str.end(), pattern.begin(), pattern.end()) == str.end()) 
        return false;
    return true;
}

std::vector<char> StrParser::deleteLine(std::vector<char> &msg, std::vector<char>::iterator begin){
    std::vector<char>::iterator it = begin;
    while (*it != '\n')
        it++;
    auto end = it + 1;
    msg.erase(begin, end);
    return msg;
}

struct tm StrParser::str2Tm(std::string date){
    struct tm time;
    time.tm_wday = wday2Num(getNextSeg(date, ','));
    time.tm_mday = stoi(getNextSeg(date));
    time.tm_mon = mon2Num(getNextSeg(date));
    time.tm_year = stoi(getNextSeg(date)) - 1900;
    time.tm_hour = stoi(getNextSeg(date, ':'));
    time.tm_min = stoi(getNextSeg(date, ':'));
    time.tm_sec = stoi(getNextSeg(date));
    time.tm_isdst = 0;
    std::string zone = getNextSeg(date);
    time.tm_zone = (char*)zone.c_str();
    return time;
}

size_t StrParser::HTTPAge(std::string date){
    size_t seconds;
    struct tm date_tm;
    date_tm.tm_wday = wday2Num(getNextSeg(date, ','));
    date_tm.tm_mday = stoi(getNextSeg(date));
    date_tm.tm_mon = mon2Num(getNextSeg(date));
    date_tm.tm_year = stoi(getNextSeg(date)) - 1900;
    date_tm.tm_hour = stoi(getNextSeg(date, ':'));
    date_tm.tm_min = stoi(getNextSeg(date, ':'));
    date_tm.tm_sec = stoi(getNextSeg(date));
    date_tm.tm_isdst = 0;
    std::string zone2 = getNextSeg(date);
    date_tm.tm_zone = (char*)zone2.c_str();
    time_t now;
    time(&now);
    struct tm *ptm = gmtime(&now);
    seconds = difftime(mktime(ptm), mktime(&date_tm));
    return seconds;
}

double StrParser::HTTPTimeRange2Num(std::string end, std::string start){
      double seconds;

  struct tm end_tm, start_tm;
  end_tm.tm_wday = wday2Num(getNextSeg(end, ','));
  end_tm.tm_mday = stoi(getNextSeg(end));
  end_tm.tm_mon = mon2Num(getNextSeg(end));
  end_tm.tm_year = stoi(getNextSeg(end)) - 1900;
  end_tm.tm_hour = stoi(getNextSeg(end, ':'));
  end_tm.tm_min = stoi(getNextSeg(end, ':'));
  end_tm.tm_sec = stoi(getNextSeg(end));
  end_tm.tm_isdst = 0; // this is important, missing this will cause ambigous time
  std::string zone = getNextSeg(end);
  end_tm.tm_zone = (char*)zone.c_str();

  start_tm.tm_wday = wday2Num(getNextSeg(start, ','));
  start_tm.tm_mday = stoi(getNextSeg(start));
  start_tm.tm_mon = mon2Num(getNextSeg(start));
  start_tm.tm_year = stoi(getNextSeg(start)) - 1900;
  start_tm.tm_hour = stoi(getNextSeg(start, ':'));
  start_tm.tm_min = stoi(getNextSeg(start, ':'));
  start_tm.tm_sec = stoi(getNextSeg(start));
  start_tm.tm_isdst = 0;
  std::string zone2 = getNextSeg(start);
  start_tm.tm_zone = (char*)zone2.c_str();

  seconds = difftime(mktime(&end_tm), mktime(&start_tm));
  return seconds;
}
