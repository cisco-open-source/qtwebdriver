#include "extension_qt/q_view_executor.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

namespace webdriver {

QViewCmdExecutor::QViewCmdExecutor(Session* session, ViewId viewId)
    : ViewCmdExecutor(session, viewId) {

}

QViewCmdExecutor::~QViewCmdExecutor() {

};

Error* QViewCmdExecutor::checkView(const ViewId& viewId, QWidget** widget) {
    ViewHandle handle = session_->GetViewHandle(viewId);

    QWidget* pWidget = static_cast<QWidget*>(handle);

    if (NULL == pWidget) {
        return new Error(kNoSuchWindow);
    }

    *widget = pWidget;

    return NULL;
}

void QViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }

    *title = view->windowTitle().toStdString();

    session_->logger().Log(kFineLogLevel, "GetTitle - "+*title);
}

void QViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }

    *bounds = ConvertQRectToRect(view->geometry());
}
    
void QViewCmdExecutor::SetBounds(const Rect& bounds, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }

    view->setGeometry(ConvertRectToQRect(bounds));
}

void QViewCmdExecutor::Maximize(Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }

    if (!view->isTopLevel()) {
        *error = new Error(kUnknownError, "Cant maximize non top level view.");
        return;
    }

    view->setGeometry(QApplication::desktop()->rect());    
}

void QViewCmdExecutor::GetScreenShot(std::string* png, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }
    
    FilePath path = session_->temp_dir();
    path.Append("/screen.png");

    QPixmap pixmap = QPixmap::grabWidget(view);

#if defined(OS_POSIX)
    if (!pixmap.save(path.value().c_str())) 
#elif defined(OS_WIN)
    if (!pixmap.save(QString::fromUtf16((ushort*)path.value().c_str())))
#endif // OS_WIN
    {
        *error = new Error(kUnknownError, "screenshot was not captured");
        return;
    }

    if (!file_util::ReadFileToString(path, png))
        *error = new Error(kUnknownError, "Could not read screenshot file");
}

void QViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    // TODO: implement
    session_->logger().Log(kSevereLogLevel, "SendKeys - not implemented, TBD");
    *error = new Error(kUnknownError, "SendKeys - not implemented, TBD");
}

void QViewCmdExecutor::Close(Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (error) {
        *error = err;
        return;
    }

    if (!view->isTopLevel()) {
        *error = new Error(kUnknownError, "Cant close non top level view.");
        return;
    }

    session_->RemoveView(view_id_);

    view->close();
}

void QViewCmdExecutor::SwitchTo(Error** error) {
    session_->set_current_view(view_id_);

    // TODO: clear frame_elements and current_frame
}

void QViewCmdExecutor::GetAlertMessage(std::string* text, Error** error) {
    // TODO: implement
    session_->logger().Log(kSevereLogLevel, "GetAlertMessage - not implemented, TBD");
    *error = new Error(kUnknownError, "GetAlertMessage - not implemented, TBD");
}

void QViewCmdExecutor::SetAlertPromptText(const std::string& alert_prompt_text, Error** error) {
    // TODO: implement
    session_->logger().Log(kSevereLogLevel, "SetAlertPromptText - not implemented, TBD");
    *error = new Error(kUnknownError, "SetAlertPromptText - not implemented, TBD");
}

void QViewCmdExecutor::AcceptOrDismissAlert(bool accept, Error** error) {
    // TODO: implement
    session_->logger().Log(kSevereLogLevel, "AcceptOrDismissAlert - not implemented, TBD");
    *error = new Error(kUnknownError, "AcceptOrDismissAlert - not implemented, TBD");
}

Rect QViewCmdExecutor::ConvertQRectToRect(const QRect &rect)
{
    return Rect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect QViewCmdExecutor::ConvertRectToQRect(const Rect &rect)
{
    QRect resultRect;
    resultRect.setX(rect.x());
    resultRect.setY(rect.y());
    resultRect.setWidth(rect.width());
    resultRect.setHeight(rect.height());

    return resultRect;
}

} // namespace webdriver     