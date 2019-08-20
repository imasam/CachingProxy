    #include "httprspnsparser.h"
    
    void HTTPRSPNSParser::parseStatus(std::string statusline){
        protocol = strparser.getNextSeg(statusline, '/');
        version_major = stoi(strparser.getNextSeg(statusline, '.'));
        version_minor = stoi(strparser.getNextSeg(statusline));
        status_code = stoi(strparser.getNextSeg(statusline));
        status_text = statusline;
    }

    void HTTPRSPNSParser::parseHeader(std::string head){
        while(!head.empty()){
            std::string key = strparser.tolower(strparser.getNextSeg(head, ':')); // problem here
            std::string val = strparser.getNextSeg(head, '\r', 2);
            headers[key] = val;
        }
    }

    // get the age of a cached HTTP response
    size_t HTTPRSPNSParser::getAge(){
        size_t age = strparser.HTTPAge(headers["date"]);
        return age;
    }

    // get the Max lifetime of a cached HTTP response
    size_t HTTPRSPNSParser::getMaxAge(){
        size_t age;
        // check "cache-control"
        if(headers.count("cache-control")){
            size_t target;
            if(target = headers["cache-control"].find("s-maxage") != std::string::npos){
                std::string age_str = headers["cache-control"].substr(target);
                int i = 0;
                while(i < age_str.size() && age_str[i] != ' ')
                    i++;
                age = stoi(age_str.substr(9, i));
                return age;
            }
            else if(target = headers["cache-control"].find("maxage") != std::string::npos){
                std::string age_str = headers["cache-control"].substr(target);
                int i = 0;
                while(i < age_str.size() && age_str[i] != ' ')
                    i++;
                age = stoi(age_str.substr(8, i));
                return age;
            }
        }
        // check "expires"
        if(headers.count("expires") && headers.count("date"))
            age = strparser.HTTPTimeRange2Num(headers["expires"], headers["date"]);
        // infer the age
        else if(headers.count("last-modified") && headers.count("date")){
            age = strparser.HTTPTimeRange2Num(headers["date"], headers["last-modified"]);
            age /= 10;
        }
        return age;
    }

    // update age field in cached response
    void HTTPRSPNSParser::updateAgeField(){
        // delete old age
        std::vector<char> pattern = {'A', 'g', 'e', ':'};
        auto begin = std::search(HTTPResponse_char.begin(), HTTPResponse_char.end(), pattern.begin(), pattern.end());
        if (begin != HTTPResponse_char.end())
            HTTPResponse_char = strparser.deleteLine(HTTPResponse_char, begin);
        // generate new age
        std::stringstream ss;
        ss << "Age: " << getAge() << "\r\n";
        std::string age = ss.str();
        // add new age
        pattern = {'\r', '\n', '\r', '\n'};
        auto it = std::search(HTTPResponse_char.begin(), HTTPResponse_char.end(), pattern.begin(), pattern.end());
        it += 2;
        HTTPResponse_char.insert(it, age.begin(), age.end());
    }
    
    HTTPRSPNSParser::HTTPRSPNSParser(std::vector<char> response){
        if (response.empty())
            throw std::string("no response");
        HTTPResponse = response.data();
        HTTPResponse_char = response;
        int target = HTTPResponse.find("\r\n");
        std::string statusline = HTTPResponse.substr(0, target);
        parseStatus(statusline);
        int head_end = HTTPResponse.find("\r\n\r\n");
        std::string head = HTTPResponse.substr(target + 2, head_end - target - 2);
        parseHeader(head);
    }

    size_t HTTPRSPNSParser::getStatusCode(){
        return status_code;
    }

    bool HTTPRSPNSParser::mustRevalidate(){
        std::string ctrl = headers["cache-controle"];
        if(ctrl.find("must-revalidation") != std::string::npos || ctrl.find("proxy-revalidation") != std::string::npos)
            return true;
        return false;
    }

    bool HTTPRSPNSParser::not_expire(){
        size_t maxage = getMaxAge();
        size_t age = getAge();
        return maxage > age;
    }

    std::string HTTPRSPNSParser::expiresAt(){
        struct tm born = strparser.str2Tm(headers["date"]);
        struct tm expires = born;
        expires.tm_sec += getMaxAge();
        time_t t = mktime(&expires);
        char *time = ctime(&t);
        return std::string(time);
    }

    bool HTTPRSPNSParser::stillfresh(){
        return not_expire() && !mustRevalidate();
    }

    std::vector<char> HTTPRSPNSParser::getResponse(){
        updateAgeField();
        return HTTPResponse_char;
    }

    std::string HTTPRSPNSParser::getLastModified(){
        if (!headers.count("last-modified"))
            return "";
        return headers["last-modified"];
    }

    std::string HTTPRSPNSParser::getETag(){
        if (!headers.count("etag"))
            return "";
        return headers["etag"];
    }

    std::vector<char> HTTPRSPNSParser::getStatusText(){
        return std::vector<char>(status_text.begin(), status_text.end());
    }
    
    bool HTTPRSPNSParser::good4Cache(){
        if(!headers.count("cache-control"))
            return true;
        std::string ctlPolicy = headers["cache-control"];
        if (ctlPolicy.find("private") != std::string::npos ||
            ctlPolicy.find("no-cache") != std::string::npos ||
            ctlPolicy.find("no-store") != std::string::npos)
            return false;
        return true;
    }

    std::string HTTPRSPNSParser::whyBad4Cache(){
        std::string ctlPolicy = headers["cache-control"];
        if (ctlPolicy.find("private") != std::string::npos)
            return "private policy of http response";
        if (ctlPolicy.find("no-cache") != std::string::npos)
            return "no-cache policy of http response";
        if (ctlPolicy.find("no-store") != std::string::npos)
            return "no-store policy of http response";
        return "max cache time is 0";
    }