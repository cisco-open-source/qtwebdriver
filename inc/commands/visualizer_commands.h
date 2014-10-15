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
