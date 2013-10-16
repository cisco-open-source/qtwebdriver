#ifndef WEBDRIVER_ACCESS_H
#define WEBDRIVER_ACCESS_H

#include <string>
#include <vector>
#include <list>
#include "base/file_util.h"

namespace webdriver {

struct AccessCommandTable
{
    std::string method;
    std::string url;
};

struct AccessRule {
    long hostIp;
    bool isGeneralRule; //for all ip
    bool allowed;
    std::vector<AccessCommandTable> commandList;
};

class AccessValidator
{
public:
    AccessValidator();
    ~AccessValidator();
    void setWhiteList(FilePath &xmlPath);
    bool isAllowed(const long &remote_ip, const std::string &url, const std::string &method);

private:
    bool convertIpString(const char *str_ip, long *int_ip);
    std::list<AccessRule> accessList;
};

}  // namespace webdriver

#endif // WEBDRIVER_ACCESS_H
