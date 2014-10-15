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

#ifndef TOUCH_COMMANDS_H
#define TOUCH_COMMANDS_H

#include <string>
#include <vector>

#include "commands/element_commands.h"
#include "webdriver_element_id.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

class TouchCommand : public ElementCommand {
public:
    TouchCommand(const std::vector<std::string>& path_segments,
                     const base::DictionaryValue* const parameters);
    virtual ~TouchCommand();

    virtual bool DoesPost() const OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchCommand);
};

/// Single tap on the touch enabled device.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/touch/click
class TouchClickCommand : public TouchCommand {
public:
    TouchClickCommand(const std::vector<std::string>& path_segments,
           const base::DictionaryValue* const parameters);
    virtual ~TouchClickCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchClickCommand);
};

/// Finger down on the screen.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/touch/down
class TouchDownCommand : public TouchCommand {
public:
    TouchDownCommand(const std::vector<std::string>& path_segments,
                const base::DictionaryValue* const parameters);
    virtual ~TouchDownCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchDownCommand);
};


/// Finger up on the screen.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/touch/up
class TouchUpCommand : public TouchCommand {
public:
    TouchUpCommand(const std::vector<std::string>& path_segments,
              const base::DictionaryValue* const parameters);
    virtual ~TouchUpCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchUpCommand);
};

/// Double tap on the touch screen using finger motion events.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#session/:sessionId/touch/doubleclick
class TouchDoubleClickCommand : public TouchCommand {
public:
    TouchDoubleClickCommand(const std::vector<std::string>& ps,
                 const base::DictionaryValue* const parameters);
    virtual ~TouchDoubleClickCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchDoubleClickCommand);
};

/// Finger move on the screen.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#session/:sessionId/touch/move
class TouchMoveCommand : public TouchCommand {
public:
    TouchMoveCommand(const std::vector<std::string>& path_segments,
              const base::DictionaryValue* const parameters);
    virtual ~TouchMoveCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchMoveCommand);
};

/// Scroll on the touch screen using finger based motion events.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#session/:sessionId/touch/scroll
class TouchScrollCommand : public TouchCommand {
public:
    TouchScrollCommand(const std::vector<std::string>& path_segments,
              const base::DictionaryValue* const parameters);
    virtual ~TouchScrollCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchScrollCommand);
};

/// Long press on the touch screen using finger motion events.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#session/:sessionId/touch/longclick
class TouchLongClickCommand : public TouchCommand {
public:
    TouchLongClickCommand(const std::vector<std::string>& path_segments,
              const base::DictionaryValue* const parameters);
    virtual ~TouchLongClickCommand();
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchLongClickCommand);
};

/// Flick on the touch screen using finger motion events.
/// https://code.google.com/p/selenium/wiki/JsonWireProtocol#session/:sessionId/touch/flick
class TouchFlickCommand : public TouchCommand {
public:
    TouchFlickCommand(const std::vector<std::string>& path_segments,
              const base::DictionaryValue* const parameters);
    virtual ~TouchFlickCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchFlickCommand);
};

/// Rotate element using pinch rotate fingers motion events.
class TouchPinchRotateCommand : public TouchCommand {
public:
    TouchPinchRotateCommand(const std::vector<std::string>& path_segments,
           const base::DictionaryValue* const parameters);
    virtual ~TouchPinchRotateCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchPinchRotateCommand);
};

/// Rotate element using pinch zoom fingers motion events.
class TouchPinchZoomCommand : public TouchCommand {
public:
    TouchPinchZoomCommand(const std::vector<std::string>& path_segments,
           const base::DictionaryValue* const parameters);
    virtual ~TouchPinchZoomCommand();

    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(TouchPinchZoomCommand);
};

}  // namespace webdriver

#endif // TOUCH_COMMANDS_H
