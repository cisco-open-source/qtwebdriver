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

#include "extension_qt/widget_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "common_util.h"
#include "q_key_converter.h"
#include "extension_qt/widget_element_handle.h"
#include "extension_qt/widget_view_handle.h"
#include "widget_view_util.h"
#include "widget_view_visualizer.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "webdriver_util.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QListView>
#include <QtWidgets/QAction>
#if (1 == WD_ENABLE_PLAYER)
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QMediaPlayer>
#endif //WD_ENABLE_PLAYER
#else
#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>
#include <QtGui/QInputDialog>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QProgressBar>
#include <QtGui/QListView>
#include <QtGui/QAction>
#endif

#include "third_party/pugixml/pugixml.hpp"

namespace webdriver {

const ViewType QWidgetViewCmdExecutorCreator::WIDGET_VIEW_TYPE = 0x13f6;    

QWidgetViewCmdExecutorCreator::QWidgetViewCmdExecutorCreator()
	: ViewCmdExecutorCreator() { }

ViewCmdExecutor* QWidgetViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QWidget* pWidget = QWidgetViewUtil::getView(session, viewId);

    if (NULL != pWidget) {
        session->logger().Log(kFineLogLevel, "Widget executor for view("+viewId.id()+")");
        return new QWidgetViewCmdExecutor(session, viewId);
    }

	return NULL;
}

bool QWidgetViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QWidget* pWidget = QWidgetViewUtil::getView(session, viewId);

    if (NULL != pWidget) {
        if (NULL != viewType) *viewType = WIDGET_VIEW_TYPE;
        return true;
    }

    return false;
}

std::string QWidgetViewCmdExecutorCreator::GetViewTypeName() const {
    return "widget";
}

QWidgetViewCmdExecutor::QWidgetViewCmdExecutor(Session* session, ViewId viewId)
	: QViewCmdExecutor(session, viewId) {
}

QWidgetViewCmdExecutor::~QWidgetViewCmdExecutor() {}

QObject* QWidgetViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    QElementHandle* element_handle = dynamic_cast<QElementHandle*>(session_->GetElementHandle(view_id_, element));

    if (NULL == element_handle) {
        *error = new Error(kNoSuchElement);
        return NULL;
    }

    if (!element_handle->is_valid()) {
        *error = new Error(kStaleElementReference);
        return NULL;
    }

    return element_handle->get();
}

QWidget* QWidgetViewCmdExecutor::getWidget(const ElementId &element, Error** error) {
    QObject* obj = getElement(element, error);

    if (NULL == obj)
        return NULL;

    QWidget* pWidget = qobject_cast<QWidget*>(obj);

    if (NULL == pWidget) {
        *error = new Error(kInvalidElementState);
        return NULL;
    }

    return pWidget;
}

void QWidgetViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    *can = QWidgetViewUtil::isUrlSupported(url);
}

void QWidgetViewCmdExecutor::GetSource(std::string* source, Error** error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);
    QWidgetXmlSerializer serializer(&buff);
    serializer.createXml(view);
    *source = byteArray.data();
}

void QWidgetViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    if (!pWidget->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pWidget->isEnabled()) {
        *error = new Error(kInvalidElementState);
        return;
    }

    std::string err_msg;
    std::vector<QKeyEvent> key_events;
    int modifiers = Qt::NoModifier;

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               true,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "SendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "SendKeys - cant convert keys:"+err_msg);
        return;
    }

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
        qApp->sendEvent(pWidget, &(*it));
        ++it;
    }
}

void QWidgetViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    const FilePath::CharType kPngFileName[] = FILE_PATH_LITERAL("./screen.png");
    FilePath path = session_->temp_dir().Append(kPngFileName);;

    QPixmap pixmap;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    pixmap = pWidget->grab();
#else
    pixmap = QPixmap::grabWidget(pWidget);
#endif

#if defined(OS_WIN)
    session_->logger().Log(kInfoLogLevel, "Save screenshot to - " + path.MaybeAsASCII());
#elif defined(OS_POSIX)
    session_->logger().Log(kInfoLogLevel, "Save screenshot to - " + path.value());
