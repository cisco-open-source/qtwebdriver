#include "commands/visualizer_commands.h"

#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

VisualizerSourceCommand::VisualizerSourceCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

VisualizerSourceCommand::~VisualizerSourceCommand() {}

bool VisualizerSourceCommand::DoesGet() const {
    return true;
}

void VisualizerSourceCommand::ExecuteGet(Response* const response) {
    std::string page_source;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::VisualizerSource,
            base::Unretained(executor_.get()),
            &page_source,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(new StringValue(page_source));
}

VisualizerShowPointCommand::VisualizerShowPointCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

VisualizerShowPointCommand::~VisualizerShowPointCommand() {}

bool VisualizerShowPointCommand::DoesGet() const {
    return true;
}

void VisualizerShowPointCommand::ExecuteGet(Response* const response) {
    std::string page_source;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::VisualizerShowPoint,
            base::Unretained(executor_.get()),
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(new StringValue(page_source));
}

}  // namespace webdriver
