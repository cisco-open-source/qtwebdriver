#include "extension_qt/q_view_executor.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

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
        session_->logger().Log(kWarningLogLevel, "checkView - no such view("+viewId.id()+")");
        return new Error(kNoSuchWindow);
    }

    *widget = pWidget;

    return NULL;
}

void QViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    *title = view->windowTitle().toStdString();

    session_->logger().Log(kFineLogLevel, "GetTitle - "+*title);
}

void QViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    *bounds = ConvertQRectToRect(view->geometry());
}
    
void QViewCmdExecutor::SetBounds(const Rect& bounds, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    view->setGeometry(ConvertRectToQRect(bounds));
}

void QViewCmdExecutor::Maximize(Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
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

    if (err) {
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

    if (err) {
        *error = err;
        return;
    }

    if (!view->isTopLevel()) {
        *error = new Error(kUnknownError, "Cant close non top level view.");
        return;
    }

    session_->logger().Log(kInfoLogLevel, "close View("+view_id_.id()+")");

    session_->RemoveView(view_id_);

    // destroy children correctly
    QList<QWidget*> childs = view->findChildren<QWidget*>();
    foreach(QWidget *child, childs)
    {
        child->setAttribute(Qt::WA_DeleteOnClose, true);
        child->close();
    }

    view->close();
}

void QViewCmdExecutor::SwitchTo(Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    session_->set_current_view(view_id_);

    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");
}

void QViewCmdExecutor::GetAlertMessage(std::string* text, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    // QMessageBox::information(pWeb, "Alert", message->c_str(), QMessageBox::Ok);
    QMessageBox *msgBox = view->findChild<QMessageBox*>();
    if (NULL != msgBox) {
        *text = msgBox->text().toStdString();
    } else {
        QInputDialog *msgbox = view->findChild<QInputDialog*>();

        if (NULL != msgbox) {
            *text = msgbox->labelText().toStdString();
        } else {
            *error = new Error(kNoAlertOpenError);
        }
    }
}

void QViewCmdExecutor::SetAlertPromptText(const std::string& alert_prompt_text, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    std::string message_text;
    GetAlertMessage(&message_text, &err);
    if (err) {
        *error = err;
        return;
    }

    QInputDialog *alert = view->findChild<QInputDialog*>();

    if (NULL != alert) {
        alert->setTextValue(alert_prompt_text.c_str());
    } else {
        // in python ELEMENT_NOT_VISIBLE = 11
        // kNoAlertError = 27
        *error = new Error(kElementNotVisible);
    }
}

void QViewCmdExecutor::AcceptOrDismissAlert(bool accept, Error** error) {
    QWidget* view = NULL;
    Error* err = checkView(view_id_, &view);

    if (err) {
        *error = err;
        return;
    }

    QMessageBox *msgBox = view->findChild<QMessageBox*>();

    if(NULL != msgBox) {
        if(accept) {
            msgBox->accept();
        } else {
            msgBox->close();
        }
    } else {
        QInputDialog *msgbox = view->findChild<QInputDialog*>();
        if(NULL != msgbox) {
            if(accept) {
                msgbox->accept();
            } else {
                msgbox->close();
            }
        } else {
            *error = new Error(kNoAlertOpenError);
        }
    }
}

Rect QViewCmdExecutor::ConvertQRectToRect(const QRect &rect) {
    return Rect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect QViewCmdExecutor::ConvertRectToQRect(const Rect &rect) {
    QRect resultRect;
    resultRect.setX(rect.x());
    resultRect.setY(rect.y());
    resultRect.setWidth(rect.width());
    resultRect.setHeight(rect.height());

    return resultRect;
}

QPoint QViewCmdExecutor::ConvertPointToQPoint(const Point &p) {
    QPoint resultPoint;
    resultPoint.setX(p.x());
    resultPoint.setY(p.y());

    return resultPoint;
}

Qt::MouseButton QViewCmdExecutor::ConvertMouseButtonToQtMouseButton(MouseButton button) {
    Qt::MouseButton result = Qt::NoButton;

    switch(button)
    {
        case kLeftButton: result = Qt::LeftButton; break;
        case kMiddleButton: result = Qt::MidButton; break;
        case kRightButton: result = Qt::RightButton; break;
        default: result = Qt::NoButton;
    }

    return result;
}

} // namespace webdriver     