#endif

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

void QWidgetViewCmdExecutor::MouseDoubleClick(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, globalPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, globalPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, dbEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void QWidgetViewCmdExecutor::MouseButtonUp(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp, view: (%4d, %4d)", point.x(), point.y()));
    
    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, globalPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void QWidgetViewCmdExecutor::MouseButtonDown(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown, view: (%4d, %4d)", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, globalPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, pressEvent);
}

void QWidgetViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick, view: (%4d, %4d)", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    Qt::MouseButton mouseButton = QCommonUtil::ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, globalPos, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, globalPos, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, pressEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
    if (Qt::RightButton == mouseButton) {
        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point, globalPos);
        QApplication::postEvent(receiverWidget, contextEvent);
    }
}

void QWidgetViewCmdExecutor::MouseWheel(const int delta, Error **error){
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel start from : (%4d, %4d) on view", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QWheelEvent *wheelEvent = new QWheelEvent(point, globalPos, delta, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, wheelEvent);
}

void QWidgetViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = QCommonUtil::ConvertPointToQPoint(prev_pos);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, view: (%4d, %4d)", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(prev_pos);
}

void QWidgetViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x(), pos.y());
    location.Offset(x_offset, y_offset);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, view: (%4d, %4d)", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
}

void QWidgetViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x()+pWidget->width()/2, pos.y()+pWidget->height()/2);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, view: (%4d, %4d)", point.x(), point.y()));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
        session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, rel: (%4d, %4d)", point.x(), point.y()));
    } else {
        receiverWidget = view;
    }

    QPoint globalPos = receiverWidget->mapToGlobal(point);
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, screen: (%4d, %4d)", globalPos.x(), globalPos.y()));

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, globalPos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
}

void QWidgetViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    QAction* action = qobject_cast<QAction*>(pElement);
    if (NULL != action) {
        if (!action->isVisible()) {
            *error = new Error(kElementNotVisible);
            return;
        }

        if (!action->isEnabled()) {
            *error = new Error(kInvalidElementState);
            return;
        }

        action->trigger();

        return;
    }

    QWidget* pWidget = qobject_cast<QWidget*>(pElement);
    if (NULL == pWidget)
        return;

    if (!pWidget->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    session_->logger().Log(kFineLogLevel, "Click on ");
    session_->logger().Log(kFineLogLevel, pWidget->objectName().toStdString());
    QPoint point;
    QRect rect;

    if ( qobject_cast<QRadioButton*>(pWidget) ||
         qobject_cast<QCheckBox*>(pWidget)) {

        QStyle::SubElement subElement;
        QStyleOptionButton opt;
        if (qobject_cast<QRadioButton*>(pWidget)) {
            subElement = QStyle::SE_RadioButtonClickRect;
        }

        if (qobject_cast<QCheckBox*>(pWidget)) {
            subElement = QStyle::SE_CheckBoxClickRect;
        }
        opt.initFrom(pWidget);
        rect = pWidget->style()->subElementRect(subElement, &opt, pWidget);
    } else {
        rect = pWidget->rect();
    }

    QRect visibleClickableLocation = pWidget->visibleRegion().boundingRect().intersected(rect);

    if(visibleClickableLocation.isEmpty()){
        QWidget *tmpParent = pWidget;
        while(tmpParent != NULL && qobject_cast<QScrollArea*>(tmpParent) == NULL){
            tmpParent = tmpParent->parentWidget();
        }
        if(!tmpParent){
            *error = new Error(kMoveTargetOutOfBounds, "Target is out of view in non scrolable widget");
            return;
        }
    }
    point = QPoint(rect.x() + rect.width()/2, rect.y() + rect.height()/2);

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(pWidget, pressEvent);
    QApplication::postEvent(pWidget, releaseEvent);

    // TODO: session_->set_mouse_position(location);
}

void QWidgetViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    QVariant propertyValue = pElement->property(key.c_str());
    Value* val = NULL;

    if (propertyValue.isValid()) {
        // convert QVariant to base::Value
        if (propertyValue.canConvert<QString>()) {
            val = Value::CreateStringValue(propertyValue.toString().toStdString());
        } else {
            session_->logger().Log(kWarningLogLevel, "cant handle proprty type.");
        }
    } else {
        session_->logger().Log(kWarningLogLevel, "property not found.");
    }

    if (NULL == val) {
        val = Value::CreateNullValue();
    }

    scoped_ptr<Value> ret_value(val);
    *value = static_cast<Value*>(ret_value.release());
}

void QWidgetViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    if (!pWidget->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pWidget->isEnabled()) {
        *error = new Error(kInvalidElementState);
        return;
    }

    // check if we can clear element
    QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit*>(pWidget);
    if (NULL != plainTextEdit) {
        plainTextEdit->clear();
        return;
    }

    QTextEdit *textEdit = qobject_cast<QTextEdit*>(pWidget);
    if (NULL != textEdit) {
        textEdit->clear();
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(pWidget);
    if (NULL != lineEdit) {
        lineEdit->clear();
        return;
    }

    QComboBox *comboBox = qobject_cast<QComboBox*>(pWidget);
    if (NULL != comboBox) {
        if (!comboBox->isEditable())
        {
            *error = new Error(kInvalidElementState);
            return;
        }

        comboBox->clearEditText();
        return;
    }

    *error = new Error(kInvalidElementState);
}

void QWidgetViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    *is_displayed = pElement->property("visible").toBool();
}

void QWidgetViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    *is_enabled = pElement->property("enabled").toBool();
}

void QWidgetViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement1 = getElement(element1, error);
    if (NULL == pElement1)
        return;

    QObject* pElement2 = getElement(element2, error);
    if (NULL == pElement2)
        return;

    *is_equal = (pElement1 == pElement2);
}

void QWidgetViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    *location = Point(pos.x(), pos.y());
}

void QWidgetViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    // TODO: take into account scrollable area
    *location = Point(pos.x(), pos.y());
}

void QWidgetViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    *tag_name = pElement->metaObject()->className();
}

void QWidgetViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pElement);
    if (NULL != checkBox) {
        *is_selected = checkBox->isChecked();
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pElement);
    if (NULL != radioButton) {
        *is_selected = radioButton->isChecked();
        return;
    }

    QAction* action = qobject_cast<QAction*>(pElement);
    if ((NULL != action) && action->isCheckable()) {
        *is_selected = action->isChecked();
        return;
    }

    *error = new Error(kInvalidElementState);
}

void QWidgetViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pElement);
    if (NULL != checkBox) {
        checkBox->setChecked(selected);
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pElement);
    if (NULL != radioButton) {
        radioButton->setChecked(selected);
        return;
    }

    QAction* action = qobject_cast<QAction*>(pElement);
    if ((NULL != action) && action->isCheckable()) {
        action->setChecked(selected);
        return;
    }

    *error = new Error(kInvalidElementState);
}

void QWidgetViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    *size = Size(pWidget->width(), pWidget->height());
}

void QWidgetViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QObject* pElement = getElement(element, error);
    if (NULL == pElement)
        return;

    QVariant visibleValue = pElement->property("visible");
    if (visibleValue.isValid()) {
        if (!visibleValue.toBool()) {
            *element_text = "";
            return;
        }    
    }

    QComboBox *comboBox = qobject_cast<QComboBox*>(pElement);
    if (NULL != comboBox) {
        *element_text = comboBox->currentText().toStdString();
        return;
    }

    QPlainTextEdit *plainText = qobject_cast<QPlainTextEdit*>(pElement);
    if (NULL != plainText) {
        *element_text = plainText->toPlainText().toStdString();
        return;
    }

    QTextEdit *pText = qobject_cast<QTextEdit*>(pElement);
    if (NULL != pText) {
        *element_text = pText->toPlainText().toStdString();
        return;
    }

    QListView *listView = qobject_cast<QListView*>(pElement);
    if (NULL != listView)
    {
        QStringList list;
        foreach(const QModelIndex &index, listView->selectionModel()->selectedIndexes())
        {
            list.append(index.data().toString());
        }
        *element_text = list.join("\n").toStdString();
        return;
    }

    QVariant textValue = pElement->property("text");
    if (textValue.isValid()) {
        if (textValue.canConvert<QString>()) {
            *element_text = textValue.toString().toStdString();
            return;   
        }
    }

    *error = new Error(kNoSuchElement);
}

void QWidgetViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "FindNativeElements, loc:"+locator+" query:"+query);

    Error* tmp_error = NULL;
    QWidget *parentWidget = getWidget(root_element, &tmp_error);
    scoped_ptr<Error> scoped_err(tmp_error);

    if (NULL == parentWidget) {
        session_->logger().Log(kInfoLogLevel, "no root element specified, search from root.");
        parentWidget = view;
    }

    if (locator == LocatorType::kXpath) {
        FindNativeElementsByXpath(parentWidget, query, elements, error);
    } else {
        // list all child widgets and find matched locator
        QList<QWidget*> childs = parentWidget->findChildren<QWidget*>();
        foreach(QWidget *child, childs) {
            if (MatchNativeElement(child, locator, query)) {
                ElementId elm;
                session_->AddElement(view_id_, new QElementHandle(child), &elm);
                (*elements).push_back(elm);

                session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
            }

            // list child actions
            QList<QAction*> actions = child->actions();
            foreach(QAction* action, actions) {
                if (MatchNativeElement(action, locator, query)) {
                    ElementId elm;
                    session_->AddElement(view_id_, new QElementHandle(action), &elm);
                    (*elements).push_back(elm);

                    session_->logger().Log(kFineLogLevel, "element found: "+elm.id());       
                }
            }
        }
    }
}

void QWidgetViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget *focusWidget = QApplication::focusWidget();
    if (NULL == focusWidget || view == focusWidget) {
        *error = new Error(kNoSuchElement);
        return;
    }

    // TODO: do we need to check if focusWidget is child of view?

    ElementId active_element;

    session_->AddElement(view_id_, new QElementHandle(focusWidget), &active_element);

    *element = active_element;
}

void QWidgetViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    ViewHandle* viewHandle = NULL;

    session_->logger().Log(kFineLogLevel, "Navigate to widget - "+url);

    std::string window_position;
    int x, y;
    scoped_ptr<Point> position(NULL);
    if (session_->get_desired_caps()->GetString(Capabilities::kWindowPosition, &window_position)) {

        if (GetTwoIntsFromString(window_position, x, y)) {
            position.reset(new Point(x, y));
        }
    }

    // create view
    ViewFactory::GetInstance()->CreateViewForUrl(session_->logger(), url, position.get(), NULL, &viewHandle);
    if (NULL == viewHandle) {
        *error = new Error(kUnknownError, "cant load widget - " + url);
        return;
    }

    view->close();

    // map viewId to new widget
    session_->ReplaceViewHandle(view_id_, viewHandle);
}

void QWidgetViewCmdExecutor::GetURL(std::string* url, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string className(view->metaObject()->className());

    *url = QWidgetViewUtil::makeUrlByClassName(className);
}

void QWidgetViewCmdExecutor::TouchClick(const ElementId& element, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0, 0);
    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, QPointF(point));
    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, QPointF(point));

    QApplication::postEvent(pWidget, touchBeginEvent);
    QApplication::postEvent(pWidget, touchEndEvent);
}

void QWidgetViewCmdExecutor::TouchDoubleClick(const ElementId& element, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0, 0);
    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

//    point = pWidget->mapToGlobal(point);

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, QPointF(point));
    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, QPointF(point));
    QTouchEvent *touchBeginEvent2 = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, QPointF(point));
    QTouchEvent *touchEndEvent2 = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, QPointF(point));

    QApplication::postEvent(pWidget, touchBeginEvent);
    QApplication::postEvent(pWidget, touchEndEvent);
    QApplication::postEvent(pWidget, touchBeginEvent2);
    QApplication::postEvent(pWidget, touchEndEvent2);
}

void QWidgetViewCmdExecutor::TouchDown(const int &x, const int &y, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(Point(x, y));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);

    QApplication::postEvent(receiverWidget, touchBeginEvent);

    QApplication::processEvents();
}

