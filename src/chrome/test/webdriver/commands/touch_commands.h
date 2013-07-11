#ifndef TOUCH_COMMANDS_H
#define TOUCH_COMMANDS_H

#include <string>
#include <vector>

#include "chrome/test/webdriver/commands/webelement_commands.h"
#include "chrome/test/webdriver/webdriver_element_id.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

class TouchCommand : public WebDriverCommand {
 public:
  TouchCommand(const std::vector<std::string>& path_segments,
                       const base::DictionaryValue* const parameters);
  virtual ~TouchCommand();

  virtual bool DoesPost() OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchCommand);
};

//Single tap on the touch enabled device.
class TouchClickCommand : public TouchCommand {
 public:
  TouchClickCommand(const std::vector<std::string>& path_segments,
               const base::DictionaryValue* const parameters);
  virtual ~TouchClickCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(TouchClickCommand);
};

//Finger down on the screen.
class TouchDownCommand : public TouchCommand {
 public:
  TouchDownCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
  virtual ~TouchDownCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchDownCommand);
};


//Finger up on the screen.
class TouchUpCommand : public TouchCommand {
 public:
  TouchUpCommand(const std::vector<std::string>& path_segments,
                  const base::DictionaryValue* const parameters);
  virtual ~TouchUpCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchUpCommand);
};

//Double tap on the touch screen using finger motion events.
class TouchDoubleClickCommand : public TouchCommand {
 public:
  TouchDoubleClickCommand(const std::vector<std::string>& ps,
                     const base::DictionaryValue* const parameters);
  virtual ~TouchDoubleClickCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(TouchDoubleClickCommand);
};

//Finger move on the screen.
class TouchMoveCommand : public TouchCommand {
 public:
  TouchMoveCommand(const std::vector<std::string>& path_segments,
                  const base::DictionaryValue* const parameters);
  virtual ~TouchMoveCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchMoveCommand);
};

//Scroll on the touch screen using finger based motion events.
class TouchScrollCommand : public TouchCommand {
 public:
  TouchScrollCommand(const std::vector<std::string>& path_segments,
                  const base::DictionaryValue* const parameters);
  virtual ~TouchScrollCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchScrollCommand);
};

//Long press on the touch screen using finger motion events.
class TouchLongClickCommand : public TouchCommand {
 public:
  TouchLongClickCommand(const std::vector<std::string>& path_segments,
                  const base::DictionaryValue* const parameters);
  virtual ~TouchLongClickCommand();
  virtual void ExecutePost(Response* const response) OVERRIDE;

  DISALLOW_COPY_AND_ASSIGN(TouchLongClickCommand);
};

//Flick on the touch screen using finger motion events.
class TouchFlickCommand : public TouchCommand {
 public:
  TouchFlickCommand(const std::vector<std::string>& path_segments,
                  const base::DictionaryValue* const parameters);
  virtual ~TouchFlickCommand();

  virtual void ExecutePost(Response* const response) OVERRIDE;

 private:
  DISALLOW_COPY_AND_ASSIGN(TouchFlickCommand);
};

}  // namespace webdriver

#endif // TOUCH_COMMANDS_H
