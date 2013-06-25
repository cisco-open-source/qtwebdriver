#include "base/json/json_reader.h"
#include "base/string_split.h"
#include "commands/response.h"
#include "commands/xdrpc_command.h"
#include "webdriver_route_table.h"
#include "webdriver_server.h"
#include "webdriver_session_manager.h"

#include <string>

namespace webdriver {

XDRPCCommand::XDRPCCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

XDRPCCommand::~XDRPCCommand() {}

bool XDRPCCommand::DoesPost() {
    return true;
}

void XDRPCCommand::ExecutePost(Response* const response) {
    std::string method;
    GetStringParameter("method", &method);

    std::string path;
    GetStringParameter("path", &path);

    SessionManager* manager = SessionManager::GetInstance();
    path = path.substr(manager->url_base().length());

    std::vector<std::string> path_segments;
    base::SplitString(path, '/', &path_segments);

    const DictionaryValue* parameters = NULL;
    GetDictionaryParameter("data", &parameters);
    parameters = parameters->DeepCopy();

    std::string matched_route;
    AbstractCommandCreator* cmdCreator =
        Server::GetInstance()->getRouteTable().GetRouteForURL(path, &matched_route);

    Server::GetInstance()->DispatchCommand(
        cmdCreator->create(path_segments, parameters),
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

