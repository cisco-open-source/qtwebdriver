#ifndef SHUTDOWN_COMMAND_H
#define SHUTDOWN_COMMAND_H

#include "commands/command.h"
#include "commands/response.h"

namespace webdriver {

class ShutdownCommand : public Command
{
public:
    ShutdownCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~ShutdownCommand();

    virtual bool DoesPost() const OVERRIDE;

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ShutdownCommand);
};

}

#endif // SHUTDOWN_COMMAND_H
