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
