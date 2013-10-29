// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_WEBELEMENT_COMMANDS_H_
#define WEBDRIVER_COMMANDS_WEBELEMENT_COMMANDS_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"
#include "webdriver_element_id.h"

namespace base {
class DictionaryValue;
}

namespace gfx {
    class Point;
}

namespace webdriver {

class Error;
class Response;

/// Handles commands that interact with a web element in the WebDriver REST
/// service.
class ElementCommand : public ViewCommand {
public:
    ElementCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~ElementCommand();

    virtual bool Init(Response* const response) OVERRIDE;

protected:
    const std::vector<std::string>& path_segments_;
    ElementId element;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementCommand);
};

/// Retrieves element attributes.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/attribute/:name
class ElementAttributeCommand : public ElementCommand {
public:
    ElementAttributeCommand(const std::vector<std::string>& path_segments,
                          const base::DictionaryValue* parameters);
    virtual ~ElementAttributeCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementAttributeCommand);
};

/// Clears test input elements.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/clear
class ElementClearCommand : public ElementCommand {
public:
    ElementClearCommand(const std::vector<std::string>& path_segments,
                      const base::DictionaryValue* parameters);
    virtual ~ElementClearCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementClearCommand);
};

/// Retrieves element style properties.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/css/:propertyName
class ElementCssCommand : public ElementCommand {
public:
    ElementCssCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* parameters);
    virtual ~ElementCssCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementCssCommand);
};

/// Queries whether an element is currently displayed ot the user.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/displayed
class ElementDisplayedCommand : public ElementCommand {
public:
    ElementDisplayedCommand(const std::vector<std::string>& path_segments,
                          const base::DictionaryValue* parameters);
    virtual ~ElementDisplayedCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementDisplayedCommand);
};

/// Queries whether an element is currently enabled.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/enabled
class ElementEnabledCommand : public ElementCommand {
public:
    ElementEnabledCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~ElementEnabledCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementEnabledCommand);
};

/// Queries whether two elements are equal.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/equals/:other
class ElementEqualsCommand : public ElementCommand {
public:
    ElementEqualsCommand(const std::vector<std::string>& path_segments,
                       const base::DictionaryValue* parameters);
    virtual ~ElementEqualsCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementEqualsCommand);
};

/// Retrieves the element's location on the page.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/location
class ElementLocationCommand : public ElementCommand {
public:
    ElementLocationCommand(const std::vector<std::string>& path_segments,
                         const base::DictionaryValue* parameters);
    virtual ~ElementLocationCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementLocationCommand);
};

/// Retrieves the element's location on the page after ensuring it is visible in
/// the current viewport.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/location_in_view
class ElementLocationInViewCommand : public ElementCommand {
public:
    ElementLocationInViewCommand(const std::vector<std::string>& path_segments,
                               const base::DictionaryValue* parameters);
    virtual ~ElementLocationInViewCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementLocationInViewCommand);
};

/// Queries for an element's tag name.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/name
class ElementNameCommand : public ElementCommand {
public:
    ElementNameCommand(const std::vector<std::string>& path_segments,
                     const base::DictionaryValue* parameters);
    virtual ~ElementNameCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementNameCommand);
};

/// Handles selecting elements and querying whether they are currently selected.
/// Queries whether an element is currently enabled.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/selected
class ElementSelectedCommand : public ElementCommand {
public:
    ElementSelectedCommand(const std::vector<std::string>& path_segments,
                         const base::DictionaryValue* parameters);
    virtual ~ElementSelectedCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementSelectedCommand);
};

/// Queries for an element's size.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/size
class ElementSizeCommand : public ElementCommand {
public:
    ElementSizeCommand(const std::vector<std::string>& path_segments,
                     const base::DictionaryValue* parameters);
    virtual ~ElementSizeCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementSizeCommand);
};

/// Submit's the form containing the target element.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/submit
class ElementSubmitCommand : public ElementCommand {
public:
    ElementSubmitCommand(const std::vector<std::string>& path_segments,
                       const base::DictionaryValue* parameters);
    virtual ~ElementSubmitCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementSubmitCommand);
};

/// Sends keys to the specified web element. Also gets the value property of an
/// element.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/value
class ElementValueCommand : public ElementCommand {
public:
    ElementValueCommand(const std::vector<std::string>& path_segments,
                      const base::DictionaryValue* parameters);
    virtual ~ElementValueCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementValueCommand);
};

/// Gets the visible text of the specified web element.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/text
class ElementTextCommand : public ElementCommand {
public:
    ElementTextCommand(const std::vector<std::string>& path_segments,
                     const base::DictionaryValue* parameters);
    virtual ~ElementTextCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementTextCommand);
};

class ElementScreenshotCommand : public ElementCommand {
public:
    ElementScreenshotCommand(const std::vector<std::string>& path_segments,
                          const base::DictionaryValue* parameters);
    virtual ~ElementScreenshotCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ElementScreenshotCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_WEBELEMENT_COMMANDS_H_
