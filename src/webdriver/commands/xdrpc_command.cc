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

#include "base/json/json_reader.h"
#include "base/string_split.h"
#include "commands/response.h"
#include "commands/xdrpc_command.h"
#include "webdriver_route_table.h"
#include "webdriver_server.h"

#include <string>

namespace webdriver {

XDRPCCommand::XDRPCCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

XDRPCCommand::~XDRPCCommand() {}

bool XDRPCCommand::DoesPost() const {
    return true;
}

void XDRPCCommand::ExecutePost(Response* const response) {
    std::string method;
    GetStringParameter("method", &method);

    std::string path;
    GetStringParameter("path", &path);

    path = path.substr(Server::GetInstance()->url_base().length());

    std::vector<std::string> path_segments;
    base::SplitString(path, '/', &path_segments);

    const DictionaryValue* parameters = NULL;
    GetDictionaryParameter("data", &parameters);
    parameters = parameters->DeepCopy();

    std::string matched_route;
    AbstractCommandCreator* cmdCreator =
        Server::GetInstance()->GetRouteTable().GetRouteForURL(path, &matched_route);
    if (NULL == cmdCreator)
    {
        response->SetError(new Error(
            kBadRequest, "no route for url: " + path));
        return;
    }

    Command* command = cmdCreator->create(path_segments, parameters);

    Server::GetInstance()->DispatchCommand(
        matched_route,
        command,
        method,
        response);
    response->SetStatus(kSuccess);

    if (response->GetValue()->IsType(Value::TYPE_STRING)) {
        std::string value;
        response->GetValue()->GetAsString(&value);
        std::string sessionId = getSessionId(value);
        if (!sessionId.empty()) {
            response->SetField("sessionId", new StringValue(sessionId));
        }
    }
}

std::string XDRPCCommand::getSessionId(const std::string& url) {
    if (url.find("/session/") == 0) {
        std::string sessionId = url.substr(std::string("/session/").size());
        if (sessionId.find('/') != std::string::npos) {
            sessionId = sessionId.substr(0, sessionId.find('/'));
        }
        return sessionId;
    } else {
        return "";
    }
}

}  // namespace webdriver

