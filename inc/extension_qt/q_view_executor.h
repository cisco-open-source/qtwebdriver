#ifndef WEBDRIVER_Q_VIEW_EXECUTOR_H
#define WEBDRIVER_Q_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "webdriver_view_executor.h"

#include "webdriver_error.h"


#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

namespace webdriver {

#define NOT_SUPPORTED_IMPL      {*error = new Error(kUnknownError, "Current view doesnt support this command.");}    
//#define NOT_IMPLEMENTED_IMPL    {*error = new Error(kUnknownError, "Command not implemented.");}
//#define RET_IF_ERROR(e)         {if(e) {*error = e; return;}}

/// base class for QT based view's implementation
class QViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit QViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~QViewCmdExecutor();

    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error);
    virtual void Maximize(Error** error);
    virtual void GetScreenShot(std::string* png, Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void Close(Error** error);
    virtual void SwitchTo(Error** error);
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error);
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error);
    virtual void AcceptOrDismissAlert(bool accept, Error** error);

protected:
    QWidget* getView(const ViewId& viewId, Error** error);
    Rect ConvertQRectToRect(const QRect &rect);
    QRect ConvertRectToQRect(const Rect &rect);
    QPoint ConvertPointToQPoint(const Point &p);
    Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);
private:
    DISALLOW_COPY_AND_ASSIGN(QViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_Q_VIEW_EXECUTOR_H
