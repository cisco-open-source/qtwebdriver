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

#include "webdriver_access.h"
#include "webdriver_logging.h"
#include <base/string_util.h>
#include "third_party/pugixml/pugixml.hpp"

namespace webdriver {

AccessValidator::AccessValidator(){
}

AccessValidator::~AccessValidator(){
}

bool AccessValidator::isAllowed(const long &remote_ip, const std::string &url, const std::string &method)
{
    if (accessList.empty())
        return true;
    bool result = false;
    for (std::list<AccessRule>::iterator it = accessList.begin(); it != accessList.end(); ++it)
    {
        AccessRule host = *it;
        if ((host.hostIp == remote_ip) || (host.isGeneralRule))
        {
            if (!host.allowed) {
                for (std::vector<AccessCommandTable>::iterator it = host.commandList.begin(); it != host.commandList.end(); ++it) {
                    AccessCommandTable table = *it;
                    if (MatchPattern(url, table.url) && MatchPattern(method, table.method))
                        return false;
                }
                return true;
            } else {
                if (host.commandList.empty())
                    return true;
                for (std::vector<AccessCommandTable>::iterator it = host.commandList.begin(); it != host.commandList.end(); ++it) {
                    AccessCommandTable table = *it;
                    if (MatchPattern(url, table.url) && MatchPattern(method, table.method))
                        return true;
                }
                return false;
            }
        }
    }
    return result;
}

void AccessValidator::setWhiteList(FilePath &xmlPath)
{
    std::string white_list;

    if (file_util::ReadFileToString(xmlPath, &white_list))
    {
        const void* content = white_list.c_str();
        int content_size = white_list.size();

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(content, content_size);

        if (result) {
            // Select host nodes
            pugi::xpath_query query_nodes("/hosts/host");
            pugi::xpath_node_set found_nodes = query_nodes.evaluate_node_set(doc);

            if ( (NULL == query_nodes.result().error) &&
                 (pugi::xpath_type_node_set == query_nodes.return_type()) ) {
                for (pugi::xpath_node_set::const_iterator it = found_nodes.begin(); it != found_nodes.end(); ++it) {
                    pugi::xpath_node node = *it;
                    pugi::xml_node xnode = node.node();
                    pugi::xml_attribute atr = xnode.attribute("ip");

                    AccessRule rule;
                    rule.isGeneralRule = false;
                    rule.allowed = true;

                    if (!convertIpString(atr.value(), &rule.hostIp)) {
                        if (!strcmp(atr.value(), "*")) {
                            rule.isGeneralRule = true;
                        } else {
                            std::string error_descr = "WhiteList: "+ std::string(atr.value()) + " is not a valid ip address";
                            GlobalLogger::Log(kWarningLogLevel, error_descr);
                            continue;
                        }

                    }

                    pugi::xpath_query query_nodes("./deny");
                    pugi::xpath_node_set deny_nodes = query_nodes.evaluate_node_set(xnode);
                    if ( (NULL == query_nodes.result().error) &&
                         (pugi::xpath_type_node_set == query_nodes.return_type()) ) {
                        for (pugi::xpath_node_set::const_iterator it = deny_nodes.begin(); it != deny_nodes.end(); ++it) {
                            pugi::xpath_node node = *it;
                            pugi::xml_node xnode = node.node();
                            pugi::xml_attribute atr = xnode.attribute("url");

                            AccessCommandTable rt;
                            rt.url = atr.value();
                            atr = xnode.attribute("method");
                            rt.method = atr.value();
                            rule.commandList.push_back(rt);
                            rule.allowed = false;
                        }
                    }
                    if (rule.allowed)
                    {
                        pugi::xpath_query query_nodes("./allow");
                        pugi::xpath_node_set allow_nodes = query_nodes.evaluate_node_set(xnode);
                        if ( (NULL == query_nodes.result().error) &&
                             (pugi::xpath_type_node_set == query_nodes.return_type()) ) {
                            for (pugi::xpath_node_set::const_iterator it = allow_nodes.begin(); it != allow_nodes.end(); ++it) {
                                pugi::xpath_node node = *it;
                                pugi::xml_node xnode = node.node();
                                pugi::xml_attribute atr = xnode.attribute("url");
                                AccessCommandTable rt;
                                rt.url = atr.value();
                                atr = xnode.attribute("method");
                                rt.method = atr.value();
                                rule.commandList.push_back(rt);
                            }
                        }
                    }
                    // if we have wildcard put this rule in the end
                    rule.isGeneralRule ? accessList.push_back(rule) : accessList.push_front(rule);
                }
            } else {
                std::string error_descr = "WhiteList: Cant evaluate XPath to node set: ";
                error_descr += query_nodes.result().description();
                GlobalLogger::Log(kWarningLogLevel, error_descr);
            }
        }
        else
        {
            std::string error_descr = "   Error description: ";
            error_descr += result.description();
            GlobalLogger::Log(kWarningLogLevel, "WhiteList: XML parsed with errors:");
            GlobalLogger::Log(kWarningLogLevel, error_descr);
        }

        // destroy tree
        doc.reset();
    }
    else
    {
        GlobalLogger::Log(kWarningLogLevel, "WhiteList: Can't read file");
    }
}

bool AccessValidator::convertIpString(const char *str_ip, long *int_ip)
{
    bool result = false;
    char *p, c;
    long octet, n;

    *int_ip = 0;
    octet = 0;
    n = 0;

    char buff[sizeof "000.000.000.000"];
    if (strlen(str_ip) < sizeof buff) {
        strcpy(buff, str_ip);
    }

    for (p = buff; *p != '\0' ; ++p) {
        c = *p;
        if (c >= '0' && c <= '9') {
            octet = octet * 10 + (c - '0');
            continue;
        }
        if (c == '.' && octet < 256) {
            *int_ip = (*int_ip << 8) + octet;
            octet = 0;
            n++;
            continue;
        }
        return result;
    }

    if (n == 3 && octet < 256) {
        *int_ip = (*int_ip << 8) + octet;
        result = true;
    }
    return result;
}

}  // namespace webdriver
