#ifndef VISUALIZER_COMMANDS_H
#define VISUALIZER_COMMANDS_H

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Get the page source with reference artifacts assembled inside.
class VisualizerSourceCommand : public ViewCommand {
public:
    VisualizerSourceCommand(const std::vector<std::string>& path_segments,
                            const base::DictionaryValue* const parameters);
    virtual ~VisualizerSourceCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(VisualizerSourceCommand);
};

class VisualizerShowPointCommand : public ViewCommand {
public:
    VisualizerShowPointCommand(const std::vector<std::string>& path_segments,
                               const base::DictionaryValue* const parameters);
    virtual ~VisualizerShowPointCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(VisualizerShowPointCommand);
};

}  // namespace webdriver

#endif // VISUALIZER_COMMANDS_H