void QWidgetViewCmdExecutor::TouchUp(const int &x, const int &y, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(Point(x, y));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);

    QApplication::postEvent(receiverWidget, touchEndEvent);

    QApplication::processEvents();
}

void QWidgetViewCmdExecutor::TouchMove(const int &x, const int &y, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(Point(x, y));

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QTouchEvent *touchMoveEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, point);

    QApplication::postEvent(receiverWidget, touchMoveEvent);

    QApplication::processEvents();
}

void QWidgetViewCmdExecutor::TouchLongClick(const ElementId& element, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0, 0);
    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

//    point = pWidget->mapToGlobal(point);

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);
    QApplication::postEvent(view, touchBeginEvent);

    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();

    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);
    QApplication::postEvent(pWidget, touchEndEvent);
}

void QWidgetViewCmdExecutor::TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size sizel;
    GetElementSize(element, &sizel, error);
    if (*error)
        return;

    location.Offset(sizel.width() / 2, sizel.height() / 2);

    QPoint startPoint = QCommonUtil::ConvertPointToQPoint(location);

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

//    startPoint = pWidget->mapToGlobal(startPoint);
    Point offset(xoffset, yoffset);
    QPoint offsetPoint = QCommonUtil::ConvertPointToQPoint(offset);
    int stepCount = 20;
    int timeBetweenEvent = 30;
    QEventLoop loop;

    for (int i = 0; i <= stepCount; ++i)
    {
        QPointF touchPoint(startPoint.x()+offsetPoint.x()*i/stepCount, startPoint.y()+offsetPoint.y()*i/stepCount);

        QTouchEvent *touchEvent;
        if (i == 0)
            touchEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, touchPoint);
        else if (i == stepCount)
            touchEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, touchPoint);
        else
            touchEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, touchPoint);


        QApplication::postEvent(pWidget, touchEvent);
        QTimer::singleShot(timeBetweenEvent, &loop, SLOT(quit()));
        loop.exec();
    }
}

void QWidgetViewCmdExecutor::TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size sizel;
    GetElementSize(element, &sizel, error);
    if (*error)
        return;

    location.Offset(sizel.width() / 2, sizel.height() / 2);

    QPoint startPoint = QCommonUtil::ConvertPointToQPoint(location);

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

//    startPoint = pWidget->mapToGlobal(startPoint);

    QPoint offsetPoint = QCommonUtil::ConvertPointToQPoint(Point(xoffset, yoffset));

    QEventLoop loop;

    //some magic numbers
    int stepCount = 20;
    int timeBetweenEvent = 30/(speed+1);

//    QVector2D velocity(xoffset*1000/(stepCount*timeBetweenEvent), yoffset*1000/(stepCount*timeBetweenEvent));

    for (int i = 0; i <= stepCount; ++i)
    {
        QPointF touchPoint(startPoint.x()+offsetPoint.x()*i/stepCount, startPoint.y()+offsetPoint.y()*i/stepCount);

        QTouchEvent *touchEvent;
        if (i == 0)
            touchEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, touchPoint);
        else if (i == stepCount)
            touchEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, touchPoint);
        else
            touchEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, touchPoint);

        QApplication::postEvent(pWidget, touchEvent);
        QTimer::singleShot(timeBetweenEvent, &loop, SLOT(quit()));
        loop.exec();
    }
}

void QWidgetViewCmdExecutor::GetPlayerState(const ElementId &element, PlayerState *state, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    *state = (PlayerState)(int)player->state();
#else
    NOT_SUPPORTED_IMPL
#endif   
}

void QWidgetViewCmdExecutor::SetPlayerState(const ElementId &element, PlayerState state, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }

    switch(state){
    case Playing: player->play();break;
    case Stopped: player->stop();break;
    case Paused: player->pause();break;
    }

