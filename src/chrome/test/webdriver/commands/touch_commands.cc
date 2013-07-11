#include "touch_commands.h"
#include "chrome/test/webdriver/commands/response.h"
#include "chrome/test/webdriver/webdriver_session.h"

namespace webdriver {


TouchCommand::TouchCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters) {}

TouchCommand::~TouchCommand() {}

bool TouchCommand::DoesPost() {
  return true;
}

TouchClickCommand::TouchClickCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchClickCommand::~TouchClickCommand() {}

void TouchClickCommand::ExecutePost(Response* const response) {
    Point location;
    Error* error;
    std::string element_name;

    if (!GetStringParameter("element", &element_name))
    {
        response->SetError(new Error(
            kBadRequest, "Invalid command arguments"));
    }

    ElementId element = ElementId(element_name);

    error = session_->GetElementLocationInView(element, &location);
    if (error) {
      response->SetError(error);
      return;
    }

    error = session_->TouchClick(location);
    if (error) {
        response->SetError(error);
        return;
    }
}

TouchDoubleClickCommand::TouchDoubleClickCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchDoubleClickCommand::~TouchDoubleClickCommand() {}


void TouchDoubleClickCommand::ExecutePost(Response *const response)
{
    Point location;
    Error* error;
    std::string element_name;

    if (!GetStringParameter("element", &element_name))
    {
        response->SetError(new Error(
            kBadRequest, "Invalid command arguments"));
    }

    ElementId element = ElementId(element_name);

    error = session_->GetElementLocationInView(element, &location);
    if (error) {
      response->SetError(error);
      return;
    }

    error = session_->TouchDoubleClick(location);
    if (error) {
        response->SetError(error);
        return;
    }
}

TouchDownCommand::TouchDownCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchDownCommand::~TouchDownCommand() {}

void TouchDownCommand::ExecutePost(Response *const response)
{
    Error* error;
    int x, y;

    if (!GetIntegerParameter("x", &x) ||
        !GetIntegerParameter("y", &y)) {
      response->SetError(new Error(
          kBadRequest,
          "Missing or invalid 'x' or 'y' parameters"));
      return;
    }

    Point location(x, y);


    error = session_->TouchDown(location);
    if (error) {
        response->SetError(error);
        return;
    }
}

TouchUpCommand::TouchUpCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchUpCommand::~TouchUpCommand() {}

void TouchUpCommand::ExecutePost(Response *const response)
{
    Error* error;
    int x, y;

    if (!GetIntegerParameter("x", &x) ||
        !GetIntegerParameter("y", &y)) {
      response->SetError(new Error(
          kBadRequest,
          "Missing or invalid 'x' or 'y' parameters"));
      return;
    }

    Point location(x, y);


    error = session_->TouchUp(location);
    if (error) {
        response->SetError(error);
        return;
    }
}

TouchMoveCommand::TouchMoveCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchMoveCommand::~TouchMoveCommand() {}

void TouchMoveCommand::ExecutePost(Response *const response)
{
    Error* error;
    int x, y;

    if (!GetIntegerParameter("x", &x) ||
        !GetIntegerParameter("y", &y)) {
      response->SetError(new Error(
          kBadRequest,
          "Missing or invalid 'x' or 'y' parameters"));
      return;
    }

    Point location(x, y);


    error = session_->TouchDown(location);
    if (error) {
        response->SetError(error);
        return;
    }
}

TouchScrollCommand::TouchScrollCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchScrollCommand::~TouchScrollCommand() {}

void TouchScrollCommand::ExecutePost(Response *const response)
{
    Error* error;
    int x, y;

    if (!GetIntegerParameter("xoffset", &x) ||
        !GetIntegerParameter("yoffset", &y)) {
      response->SetError(new Error(
          kBadRequest,
          "Missing or invalid 'xoffset' or 'yoffset' parameters"));
      return;
    }

    Point offset(x, y);

    std::string element_name;
    bool has_element_ = GetStringParameter("element", &element_name);

    if (has_element_) {
        ElementId element = ElementId(element_name);
        Point location;
        error = session_->GetElementLocationInView(element, &location);
        if (error) {
            response->SetError(error);
            return;
        }

        error = session_->TouchScroll(element, offset);
    }
    else
    {
        error = session_->TouchScroll(offset);
    }

    if (error) {
        response->SetError(error);
        return;
    }

}

TouchLongClickCommand::TouchLongClickCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchLongClickCommand::~TouchLongClickCommand() {}

void TouchLongClickCommand::ExecutePost(Response *const response)
{
    Error* error;
    Point location;
    std::string element_name;

    if (!GetStringParameter("element", &element_name))
    {
        response->SetError(new Error(
            kBadRequest, "Invalid command arguments"));
    }

    ElementId element = ElementId(element_name);

    error = session_->GetElementLocationInView(element, &location);
    if (error) {
      response->SetError(error);
      return;
    }

    error = session_->TouchLongClick(location);
    if (error) {
        response->SetError(error);
        return;
    }

}

TouchFlickCommand::TouchFlickCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : TouchCommand(path_segments, parameters) {
}

TouchFlickCommand::~TouchFlickCommand() {}

void TouchFlickCommand::ExecutePost(Response *const response)
{
    Error* error;
    int xSpeed, ySpeed;
    int xoffset, yoffset, speed;
    std::string element_name;

    if (GetIntegerParameter("xspeed", &xSpeed) &&
        GetIntegerParameter("yspeed", &ySpeed))
    {
        error = session_->TouchFlick(xSpeed, ySpeed);
    }
    else if (GetStringParameter("element", &element_name) && GetIntegerParameter("xoffset", &xoffset) &&
             GetIntegerParameter("yoffset", &yoffset) && GetIntegerParameter("speed", &speed))
    {
        Point location(xoffset, yoffset);
        ElementId element = ElementId(element_name);
        error = session_->TouchFlick(element, location, speed);
    }
    else
    {
        response->SetError(new Error(
            kBadRequest,
            "Missing or invalid parameters"));
        return;
    }

    if (error) {
        response->SetError(error);
        return;
    }

}


}

