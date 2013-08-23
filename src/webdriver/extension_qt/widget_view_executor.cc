#include "extension_qt/widget_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "webdriver_util.h"
#include "q_key_converter.h"
#include "extension_qt/widget_element_handle.h"
#include "extension_qt/widget_view_handle.h"
#include "widget_view_util.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QMetaProperty>
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
#endif

#include "third_party/pugixml/pugixml.hpp"

namespace webdriver {

class QWidgetXmlSerializer {
public:
    typedef QHash<QString, QWidget*> XMLElementMap;

    QWidgetXmlSerializer(QIODevice* buff, XMLElementMap& elementsMap)
        : elementsMap_(elementsMap), dumpAll_(false)
    {
        writer_.setDevice(buff);
        writer_.setAutoFormatting(true);
    }

    void createXml(QWidget* widget) {
        writer_.writeStartDocument();
        addWidget(widget);
        writer_.writeEndDocument();
    }

    void setDumpAll(bool dumpAll) {
        dumpAll_ = dumpAll;
    }

private:
    void addWidget(QWidget* widget) {
        writer_.writeStartElement(widget->metaObject()->className());

        if (dumpAll_) {
            for (int propertyIndex = 0; propertyIndex < widget->metaObject()->propertyCount(); propertyIndex++) {
                const QMetaProperty& property = widget->metaObject()->property(propertyIndex);
                writer_.writeAttribute(property.name(), property.read(widget).toString());
            }
        }

        if (!widget->objectName().isEmpty())
            writer_.writeAttribute("id", widget->objectName());

        if (!widget->windowTitle().isEmpty())
            writer_.writeAttribute("name", widget->windowTitle());

        QString elementKey = GenerateRandomID().c_str();
        elementsMap_.insert(elementKey, QPointer<QWidget>(widget));
        writer_.writeAttribute("elementId", elementKey);

        QList<QObject*> childs = widget->children();
        foreach(QObject* child, childs) {
            QWidget* childWgt = qobject_cast<QWidget*>(child);
            if (childWgt)
                addWidget(childWgt);
        }

        writer_.writeEndElement();
    }

    QXmlStreamWriter writer_;
    XMLElementMap& elementsMap_;
    bool dumpAll_;
};

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

QWidgetViewCmdExecutor::QWidgetViewCmdExecutor(Session* session, ViewId viewId)
	: QViewCmdExecutor(session, viewId) {
}

QWidgetViewCmdExecutor::~QWidgetViewCmdExecutor() {}

QWidget* QWidgetViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    QElementHandle* element_handle = dynamic_cast<QElementHandle*>(session_->GetElementHandle(view_id_, element));

    if (NULL == element_handle) {
        *error = new Error(kNoSuchElement);
        return NULL;
    }

    if (!element_handle->is_valid()) {
        *error = new Error(kStaleElementReference);
        return NULL;
    }

    QWidget* retObj = qobject_cast<QWidget*>(element_handle->get());
    if (NULL == retObj) {
        *error = new Error(kUnknownError, "canot cast element to QWidget.");
        return NULL;
    }

    return retObj;
}

void QWidgetViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
	*can = QWidgetViewUtil::isUrlSupported(url);
}

void QWidgetViewCmdExecutor::GetSource(std::string* source, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    XMLElementMap elementsMap;
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);
    QWidgetXmlSerializer serializer(&buff, elementsMap);
    serializer.createXml(view);
    *source = byteArray.data();
}

void QWidgetViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
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

void QWidgetViewCmdExecutor::MouseDoubleClick(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, dbEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void QWidgetViewCmdExecutor::MouseButtonUp(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void QWidgetViewCmdExecutor::MouseButtonDown(Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(receiverWidget, pressEvent);
}

void QWidgetViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, pressEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
    if (Qt::RightButton == mouseButton) {
        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point);
        QApplication::postEvent(receiverWidget, contextEvent);
    }
}

void QWidgetViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = ConvertPointToQPoint(prev_pos);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(prev_pos);
}

void QWidgetViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x(), pos.y());
    location.Offset(x_offset, y_offset);

    QPoint point = ConvertPointToQPoint(location);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
}

void QWidgetViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x()+pWidget->width()/2, pos.y()+pWidget->height()/2);

    QPoint point = ConvertPointToQPoint(location);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
}

void QWidgetViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
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

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QVariant propertyValue = pWidget->property(key.c_str());
    Value* val = NULL;

    if (propertyValue.isValid()) {
        // convert QVariant to base::Value
        switch (propertyValue.type()) {
        case QVariant::Bool:
            val = Value::CreateBooleanValue(propertyValue.toBool());
            break;
        case QVariant::Int:
            val = Value::CreateIntegerValue(propertyValue.toInt());
            break;
        case QVariant::Double:
            val = Value::CreateDoubleValue(propertyValue.toDouble());
            break;
        case QVariant::String:
            val = Value::CreateStringValue(propertyValue.toString().toStdString());
            break;
        default:
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

    QWidget* pWidget = getElement(element, error);
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

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    *is_displayed = pWidget->isVisible();
}

void QWidgetViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    *is_enabled = pWidget->isEnabled();
}

void QWidgetViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget1 = getElement(element1, error);
    if (NULL == pWidget1)
        return;

    QWidget* pWidget2 = getElement(element2, error);
    if (NULL == pWidget2)
        return;

    *is_equal = (pWidget1 == pWidget2);
}

void QWidgetViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    *location = Point(pos.x(), pos.y());
}

void QWidgetViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
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

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    *tag_name = pWidget->metaObject()->className();
}

void QWidgetViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pWidget);
    if (NULL != checkBox) {
        *is_selected = checkBox->isChecked();
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pWidget);
    if (NULL != radioButton) {
        *is_selected = radioButton->isChecked();
        return;
    }

    *error = new Error(kInvalidElementState);
}

void QWidgetViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pWidget);
    if (NULL != checkBox) {
        checkBox->setChecked(selected);
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pWidget);
    if (NULL != radioButton) {
        radioButton->setChecked(selected);
        return;
    }

    *error = new Error(kInvalidElementState);
}

void QWidgetViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    *size = Size(pWidget->width(), pWidget->height());
}

void QWidgetViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    if (!pWidget->isVisible()) {
        *element_text = "";
        return;
    }

    QComboBox *comboBox = qobject_cast<QComboBox*>(pWidget);
    if (NULL != comboBox) {
        *element_text = comboBox->currentText().toStdString();
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(pWidget);
    if (NULL != lineEdit) {
        *element_text = lineEdit->text().toStdString();
        return;
    }

    QPlainTextEdit *plainText = qobject_cast<QPlainTextEdit*>(pWidget);
    if (NULL != plainText) {
        *element_text = plainText->toPlainText().toStdString();
        return;
    }

    QTextEdit *pText = qobject_cast<QTextEdit*>(pWidget);
    if (NULL != pText) {
        *element_text = pText->toPlainText().toStdString();
        return;
    }

    QPushButton *pushButton = qobject_cast<QPushButton*>(pWidget);
    if (NULL != pushButton) {
        *element_text = pushButton->text().toStdString();
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pWidget);
    if (NULL != radioButton) {
        *element_text = radioButton->text().toStdString();
        return;
    }

    QLabel *label = qobject_cast<QLabel*>(pWidget);
    if (NULL != label) {
        *element_text = label->text().toStdString();
        return;
    }

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pWidget);
    if (NULL != checkBox) {
        *element_text = checkBox->text().toStdString();
        return;
    }

    QProgressBar *progressBar= qobject_cast<QProgressBar*>(pWidget);
    if (NULL != progressBar) {
        *element_text = progressBar->text().toStdString();
        return;
    }

    QListView *listView = qobject_cast<QListView*>(pWidget);
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

    *error = new Error(kNoSuchElement);
}

void QWidgetViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "FindNativeElements, loc:"+locator+" query:"+query);

    Error* tmp_error = NULL;
    QWidget *parentWidget = getElement(root_element, &tmp_error);
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
            if (FilterNativeWidget(child, locator, query)) {
                ElementId elm;
                session_->AddElement(view_id_, new QElementHandle(child), &elm);
                (*elements).push_back(elm);

                session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
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

    // create view
    ViewFactory::GetInstance()->CreateViewForUrl(session_->logger(), url, &viewHandle);
    if (NULL == viewHandle) {
        *error = new Error(kUnknownError, "cant load widget - " + url);
        return;
    }

    // close old widget, destroy children correctly
    QList<QWidget*> childs = view->findChildren<QWidget*>();
    foreach(QWidget *child, childs)
    {
        child->setAttribute(Qt::WA_DeleteOnClose, true);
        child->close();
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

bool QWidgetViewCmdExecutor::FilterNativeWidget(const QWidget* widget, const std::string& locator, const std::string& query) {
    if (locator == LocatorType::kClassName) {
        if (query == widget->metaObject()->className())
            return true;
    } else if (locator == LocatorType::kId) {
        if (query == widget->objectName().toStdString())
            return true;
    } else if (locator == LocatorType::kName) {
        if (query == widget->windowTitle().toStdString())
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
    XMLElementMap elementsMap;
    QWidgetXmlSerializer serializer(&buff, elementsMap);
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

    XMLElementMap elementsMap;
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    QWidgetXmlSerializer serializer(&buff, elementsMap);
    serializer.setDumpAll(true);
    serializer.createXml(view);
    *source = byteArray.data();

    session_->logger().Log(kInfoLogLevel, "[VisualizerSource] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);
    *source = transform(*source, "src/webdriver/extension_qt/widget_view_visualizer.xsl");
}

std::string QWidgetViewCmdExecutor::transform(const std::string& source, const std::string& stylesheet) const {
    QProcess process;
    QStringList arguments;
    arguments << "-jar" << "src/third_party/saxon/saxon9he.jar";
    arguments << QString::fromStdString("-xsl:" + stylesheet);
    arguments << "-";
    process.start("java", arguments);
    if (!process.waitForStarted(-1)) {
        session_->logger().Log(kSevereLogLevel, "Can not start process!");
        return "";
    }
    qint64 written = process.write(source.data(), source.length());
    if (written != source.length()) {
        session_->logger().Log(kSevereLogLevel, "Writting xml to xsl processor failure!");
        return "";
    }
    process.waitForBytesWritten(-1);
    process.closeWriteChannel();

    process.waitForFinished(-1);

    QByteArray stderr = process.readAllStandardError();
    if (stderr.length() > 0) {
        session_->logger().Log(kSevereLogLevel, QString::fromAscii(stderr.data(), stderr.length()).toStdString());
    }

    QByteArray stdout = process.readAllStandardOutput();
    return QString::fromAscii(stdout.data(), stdout.length()).toStdString();
}

} //namespace webdriver 
