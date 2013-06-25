#ifndef WEBDRIVER_COMMANDS_XDRPC_COMMAND_H_
#define WEBDRIVER_COMMANDS_XDRPC_COMMAND_H_

#include <string>
#include <vector>

#include "commands/command.h"

namespace webdriver {

class Response;

class XDRPCCommand : public Command {
public:
    XDRPCCommand(const std::vector<std::string>& path_segments,
                 const DictionaryValue* const parameters);
    virtual ~XDRPCCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    static std::string getSessionId(const std::string& url);

    DISALLOW_COPY_AND_ASSIGN(XDRPCCommand);
};

}  // namespace webdriver

#endif // WEBDRIVER_COMMANDS_XDRPC_COMMAND_H_