#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::GetPlayerVolume(const ElementId &element, double *volume, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }

    *volume = player->volume() / 100.0;
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::SetPlayerVolume(const ElementId &element, double volume, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }

    player->setVolume(int(volume*100));
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::GetPlayingPosition(const ElementId &element, double *position, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    *position = player->position()/1000.0;
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::SetPlayingPosition(const ElementId &element, double position, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    player->setPosition(qint64(position*1000));
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::SetMute(const ElementId &element, bool mute, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    player->setMuted(mute);
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::GetMute(const ElementId &element, bool *mute, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    *mute = player->isMuted();
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::SetPlaybackSpeed(const ElementId &element, double speed, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    player->setPlaybackRate(speed);
#else
    NOT_SUPPORTED_IMPL
#endif
}

void QWidgetViewCmdExecutor::GetPlaybackSpeed(const ElementId &element, double *speed, Error **error)
{
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getWidget(element, error);
    if (NULL == pWidget)
        return;

    QVideoWidget* videoWidget = dynamic_cast<QVideoWidget*>(pWidget);
    if(NULL == videoWidget){
        *error = new Error(kInvalidElementState);
        return;
    }

    QMediaPlayer *player = dynamic_cast<QMediaPlayer*>(videoWidget->mediaObject());
    if(NULL == player){
        *error = new Error(kInvalidElementState);
        return;
    }
    *speed = player->playbackRate();
#else
    NOT_SUPPORTED_IMPL
#endif
}

bool QWidgetViewCmdExecutor::MatchNativeElement(const QObject* item, const std::string& locator, const std::string& query) {
    if (locator == LocatorType::kClassName) {
        if (query == item->metaObject()->className())
            return true;
    } else if (locator == LocatorType::kId) {
        if (query == item->objectName().toStdString())
            return true;
    } else if (locator == LocatorType::kName) {
        if (item->isWidgetType())
            if (query == (qobject_cast<const QWidget*>(item))->windowTitle().toStdString())
                return true;
    } else {
        session_->logger().Log(kWarningLogLevel, "unsupported locator - "+locator);
        // LocatorType::kLinkText
        // LocatorType::kPartialLinkText
        // LocatorType::kCss
        // LocatorType::kTagName
    }

    return false;
}

void QWidgetViewCmdExecutor::FindNativeElementsByXpath(QWidget* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);
    QWidgetXmlSerializer serializer(&buff);
    serializer.setDumpAll(true);
    serializer.createXml(parent);

    buff.seek(0);

    // prepare pointer to data to parse
    const void* content = byteArray.data();
    int content_size = byteArray.size();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(content, content_size);

    if (result) {
        session_->logger().Log(kFineLogLevel, "UI XML parsed without errors");

        // Select nodes via compiled query
        pugi::xpath_query query_nodes(query.c_str());
        pugi::xpath_node_set found_nodes = query_nodes.evaluate_node_set(doc);

        if ( (NULL == query_nodes.result().error) && 
             (pugi::xpath_type_node_set == query_nodes.return_type()) ) {

            for (pugi::xpath_node_set::const_iterator it = found_nodes.begin(); it != found_nodes.end(); ++it) {
                pugi::xpath_node node = *it;

                QString elemId(node.node().attribute("elementId").value());

                if (!elemId.isEmpty()) {
                    const QWidgetXmlSerializer::XMLElementMap& elementsMap = serializer.getElementsMap();
                    if (elementsMap.contains(elemId)) {
                        ElementId elm;
                        session_->AddElement(view_id_, new QElementHandle(elementsMap[elemId]), &elm);
                        (*elements).push_back(elm);
                        session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
                    } else {
                        session_->logger().Log(kSevereLogLevel, "cant get element from map, skipped");
                    }
                }
            }
        } else {
            std::string error_descr = "Cant evaluate XPath to node set: ";       
            error_descr += query_nodes.result().description();
            session_->logger().Log(kWarningLogLevel, error_descr);
            *error = new Error(kXPathLookupError);
        }
    }
    else
    {
        std::string error_descr = "   Error description: ";
        error_descr += result.description();
        session_->logger().Log(kWarningLogLevel, "UI XML parsed with errors:");
        session_->logger().Log(kWarningLogLevel, error_descr);
    }

    // destroy tree
    doc.reset();

    buff.close();
}

void QWidgetViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidgetViewVisualizerSourceCommand command(session_, view_id_, view);
    command.Execute(source, error);
}

} //namespace webdriver 
