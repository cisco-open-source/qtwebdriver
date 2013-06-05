#ifndef WEBDRIVER_Q_VIEW_EXECUTOR_H
#define WEBDRIVER_Q_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "webdriver_view_executor.h"

#include "webdriver_error.h"

#include <QtGui/QWidget>

namespace webdriver {

//#define NOT_SUPPORTED_IMPL      {*error = new Error(kUnknownError, "Current view doesnt support this command.");}    
//#define NOT_IMPLEMENTED_IMPL    {*error = new Error(kUnknownError, "Command not implemented.");}
//#define RET_IF_ERROR(e)         {if(e) {*error = e; return;}}

/// base class for QT based view's implementation
class QViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit QViewCmdExecutor(Session* session, ViewId viewId);
    ~QViewCmdExecutor();

    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error);
    virtual void Maximize(Error** error);
    virtual void GetScreenShot(std::string* png, Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void MouseDoubleClick(Error** error) = 0;
    virtual void MouseButtonUp(Error** error) = 0;
    virtual void MouseButtonDown(Error** error) = 0;
    virtual void MouseClick(MouseButton button, Error** error) = 0;
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error) = 0;
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) = 0;
    virtual void MouseMove(const ElementId& element, Error** error) = 0;
    virtual void ClickElement(const ElementId& element, Error** error) = 0;
    virtual void Close(Error** error);
    virtual void SwitchTo(Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error);
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error);
    virtual void AcceptOrDismissAlert(bool accept, Error** error);

protected:
    Error* checkView(const ViewId& viewId, QWidget** widget);
    Rect ConvertQRectToRect(const QRect &rect);
    QRect ConvertRectToQRect(const Rect &rect);
    QPoint ConvertPointToQPoint(const Point &p);
    Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);
private:
    DISALLOW_COPY_AND_ASSIGN(QViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_Q_VIEW_EXECUTOR_H
