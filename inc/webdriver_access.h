/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

/// \page page_whitelist IP/command whitelist
///Whitelist functionality allows to specify xml-file with list of IP with allowed/denied commands. To pass whitelist file to WebDriver need to use "white-list" server option(\ref page_webdriver_switches)
///\code
/// -white-list=/path/to/whitelist.xml
///\endcode
///If no whitelist xml-file defined then all commands for all IP is allowed.
///
///Example of whitelist.xml:
///\code
/// <?xml version="1.0" encoding="UTF-8"?>
/// <hosts>
///     <host ip="10.32.191.8"/>
///     <host ip="192.27.27.7">
///         <deny url="/session/*/window/*" method="*"/>
///         <deny url="/session/*/click" method="POST"/>
///     </host>
///     <host ip="192.34.191.7">
///         <allow url="/status" method="GET" />
///     </host>
/// </hosts>
///\endcode
///*In this example:
///*   - if nothing is specified (10.32.191.8) all command is allowed for that IP;
///*   - if "allow" node is specified only these commands is allowed, other disallowed;
///*   - if "deny" node is specified this command will be disallowed, all other allowed;
///*   - if there will be "allow" and "deny" nodes for one IP, only "deny" will have effect, all other will be allowed;
///*   - for all other IP all commands will be disallowed;
///
///To allow some command for any IP wildcard should be used:
///\code
/// <host ip="*">
///     <allow url="/status" method="GET" />
/// </host>
///\endcode
///
///
///There is <a href="whitelist.xsd" target="_blank"><b>xsd</b></a> file to validate/generate whitelist.xml


#ifndef WEBDRIVER_ACCESS_H
#define WEBDRIVER_ACCESS_H

#include <string>
#include <vector>
#include <list>
#include "base/file_util.h"

namespace webdriver {

/// Provides \ref page_whitelist functionality
class AccessValidator
{
public:
    AccessValidator();
    ~AccessValidator();

    ///Parse xml file with whitelist config
    ///@param xmlPath path to whitelist xml-file
    void setWhiteList(FilePath &xmlPath);

    ///Check if given command is allowed for this IP
    ///@param remote_ip origin IP to check
    ///@param url command url
    ///@param method command method
    bool isAllowed(const long &remote_ip, const std::string &url, const std::string &method);

private:

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

    bool convertIpString(const char *str_ip, long *int_ip);
    std::list<AccessRule> accessList;
};

}  // namespace webdriver

#endif // WEBDRIVER_ACCESS_H
