#include "extension_qt/web_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"

#include "third_party/webdriver/atoms.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "q_key_converter.h"
#include "webdriver_server.h"

#include "web_view_util.h"
#include "extension_qt/widget_view_handle.h"

#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"

#include <stdexcept>
#include <QtCore/QtGlobal>
#include <QtCore/QStringBuilder>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QTimer>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QApplication>
#include <QtWebKitWidgets/QWebFrame>

#else
#include <QtGui/QApplication>
#endif
class QNetworkCookie;

namespace webdriver {

void QPageLoader::loadPage(QUrl url) {
    connect(webView, SIGNAL(loadStarted()),this, SLOT(pageLoadStarted()));
    webView->load(url);
}

void QPageLoader::reloadPage() {
    connect(webView, SIGNAL(loadStarted()),this, SLOT(pageLoadStarted()));
    webView->reload();
}

void QPageLoader::pageLoadStarted() {
    is_loading = true;
    connect(webView, SIGNAL(loadFinished(bool)),this, SLOT(pageLoadFinished()), Qt::QueuedConnection);
}

void QPageLoader::pageLoadFinished() {
    is_loading = false;
    emit loaded();
}

JSNotifier::JSNotifier():
    isCompleted(false)
{
}

QVariant JSNotifier::getResult()
{
    return res;
}

bool JSNotifier::IsCompleted()
{
    return isCompleted;
}

void JSNotifier::setResult(QVariant result)
{
    res = result;
    isCompleted = true;
    emit completed();
}

BrowserLogHandler::BrowserLogHandler(QObject *parent): QObject(parent), jslogger()
{
}

base::ListValue* BrowserLogHandler::getLog()
{
    return jslogger.getLog();
}

void BrowserLogHandler::SetMinLogLevel(LogLevel level)
{
    jslogger.SetMinLogLevel(level);
}

void BrowserLogHandler::loadJSLogObject()
{
    QWebFrame *mainFrame = qobject_cast<QWebFrame*>(sender());
    if (mainFrame)
        loadJSLogObject(mainFrame);
}

void BrowserLogHandler::loadJSLogObject(QWebFrame *frame)
{
    frame->addToJavaScriptWindowObject("wdconsole", &jslogger);
}

void BrowserLogHandler::loadConsoleJS()
{
    QWebView* view = qobject_cast<QWebView*>(sender());
    if (view)
        loadConsoleJS(view);
}

void BrowserLogHandler::loadConsoleJS(const QWebView *view)
{
    QString jscript ("if ( _log == null ){"
                    "var _log = console.log,"
                        "_warn = console.warn,"
                        "_error = console.error;"
                    "};"

                    "console.log = function() {"
                        "wdconsole.log(arguments[0]);"
                        "return _log.apply(console, arguments);"
                    "};"

                     "console.warn = function() {"
                        "wdconsole.warn(arguments[0]);"
                        "return _warn.apply(console, arguments);"
                     "};"

                     "console.error = function() {"
                        "wdconsole.error(arguments[0]);"
                        "return _error.apply(console, arguments);"
                     "};");
    view->page()->mainFrame()->evaluateJavaScript(jscript);
}

JSLogger::JSLogger()
{
    browserLogger.AddHandler(&browserLog);
}

base::ListValue* JSLogger::getLog()
{
    base::ListValue* retVal = browserLog.entries_list()->DeepCopy();
    browserLog.clear_entries_list();
    return retVal;
}

void  JSLogger::SetMinLogLevel(LogLevel level)
{
    browserLogger.set_min_log_level(level);
}

void JSLogger::log(QVariant message)
{
    browserLogger.Log(kInfoLogLevel, message.toString().toStdString());
}

void JSLogger::warn(QVariant message)
{
    browserLogger.Log(kWarningLogLevel, message.toString().toStdString());
}

void JSLogger::error(QVariant message)
{
    browserLogger.Log(kSevereLogLevel, message.toString().toStdString());
}

QWebViewVisualizerSourceCommand::QWebViewVisualizerSourceCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view)
    : executor_(executor), session_(session), view_(view)
{}

void QWebViewVisualizerSourceCommand::Execute(std::string* source, Error** error) {
    // Convert DOM tree to valid XML.
    const char* kSource =
        "function() {\n"
        "  var scripts = document.getElementsByTagName('script');\n"
        "  for (var scriptIndex = 0; scriptIndex < scripts.length; scriptIndex++) {\n"
        "    var script = scripts[scriptIndex];\n"
        "    script.innerHTML = '';\n"
        "  }\n"
        "\n"
        "  var xhtml = document.implementation.createDocument();\n"
        "  xhtml = xhtml.importNode(document.documentElement, true);\n"
        "\n"
        "  return new XMLSerializer().serializeToString(xhtml);\n"
        "}";

    *error = executor_->ExecuteScriptAndParse(
                executor_->GetFrame(view_, session_->current_frame()),
                kSource,
                "getSource",
                new ListValue(),
                CreateDirectValueParser(source));

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);

    QSharedPointer<QDomDocument> document = ParseXml(QString::fromStdString(*source), error);
    if (!document)
        return;

    QDomElement documentElement = document->documentElement();
    AssemblePage(documentElement);

    *source = document->toString().toStdString();

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] after transform:");
    session_->logger().Log(kInfoLogLevel, *source);
}

QSharedPointer<QDomDocument> QWebViewVisualizerSourceCommand::ParseXml(const QString& input, Error** error) {
    if (*error) throw std::invalid_argument("error");

    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    QSharedPointer<QDomDocument> document(new QDomDocument());
    bool retval = document->setContent(input, &errorMsg, &errorLine, &errorColumn);
    if (!retval) {
        *error = new Error(kInternalServerError, errorMsg.toStdString());
        return QSharedPointer<QDomDocument>();
    }

    return document;
}

void QWebViewVisualizerSourceCommand::AssemblePage(QDomElement element) const {
    if (element.tagName() == "img") {
        AssembleImg(element);
    }
    if (element.tagName() == "link") {
        AssembleLink(element);
    }
    if (element.tagName() == "style") {
        AssembleStyle(element);
    }

    if (element.hasAttribute("style")) {
        AssembleStyle(element.attributeNode("style"));
    }

    RemoveScripts(element);

    // Chrome does like empty tags like <textarea/>
    if (element.tagName() != "br" &&
        element.childNodes().length() == 0)
        element.appendChild(element.ownerDocument().createTextNode(" "));

    // Recursively walk DOM tree
    QDomNodeList children = element.childNodes();
    for (int childIndex = 0; childIndex < children.length(); childIndex++) {
        QDomNode child = children.at(childIndex);
        if (child.nodeType() == QDomNode::ElementNode) {
            QDomElement childElement = child.toElement();
            AssemblePage(childElement);
        }
    }
}

void QWebViewVisualizerSourceCommand::AssembleLink(QDomElement element) const {
    QString type = element.attribute("type");
    if (type != "text/css")
        return;

    QString url = AbsoluteUrl(element.attribute("href"));
    QByteArray file;
    Download(url, &file, NULL);

    QDomElement style = element.ownerDocument().createElement("style");
    style.setAttribute("type", "text/css");
    style.setNodeValue(file);

    element.parentNode().replaceChild(style, element);
}

// Convert <img> tag 'src' attribute to base64
void QWebViewVisualizerSourceCommand::AssembleImg(QDomElement element) const {
    QString url = element.attribute("src");
    if (url.startsWith("data:"))
        return;

    element.setAttribute("src", DownloadAndEncode(url));
}

void QWebViewVisualizerSourceCommand::AssembleStyle(QDomElement element) const {
    QString type = element.attribute("type");
    if (type.length() != 0 && type != "text/css")
        return;

    QString value = element.text();
    value = AssembleStyle(value);
    element.firstChild().toText().setData(value);
}

void QWebViewVisualizerSourceCommand::AssembleStyle(QDomAttr attribute) const {
    QString value = attribute.value();
    value = AssembleStyle(value);
    attribute.setValue(value);
}

QString QWebViewVisualizerSourceCommand::AssembleStyle(const QString& value) const {
    QRegExp regex(":\\s*url\\(([^\\)]+)\\)");
    QString result;

    int lastMatchEnd = 0;
    int newMatchStart;
    while ((newMatchStart = regex.indexIn(value, lastMatchEnd)) != -1) {
        result += value.mid(lastMatchEnd, newMatchStart - lastMatchEnd);

        QString url = regex.cap(1);
        url = trimmed(url, " \t'\"");
        url = DownloadAndEncode(url);
        result += ":url('" + url + "')";

        lastMatchEnd = newMatchStart + regex.matchedLength();
    }

    if (lastMatchEnd != value.length() - 1) {
        result += value.mid(lastMatchEnd, value.length() - lastMatchEnd);
    }
    return result;
}

// Remove <script> tags
void QWebViewVisualizerSourceCommand::RemoveScripts(QDomElement element) const {
    std::vector<QDomNode> scripts;

    QDomNodeList children = element.childNodes();
    for (int childIndex = 0; childIndex < children.length(); childIndex++) {
        QDomNode child = children.at(childIndex);
        if (child.nodeType() == QDomNode::ElementNode) {
            QDomElement childElement = child.toElement();
            if (childElement.tagName() == "script") {
                scripts.push_back(childElement);
            }
        }
    }

    for (std::vector<QDomNode>::const_iterator it = scripts.begin(); it != scripts.end(); it++) {
        element.removeChild(*it);
    }
}

QString QWebViewVisualizerSourceCommand::AbsoluteUrl(const QString& url) const {
    if (url.contains("://"))
        return url;

    if (url.startsWith("//"))
        return view_->url().scheme() + ":" + url;
    else if (url.startsWith('/'))
        return view_->url().scheme() + "://" + view_->url().host() + url;
    else
        return view_->url().scheme() + "://" + view_->url().host() + view_->url().path() + url;
}

void QWebViewVisualizerSourceCommand::Download(const QString& url, QByteArray* buffer, QString* contentType) const {
    QString absoluteUrl = AbsoluteUrl(url);
    QSharedPointer<QNetworkReply> reply(view_->page()->networkAccessManager()->get(QNetworkRequest(absoluteUrl)));

    QEventLoop loop;
    QObject::connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    *buffer = reply->readAll();
    *contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
}

QString QWebViewVisualizerSourceCommand::DownloadAndEncode(const QString& url) const {
    QByteArray file;
    QString contentType = "image";
    Download(url, &file, &contentType);

    QString base64 = file.toBase64();
    return "data:" + contentType + ";base64," + base64;
}

QString QWebViewVisualizerSourceCommand::trimmed(const QString& str, const QString& symbols) {
    int start = 0;
    while (start < str.length() && symbols.contains(str.at(start))) {
        start++;
    }

    int end = str.length() - 1;
    while (end >= 0 && symbols.contains(str.at(end))) {
        end--;
    }

    return str.mid(start, end + 1 - start);
}

void QWebViewVisualizerSourceCommand::DownloadFinished() {
}

const ViewType QWebViewCmdExecutorCreator::WEB_VIEW_TYPE = 0x13f0;

QWebViewCmdExecutorCreator::QWebViewCmdExecutorCreator()
    : ViewCmdExecutorCreator() { }

QWebViewCmdExecutorCreator::~QWebViewCmdExecutorCreator() {}

ViewCmdExecutor* QWebViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QWebView* pWebView = QWebViewUtil::getWebView(session, viewId);
    if (NULL != pWebView) {
        session->logger().Log(kFineLogLevel, "Web executor for view("+viewId.id()+")");
        return new QWebViewCmdExecutor(session, viewId);
    }

    return NULL;
}

bool QWebViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QWebView* pWebView = QWebViewUtil::getWebView(session, viewId);

    if (NULL != pWebView) {
        if (NULL != viewType) *viewType = WEB_VIEW_TYPE;
        return true;
    }

    return false;
}

QWebViewCmdExecutor::QWebViewCmdExecutor(Session* session, ViewId viewId)
    : QViewCmdExecutor(session, viewId) {
}
QWebViewCmdExecutor::~QWebViewCmdExecutor() {}

QWebView* QWebViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QWebView* pWebView = QWebViewUtil::getWebView(session_, viewId);

    if (NULL == pWebView) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such web view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pWebView;
}   

void QWebViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    QWebView* pWebView = getView(view_id_, error);

    if (NULL == pWebView) {

        *error = new Error(kNoSuchWindow);
        return;
    }
    *can = QWebViewUtil::isUrlSupported(pWebView, url, error);
}

void QWebViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    const char* kGetTitleScript =
        "function() {"
        "  if (document.title)"
        "    return document.title;"
        "  else"
        "    return document.URL;"
        "}";
    *error = ExecuteScriptAndParse(
                            GetFrame(view, FramePath()),
                            kGetTitleScript,
                            "getTitle",
                            new ListValue(),
                            CreateDirectValueParser(title));
}

void QWebViewCmdExecutor::GetWindowName(std::string* name, Error ** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                "function() { return window.name; }",
                "getWindowName",
                new ListValue(),
                CreateDirectValueParser(name));

    session_->logger().Log(kFineLogLevel, "GetWindowName - "+*name);
}

void QWebViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    const char* kGetWindowBoundsScript =
        "function() {"
        "  return {"
        "    'left': window.screenX,"
        "    'top': window.screenY,"
        "    'width': window.outerWidth,"
        "    'height': window.outerHeight"
        "  }"
        "}";
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                kGetWindowBoundsScript,
                "getWindowBoundsScript",
                new ListValue(),
                CreateDirectValueParser(bounds));
}

void QWebViewCmdExecutor::GoForward(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWebHistory *history = view->history();
    history->forward();
}

void QWebViewCmdExecutor::GoBack(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWebHistory *history = view->history();
    history->back();
}

void QWebViewCmdExecutor::Reload(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    // TODO: take into account page load strategy
    {
        // sync reload
        QPageLoader pageLoader(view);
        QEventLoop loop;
        view->stop();
        QObject::connect(&pageLoader, SIGNAL(loaded()),&loop,SLOT(quit()));
        pageLoader.reloadPage();
        if (pageLoader.isLoading()) {
            loop.exec();
        }

        session_->logger().Log(kFineLogLevel, "Web sync reload.");
    }
}

void QWebViewCmdExecutor::GetSource(std::string* source, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    const char* kSource =
        "function() {"
        "  return new XMLSerializer().serializeToString(document);"
        "}";

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                kSource,
                "getSource",
                new ListValue(),
                CreateDirectValueParser(source));
}

void QWebViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    bool is_displayed = false;
    IsElementDisplayed(element, true, &is_displayed, error);
    if (*error)
        return;
    
    if (!is_displayed) {
        *error = new Error(kElementNotVisible);
        return;
    }

    bool is_enabled = false;
    IsElementEnabled(element, &is_enabled, error);
    if (*error)
        return;
    
    if (!is_enabled) {
        *error = new Error(kInvalidElementState);
        return;
    }

    // Focus the target element in order to send keys to it.
    // First, the currently active element is blurred, if it is different from
    // the target element. We do not want to blur an element unnecessarily,
    // because this may cause us to lose the current cursor position in the
    // element.
    // Secondly, we focus the target element.
    // Thirdly, if the target element is newly focused and is a text input, we
    // set the cursor position at the end.
    // Fourthly, we check if the new active element is the target element. If not,
    // we throw an error.
    // Additional notes:
    //   - |document.activeElement| is the currently focused element, or body if
    //     no element is focused
    //   - Even if |document.hasFocus()| returns true and the active element is
    //     the body, sometimes we still need to focus the body element for send
    //     keys to work. Not sure why
    //   - You cannot focus a descendant of a content editable node
    // TODO(jleyba): Update this to use the correct atom.
    const char* kFocusScript =
        "function(elem) {"
        "  var doc = elem.ownerDocument || elem;"
        "  var prevActiveElem = doc.activeElement;"
        "  if (elem != prevActiveElem && prevActiveElem)"
        "    prevActiveElem.blur();"
        "  elem.focus();"
        "  if (elem != prevActiveElem && elem.value && elem.value.length &&"
        "      elem.setSelectionRange) {"
        "    elem.setSelectionRange(elem.value.length, elem.value.length);"
        "  }"
        "  if (elem != doc.activeElement)"
        "    throw new Error('Failed to send keys because cannot focus element');"
        "}";
    *error = ExecuteScriptAndParse(GetFrame(view, session_->current_frame()),
                                kFocusScript,
                                "focusElement",
                                CreateListValueFrom(element),
                                CreateDirectValueParser(kSkipParsing));
    if (*error)
        return;

    std::string err_msg;
    std::vector<QKeyEvent> key_events;
    int modifiers = Qt::NoModifier;

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               true,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "ElementSendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "ElementSendKeys - cant convert keys:"+err_msg);
        return;
    }

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
        qApp->sendEvent(view, &(*it));
        ++it;
    }
}

void QWebViewCmdExecutor::MouseDoubleClick(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view, dbEvent);
    QApplication::postEvent(view, releaseEvent);
}

void QWebViewCmdExecutor::MouseButtonUp(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view, releaseEvent);
}

void QWebViewCmdExecutor::MouseButtonDown(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(view, pressEvent);
}

void QWebViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view, pressEvent);
    QApplication::postEvent(view, releaseEvent);
    if (Qt::RightButton == mouseButton) {
        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point);
        QApplication::postEvent(view, contextEvent);
    }
}

class QCursorMark : public QWidget
{
public:
    explicit QCursorMark(QWidget* parent)
        : QWidget(parent)
    {
        resize(2 * RADIUS, 2 * RADIUS);
    }

    virtual void paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::red));

        QBrush brush = painter.brush();
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);

        painter.drawEllipse(QPoint(RADIUS, RADIUS), RADIUS, RADIUS);
    }

private:
    static const int RADIUS = 5;
};

void QWebViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

    QPoint point = ConvertPointToQPoint(prev_pos);

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view, moveEvent);

    session_->set_mouse_position(prev_pos);
    DrawMark(point);
}

void QWebViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location;
    GetElementLocationInView(element, &location, error);
    if (*error)
        return;

    location.Offset(x_offset, y_offset);

    QPoint point = ConvertPointToQPoint(location);

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view, moveEvent);

    session_->set_mouse_position(location);
    DrawMark(point);
}

void QWebViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location;

    // element is specified, calculate the coordinate.
    GetElementLocationInView(element, &location, error);
    if (*error)
        return;
    
    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPoint point = ConvertPointToQPoint(location);

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view, moveEvent);

    session_->set_mouse_position(location);
    DrawMark(point);
}

void QWebViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string tag_name;
    GetElementTagName(element, &tag_name, error);
    if (*error)
        return;

    if (tag_name == "option") {
        const char* kCanOptionBeToggledScript =
            "function(option) {"
            "  for (var parent = option.parentElement;"
            "       parent;"
            "       parent = parent.parentElement) {"
            "    if (parent.tagName.toLowerCase() == 'select') {"
            "      return parent.multiple;"
            "    }"
            "  }"
            "  throw new Error('Option element is not in a select');"
            "}";
        bool can_be_toggled;
        *error = ExecuteScriptAndParse(
            GetFrame(view, session_->current_frame()),
            kCanOptionBeToggledScript,
            "canOptionBeToggled",
            CreateListValueFrom(element),
            CreateDirectValueParser(&can_be_toggled));
        if (*error)
            return;

        if (can_be_toggled) {
            *error = ToggleOptionElement(element);
        } else {
            SetOptionElementSelected(element, true, error);
        }
    } else {
        Point location;

        *error = GetClickableLocation(view, element, &location);
        if (!(*error)) {
            session_->logger().Log(kFineLogLevel,
                base::StringPrintf("ClickElement at pos (%f, %f).", location.x(), location.y()));
            session_->set_mouse_position(location);
            MouseClick(kLeftButton, error);
        }
    }

}

void QWebViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                    GetFrame(view, session_->current_frame()),
                    atoms::asString(atoms::GET_ATTRIBUTE),
                    "getAttribute",
                    CreateListValueFrom(element, key),
                    CreateDirectValueParser(value));
}

void QWebViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string script = base::StringPrintf(
        "(%s).apply(null, arguments);", atoms::asString(atoms::CLEAR).c_str());

    ListValue args;
    args.Append(element.ToValue());

    Value* value = NULL;
    *error = ExecuteScript(
                    GetFrame(view, session_->current_frame()),
                    script,
                    &args,
                    &value);
    scoped_ptr<Value> scoped_value(value);
}

void QWebViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::IS_DISPLAYED),
                "isDisplayed",
                CreateListValueFrom(element, ignore_opacity),
                CreateDirectValueParser(is_displayed));
}

void QWebViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::IS_ENABLED),
                "isEnabled",
                CreateListValueFrom(element),
                CreateDirectValueParser(is_enabled));
}

void QWebViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    const std::string script = "function(el1, el2) { return el1 == el2; }";

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                script,
                "elementEquals",
                CreateListValueFrom(element1, element2),
                CreateDirectValueParser(is_equal));
}

void QWebViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::GET_LOCATION),
                "getLocation",
                CreateListValueFrom(element),
                CreateDirectValueParser(location));
}

void QWebViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;
        
    *error = GetElementRegionInView(
            view,
            element,
            Rect(Point(0, 0), size),
            false, // cente,
            false, // verify_clickable_at_middle
            location);
}

void QWebViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                "function(elem) { return elem.tagName.toLowerCase() }",
                "getElementTagName",
                CreateListValueFrom(element),
                CreateDirectValueParser(tag_name));    
}

void QWebViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::IS_SELECTED),
                "isSelected",
                CreateListValueFrom(element),
                CreateDirectValueParser(is_selected));
}

void QWebViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    // This wrapper ensures the script is started successfully and
    // allows for an alert to happen when the option selection occurs.
    // See selenium bug 2671.
    const char kSetSelectedWrapper[] =
        "var args = [].slice.apply(arguments);"
        "args[args.length - 1]();"
        "return (%s).apply(null, args.slice(0, args.length - 1));";
    Value* value = NULL;
    *error = ExecuteAsyncScript(
                GetFrame(view, session_->current_frame()),
                base::StringPrintf(kSetSelectedWrapper,
                         atoms::asString(atoms::CLICK).c_str()),
                CreateListValueFrom(element, selected),
                &value);
    scoped_ptr<Value> scoped_value(value);
}

void QWebViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::GET_SIZE),
                "getSize",
                CreateListValueFrom(element),
                CreateDirectValueParser(size));
}

void QWebViewCmdExecutor::ElementSubmit(const ElementId& element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string script = base::StringPrintf(
        "(%s).apply(null, arguments);", atoms::asString(atoms::SUBMIT).c_str());

    ListValue args;
    args.Append(element.ToValue());

    Value* result = NULL;
    *error = ExecuteScript(
                    GetFrame(view, session_->current_frame()),
                    script, &args, &result);
    scoped_ptr<Value> scoped_value(result);
}

void QWebViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::GET_TEXT),
                "getText",
                CreateListValueFrom(element),
                CreateDirectValueParser(element_text));
}

void QWebViewCmdExecutor::GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string script = base::StringPrintf(
        "return (%s).apply(null, arguments);",
        atoms::asString(atoms::GET_EFFECTIVE_STYLE).c_str());

    ListValue args;
    args.Append(element.ToValue());
    args.Append(Value::CreateStringValue(property));

    *error = ExecuteScript(
                GetFrame(view, session_->current_frame()),
                script, &args, value);
}

void QWebViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::vector<ElementId> elements;
    *error = FindElementsHelper(
                GetFrame(view, session_->current_frame()),
                root_element, locator, query, true, &elements);
    if (!(*error))
      *element = elements[0];
}

void QWebViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = FindElementsHelper(
                GetFrame(view, session_->current_frame()),
                root_element, locator, query, false, elements);
}

void QWebViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string script = "function() { return document.activeElement || document.body; }";

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                script,
                "activeElement",
                new ListValue(),
                CreateDirectValueParser(element));
}

void QWebViewCmdExecutor::SwitchTo(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->set_current_view(view_id_);

    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");

    AddBrowserLoggerToView(view);

    // reset frame path
    session_->frame_elements_.clear();
    session_->set_current_frame(FramePath());
}

void QWebViewCmdExecutor::SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string script =
        "function(arg) {"
        "  var xpath = '(/html/body//iframe|/html/frameset/frame)';"
        "  var sub = function(s) { return s.replace(/\\$/g, arg); };"
        "  xpath += sub('[@name=\"$\" or @id=\"$\"]');"
        "  return document.evaluate(xpath, document, null, "
        "      XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;"
        "}";
    *error = SwitchToFrameWithJavaScriptLocatedFrame(
                    view,
                    GetFrame(view, session_->current_frame()),
                    script, CreateListValueFrom(name_or_id));
}

void QWebViewCmdExecutor::SwitchToFrameWithIndex(int index, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    // We cannot simply index into window.frames because we need to know the
    // tagName of the frameElement. If child frame N is from another domain, then
    // the following will run afoul of the same origin policy:
    //   window.frames[N].frameElement;
    // Instead of indexing window.frames, we use an XPath expression to index
    // into the list of all IFRAME and FRAME elements on the page - if we find
    // something, then that XPath expression can be used as the new frame's XPath.
    std::string script =
        "function(index) {"
        "  var xpathIndex = '[' + (index + 1) + ']';"
        "  var xpath = '(/html/body//iframe|/html/frameset/frame)' + "
        "              xpathIndex;"
        "  return document.evaluate(xpath, document, null, "
        "      XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;"
        "}";
    *error = SwitchToFrameWithJavaScriptLocatedFrame(
                    view,
                    GetFrame(view, session_->current_frame()),
                    script, CreateListValueFrom(index));
}

void QWebViewCmdExecutor::SwitchToFrameWithElement(const ElementId& element, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    // TODO(jleyba): Extract this, and the other frame switch methods to an atom.
    std::string script =
        "function(elem) {"
        "  if (elem.nodeType != 1 || !/^i?frame$/i.test(elem.tagName)) {"
        "    console.error('Element is not a frame');"
        "    return null;"
        "  }"
        "  for (var i = 0; i < window.frames.length; i++) {"
        "    if (elem.contentWindow == window.frames[i]) {"
        "      return elem;"
        "    }"
        "  }"
        "  console.info('Frame is not connected to this DOM tree');"
        "  return null;"
        "}";
    *error = SwitchToFrameWithJavaScriptLocatedFrame(
            view,
            GetFrame(view, session_->current_frame()),
            script, CreateListValueFrom(element));
}

void QWebViewCmdExecutor::SwitchToTopFrame(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->frame_elements_.clear();
    session_->set_current_frame(FramePath());
}

void QWebViewCmdExecutor::SwitchToTopFrameIfCurrentFrameInvalid(Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::vector<std::string> components;
    session_->current_frame().GetComponents(&components);
    if (session_->frame_elements_.size() != components.size()) {
        *error = new Error(kUnknownError,
                     "Frame element vector out of sync with frame path");
        return;
    }

    FramePath frame_path;
    // Start from the root path and check that each frame element that makes
    // up the current frame target is valid by executing an empty script.
    // This code should not execute script in any frame before making sure the
    // frame element is valid, otherwise the automation hangs until a timeout.
    for (size_t i = 0; i < session_->frame_elements_.size(); ++i) {
        scoped_ptr<Error> scoped_error(ExecuteScriptAndParse(
            GetFrame(view, frame_path),
            "function(){ }",
            "emptyScript",
            CreateListValueFrom(session_->frame_elements_[i]),
            CreateDirectValueParser(kSkipParsing)));
        if (scoped_error.get() && scoped_error->code() == kStaleElementReference) {
            Error* terr = NULL;
            SwitchToTopFrame(&terr);
            scoped_ptr<Error> tmp(terr);
        } else if (scoped_error.get()) {
            *error = scoped_error.release();
            return;
        }
        frame_path = frame_path.Append(components[i]);
    }
}

void QWebViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QUrl address(QString(url.c_str()));

    if (sync) {
        QPageLoader pageLoader(view);
        QEventLoop loop;
        view->stop();
        QObject::connect(&pageLoader, SIGNAL(loaded()),&loop,SLOT(quit()));
        pageLoader.loadPage(address);
        if (pageLoader.isLoading()) {
            loop.exec();
        }

        session_->logger().Log(kFineLogLevel, "Web sync load - " + url);
    } else {
        view->load(address);

        session_->logger().Log(kFineLogLevel, "Web async load - " + url);
    }
}

void QWebViewCmdExecutor::GetURL(std::string* url, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                    GetFrame(view, session_->current_frame()),
                    "function() { return document.URL }",
                    "getUrl",
                    new ListValue(),
                    CreateDirectValueParser(url));

}

void QWebViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScript(
                GetFrame(view, session_->current_frame()),
                script,
                args,
                value);
}

void QWebViewCmdExecutor::ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteAsyncScript(
                GetFrame(view, session_->current_frame()),
                script,
                args,
                value);
}

void QWebViewCmdExecutor::GetAppCacheStatus(int* status, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                atoms::asString(atoms::GET_APPCACHE_STATUS),
                "getAppcacheStatus",
                new ListValue(),
                CreateDirectValueParser(status));
}

void QWebViewCmdExecutor::GetCookies(const std::string& url, ListValue** cookies, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QString qUrl = url.c_str();
    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (NULL == jar) {
        *error = new Error(kUnknownError, "No active NetworkCookieJar");
        return;
    }

    QList<QNetworkCookie> cookies_list = jar->cookiesForUrl(QUrl(qUrl));

    ListValue* list = new ListValue();
    for (int i = 0; i < cookies_list.size(); ++i) {
        const QNetworkCookie& cookie = cookies_list[i];

        DictionaryValue* cookie_dict = new DictionaryValue();
        cookie_dict->SetString("name", cookie.name().data());
        cookie_dict->SetString("value", cookie.value().data());
        cookie_dict->SetString("path", cookie.path().toStdString());
        cookie_dict->SetString("domain", cookie.domain().toStdString());
        cookie_dict->SetBoolean("secure", cookie.isSecure());
        cookie_dict->SetBoolean("http_only", cookie.isHttpOnly());
        if (!cookie.isSessionCookie())
            cookie_dict->SetDouble("expiry", (double)cookie.expirationDate().toTime_t());

        list->Append(cookie_dict);
    }

    scoped_ptr<Value> cookies_value(list);
    if (!cookies_value->IsType(Value::TYPE_LIST)) {
        *error = new Error(kUnknownError);
        return;
    }

    *cookies = static_cast<ListValue*>(cookies_value.release());
}

void QWebViewCmdExecutor::SetCookie(const std::string& url, DictionaryValue* cookie_dict, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QList<QNetworkCookie> cookie_list;
    std::string name, value;
    std::string domain;
    std::string path = "/";
    bool secure = false;
    double expiry = 0;
    bool http_only = false;

    if (!cookie_dict->GetString("name", &name)) {
        *error = new Error(kUnknownError, "'name' missing or invalid");
        return;
    }

    if (!cookie_dict->GetString("value", &value)) {
        *error = new Error(kUnknownError, "'value' missing or invalid");
        return;
    }

    QNetworkCookie cookie(QByteArray(name.c_str()), QByteArray(value.c_str()));

    if (cookie_dict->HasKey("domain")) {
        if (!cookie_dict->GetString("domain", &domain)) {
            *error = new Error(kUnknownError, "optional 'domain' invalid");
            return;
        }

        session_->logger().Log(kFineLogLevel, "SetCookie - domain:" + domain);

        // TODO: check why it fails here
        //cookie.setDomain(QString(domain.c_str()));
    }

    if (cookie_dict->HasKey("path")) {
        if (!cookie_dict->GetString("path", &path)) {
            *error = new Error(kUnknownError, "optional 'path' invalid");
            return;
        }

        cookie.setPath(QString(path.data()));
    }

    if (cookie_dict->HasKey("secure")) {
        if (!cookie_dict->GetBoolean("secure", &secure)) {
            *error = new Error(kUnknownError, "optional 'secure' invalid");
            return;
        }

        cookie.setSecure(secure);
    }

    if (cookie_dict->HasKey("expiry")) {
        if (!cookie_dict->GetDouble("expiry", &expiry)) {
            *error = new Error(kUnknownError, "optional 'expiry' invalid");
            return;
        }

        time_t time = (base::Time::FromDoubleT(expiry)).ToTimeT();
        //qDebug()<<"[WD]:" << "time=[" << time <<"]";

        QDateTime qtime;
        qtime.setTime_t(time);

        if (qtime > QDateTime::currentDateTime()) {
            session_->logger().Log(kFineLogLevel, "SetCookie - adding cookie");
            cookie.setExpirationDate(qtime);
        } else {
            session_->logger().Log(kWarningLogLevel, "SetCookie - cookie expired");
            *error = new Error(kUnknownError, "Could not set expired cookie");
            return;
        }
    }

    if (cookie_dict->HasKey("http_only")) {
        if (!cookie_dict->GetBoolean("http_only", &http_only)) {
            *error = new Error(kUnknownError, "optional 'http_only' invalid");
            return;
        }

        cookie.setHttpOnly(http_only);
    }

    cookie_list.append(cookie);

    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (!jar) {
        jar = new QNetworkCookieJar();
        view->page()->networkAccessManager()->setCookieJar(jar);
    }

    if (!jar->setCookiesFromUrl(cookie_list, QUrl(url.c_str()))) {
        *error = new Error(kUnknownError, "Could not set the cookie");
    }
}

void QWebViewCmdExecutor::DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QString qUrl = url.c_str();
    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (NULL == jar) {
        *error = new Error(kUnknownError, "No active NetworkCookieJar");
        return;
    }

    QList<QNetworkCookie> cookies = jar->cookiesForUrl(QUrl(qUrl));

    if (cookies.isEmpty())
        return;

    QList<QNetworkCookie>::Iterator it = cookies.begin();
    QList<QNetworkCookie>::Iterator end = cookies.end();

    while (it != end) {
        std::string str_name(it->name().data());

        if (str_name == cookie_name) {
            it = cookies.erase(it);

            // NOTE: use QNetworkCookieJar::deleteCookie in case QT 5.0
            QNetworkCookieJar * newCookieJar = new QNetworkCookieJar();
            newCookieJar->setCookiesFromUrl(cookies, QUrl(qUrl));
            view->page()->networkAccessManager()->setCookieJar(newCookieJar);
            return;
        } else {
            it++;
        }
    }

    *error = new Error(kUnknownError, "No such cookie");
}

void QWebViewCmdExecutor::GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_KEYS
                                    : atoms::GET_SESSION_STORAGE_KEYS);
    *error = ExecuteScriptAndParse(
                    GetFrame(view, session_->current_frame()),
                    js,
                    "getStorageKeys",
                    new ListValue(),
                    CreateDirectValueParser(keys));
}

void QWebViewCmdExecutor::SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::SET_LOCAL_STORAGE_ITEM
                                : atoms::SET_SESSION_STORAGE_ITEM);
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                js,
                "setStorageItem",
                CreateListValueFrom(key, value),
                CreateDirectValueParser(kSkipParsing));
}

void QWebViewCmdExecutor::ClearStorage(StorageType type, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::CLEAR_LOCAL_STORAGE
                                : atoms::CLEAR_SESSION_STORAGE);
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                js,
                "clearStorage",
                new ListValue(),
                CreateDirectValueParser(kSkipParsing));
}

void QWebViewCmdExecutor::GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_ITEM
                                : atoms::GET_SESSION_STORAGE_ITEM);
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                js,
                "getStorageItem",
                CreateListValueFrom(key),
                CreateDirectValueParser(value));
}

void QWebViewCmdExecutor::RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::REMOVE_LOCAL_STORAGE_ITEM
                                : atoms::REMOVE_SESSION_STORAGE_ITEM);
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                js,
                "removeStorageItem",
                CreateListValueFrom(key),
                CreateDirectValueParser(value));
}

void QWebViewCmdExecutor::GetStorageSize(StorageType type, int* size, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_SIZE
                                : atoms::GET_SESSION_STORAGE_SIZE);
    *error = ExecuteScriptAndParse(
                GetFrame(view, session_->current_frame()),
                js,
                "getStorageSize",
                new ListValue(),
                CreateDirectValueParser(size));
}

void QWebViewCmdExecutor::TouchClick(const ElementId& element, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location;
    GetElementLocationInView(element, &location, error);
    if (*error)
        return;

    QPoint point = ConvertPointToQPoint(location);

    QList<QTouchEvent::TouchPoint> points;
    QTouchEvent::TouchPoint touchPoint(1);
    touchPoint.setPos(point);
    touchPoint.setScreenPos(view->mapToGlobal(point));
    touchPoint.setStartScreenPos(view->mapToGlobal(point));
    touchPoint.setState(Qt::TouchPointPressed);
    touchPoint.setPressure(1);
    points.append(touchPoint);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchBeginEvent = new QTouchEvent(QEvent::TouchBegin, &touchDevice, Qt::NoModifier, Qt::TouchPointPressed, points);
#else
    QTouchEvent *touchBeginEvent = new QTouchEvent(QEvent::TouchBegin, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointPressed, points);
#endif
    points.clear();
    touchPoint.setPos(point);
    touchPoint.setScreenPos(view->mapToGlobal(point));
    touchPoint.setStartScreenPos(view->mapToGlobal(point));
    touchPoint.setState(Qt::TouchPointReleased);
    touchPoint.setPressure(1);
    points.append(touchPoint);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchEndEvent = new QTouchEvent(QEvent::TouchEnd, &touchDevice, Qt::NoModifier, Qt::TouchPointReleased, points);
#else
    QTouchEvent *touchEndEvent = new QTouchEvent(QEvent::TouchEnd, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointReleased, points);
#endif

    QApplication::postEvent(view, touchBeginEvent);
    QApplication::postEvent(view, touchEndEvent);

    //additional we send mouse event for QWebView
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view, pressEvent);
    QApplication::postEvent(view, releaseEvent);

}

void QWebViewCmdExecutor::TouchDoubleClick(const ElementId& element, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location;
    GetElementLocationInView(element, &location, error);
    if (*error)
        return;

    QPoint point = ConvertPointToQPoint(location);

    view->setZoomFactor(2/view->zoomFactor());
    view->page()->mainFrame()->scroll(point.x(), point.y());
}

void QWebViewCmdExecutor::TouchDown(const int &x, const int &y, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(Point(x, y));

    QList<QTouchEvent::TouchPoint> points;
    QTouchEvent::TouchPoint touchPoint(1);
    touchPoint.setPos(point);
    touchPoint.setState(Qt::TouchPointPressed);
    touchPoint.setPressure(1);
    points.append(touchPoint);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchBeginEvent = new QTouchEvent(QEvent::TouchBegin, &touchDevice, Qt::NoModifier, Qt::TouchPointPressed, points);
#else
    QTouchEvent *touchBeginEvent = new QTouchEvent(QEvent::TouchBegin, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointPressed, points);
#endif

    QApplication::postEvent(view, touchBeginEvent);
}

void QWebViewCmdExecutor::TouchUp(const int &x, const int &y, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(Point(x, y));

    QList<QTouchEvent::TouchPoint> points;
    QTouchEvent::TouchPoint touchPoint(1);
    touchPoint.setPos(point);
    touchPoint.setState(Qt::TouchPointPressed);
    touchPoint.setPressure(1);
    points.append(touchPoint);


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchEndEvent = new QTouchEvent(QEvent::TouchEnd, &touchDevice, Qt::NoModifier, Qt::TouchPointReleased, points);
#else
    QTouchEvent *touchEndEvent = new QTouchEvent(QEvent::TouchEnd, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointReleased, points);
#endif

    QApplication::postEvent(view, touchEndEvent);
}

void QWebViewCmdExecutor::TouchMove(const int &x, const int &y, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(Point(x, y));

    QList<QTouchEvent::TouchPoint> points;
    QTouchEvent::TouchPoint touchPoint(1);
    touchPoint.setPos(point);
    touchPoint.setState(Qt::TouchPointMoved);
    touchPoint.setPressure(1);
    points.append(touchPoint);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchEvent *touchMoveEvent = new QTouchEvent(QEvent::TouchUpdate, &touchDevice, Qt::NoModifier, Qt::TouchPointMoved, points);
#else
    QTouchEvent *touchMoveEvent = new QTouchEvent(QEvent::TouchUpdate, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointMoved, points);
#endif

    QApplication::postEvent(view, touchMoveEvent);

}

void QWebViewCmdExecutor::TouchLongClick(const ElementId& element, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location;
    GetElementLocationInView(element, &location, error);
    if (*error)
        return;

    QPoint point = ConvertPointToQPoint(location);

//    QList<QTouchEvent::TouchPoint> points;
//    QTouchEvent::TouchPoint touchPoint(1);
//    touchPoint.setPos(point);
//    touchPoint.setScreenPos(view->mapToGlobal(point));
//    touchPoint.setStartScreenPos(view->mapToGlobal(point));
//    touchPoint.setState(Qt::TouchPointPressed);
//    touchPoint.setPressure(1);
//    points.append(touchPoint);
//    QTouchEvent *touchBeginEvent = new QTouchEvent(QEvent::TouchBegin, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointPressed, points);

//    points.clear();
//    touchPoint.setPos(point);
//    touchPoint.setScreenPos(view->mapToGlobal(point));
//    touchPoint.setStartScreenPos(view->mapToGlobal(point));
//    touchPoint.setState(Qt::TouchPointReleased);
//    touchPoint.setPressure(1);
//    points.append(touchPoint);
//    QTouchEvent *touchEndEvent = new QTouchEvent(QEvent::TouchEnd, QTouchEvent::TouchScreen, Qt::NoModifier, Qt::TouchPointReleased, points);

//    QTimer::singleShot(1000, &loop, SLOT(quit()));
//    QApplication::postEvent(view, touchBeginEvent);
//    loop.exec(QEventLoop::ExcludeUserInputEvents);
//    QApplication::postEvent(view, touchEndEvent);

    QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Other, point, view->mapToGlobal(point));
    qApp->postEvent(view, contextEvent);
}

void QWebViewCmdExecutor::TouchScroll(const int &xoffset, const int &yoffset, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    view->page()->mainFrame()->scroll(xoffset, yoffset);
}

void QWebViewCmdExecutor::TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;
    view->page()->mainFrame()->scroll(-xoffset, -yoffset);
}

void QWebViewCmdExecutor::TouchFlick(const int &xSpeed, const int &ySpeed, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    view->page()->mainFrame()->scroll(xSpeed*3, ySpeed*3);
}

void QWebViewCmdExecutor::TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    view->page()->mainFrame()->scroll(-xoffset*(speed+1), -yoffset*(speed+1));
}

void QWebViewCmdExecutor::GetBrowserLog(base::ListValue** browserLog, Error **error)
{
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    BrowserLogHandler* logHandler = view->findChild<BrowserLogHandler*>();
    if (NULL == logHandler)
        return;

    *browserLog = logHandler->getLog();
}

void QWebViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    QWebView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWebViewVisualizerSourceCommand command(this, session_, view);
    command.Execute(source, error);
}

QWebFrame* QWebViewCmdExecutor::FindFrameByPath(QWebFrame* parent, const FramePath &frame_path) {
    if (frame_path.value().empty())
        return NULL;

    QStringList frame_path_list = QString(frame_path.value().c_str()).split("\n");
    QWebFrame* frame = parent;
    QString frame_path_str;

    for(int i = 0; i < frame_path_list.size(); ++i)
    {
        frame_path_str = frame_path_list.at(i);
        foreach(QWebFrame* pFrame, frame->childFrames())
        {
            QString frame_id = pFrame->property("frame_id").toString();
            if(frame_id == frame_path_str)
            {
                if (i == (frame_path_list.size() - 1))
                    return pFrame;
                else
                {
                    frame = pFrame;
                    break;
                }
            }
        }
    }
    return NULL;
}

Error* QWebViewCmdExecutor::ExecuteScriptAndParse(QWebFrame* frame,
                                    const std::string& anonymous_func_script,
                                    const std::string& script_name,
                                    const ListValue* args,
                                    const ValueParser* parser) {
    scoped_ptr<const ListValue> scoped_args(args);
    scoped_ptr<const ValueParser> scoped_parser(parser);
    std::string called_script = base::StringPrintf(
        "return (%s).apply(null, arguments);", anonymous_func_script.c_str());
    Value* unscoped_value = NULL;
    Error* error = ExecuteScript(frame, called_script, args, &unscoped_value);
    if (error) {
        error->AddDetails(script_name + " execution failed");
        return error;
    }

    scoped_ptr<Value> value(unscoped_value);
    std::string error_msg;
    if (!parser->Parse(value.get())) {
        error_msg = base::StringPrintf("%s returned invalid value: %s",
            script_name.c_str(), JsonStringify(value.get()).c_str());
        return new Error(kUnknownError, error_msg);
    }
    return NULL;
}

Error* QWebViewCmdExecutor::ExecuteAsyncScript(QWebFrame* frame,
                                   const std::string& script,
                                   const ListValue* const args,
                                   Value** value) {

    std::string args_as_json;
    base::JSONWriter::Write(static_cast<const Value* const>(args),
                            &args_as_json);

    int timeout_ms = session_->async_script_timeout();

    // Every injected script is fed through the executeScript atom. This atom
    // will catch any errors that are thrown and convert them to the
    // appropriate JSON structure.
    std::string jscript = base::StringPrintf(
        "(%s).apply(null, [function(){%s},%s,%d,%s,true]);",
        atoms::asString(atoms::EXECUTE_ASYNC_SCRIPT).c_str(),
        script.c_str(),
        args_as_json.c_str(),
        timeout_ms,
        "function(result) {jsnotify.setResult(result);}");

    return ExecuteScriptAndParseValue(frame, jscript, value, true);
}

Error* QWebViewCmdExecutor::ExecuteScript(QWebFrame* frame,
                              const std::string& script,
                              const ListValue* const args,
                              Value** value) {
    std::string args_as_json;
    base::JSONWriter::Write(static_cast<const Value* const>(args),
                            &args_as_json);

    // Every injected script is fed through the executeScript atom. This atom
    // will catch any errors that are thrown and convert them to the
    // appropriate JSON structure.
    std::string jscript = base::StringPrintf(
        "function runScript() {return %s.apply(null,"
        "[function(){%s\n},%s,true])}; runScript()",
        atoms::asString(atoms::EXECUTE_SCRIPT).c_str(), script.c_str(),
        args_as_json.c_str());

    return ExecuteScriptAndParseValue(frame, jscript, value, false);
}

Error* QWebViewCmdExecutor::ExecuteScriptAndParseValue(QWebFrame* frame,
                                           const std::string& script,
                                           Value** script_result, bool isAsync) {
    std::string response_json;
    Error* error = ExecuteScriptImpl(frame, script, &response_json, isAsync);
    if (error)
        return error;

    scoped_ptr<Value> value(base::JSONReader::ReadAndReturnError(
        response_json, base::JSON_ALLOW_TRAILING_COMMAS, NULL, NULL));
    if (!value.get())
        return new Error(kUnknownError, "Failed to parse script result");
    if (value->GetType() != Value::TYPE_DICTIONARY)
        return new Error(kUnknownError, "Execute script returned non-dict: " +
                         JsonStringify(value.get()));
    DictionaryValue* result_dict = static_cast<DictionaryValue*>(value.get());

    int status;
    if (!result_dict->GetInteger("status", &status))
        return new Error(kUnknownError, "Execute script did not return status: " +
                         JsonStringify(result_dict));
    ErrorCode code = static_cast<ErrorCode>(status);
    if (code != kSuccess) {
        DictionaryValue* error_dict;
        std::string error_msg;
        if (result_dict->GetDictionary("value", &error_dict))
            error_dict->GetString("message", &error_msg);
        if (error_msg.empty())
            error_msg = "Script failed with error code: " + base::IntToString(code);
        return new Error(code, error_msg);
    }

    Value* tmp;
    if (result_dict->Get("value", &tmp)) {
        *script_result= tmp->DeepCopy();
    } else {
        // "value" was not defined in the returned dictionary; set to null.
        *script_result= Value::CreateNullValue();
    }
    return NULL;
}

Error* QWebViewCmdExecutor::ExecuteScriptImpl(QWebFrame* frame,
                               const std::string &script,
                               std::string *result,
                               bool isAsync)
{
    std::string res;
    if (isAsync)
    {
        QEventLoop loop;
        JSNotifier* jsNotify = new JSNotifier();
        frame->addToJavaScriptWindowObject("jsnotify", jsNotify );
        QObject::connect(jsNotify, SIGNAL(completed()), &loop, SLOT(quit()));
        frame->evaluateJavaScript(script.c_str());
        if (!jsNotify->IsCompleted())
            loop.exec();
        res = jsNotify->getResult().toString().toStdString();
        jsNotify->deleteLater();
    }
    else
    {
        QVariant f1result = frame->evaluateJavaScript(script.c_str());
        res = f1result.toString().toStdString();
    }
    *result = res;

    session_->logger().Log(kFineLogLevel, "ExecuteScriptImpl - "+res);

    return NULL;
}

QWebFrame* QWebViewCmdExecutor::GetFrame(QWebView* view, const FramePath& frame_path) {
    QWebFrame* frame = FindFrameByPath(view->page()->mainFrame(), frame_path);
    if (frame == NULL)
        frame = view->page()->mainFrame();

    return frame;
}

Error* QWebViewCmdExecutor::FindElementsHelper(QWebFrame* frame,
                                   const ElementId& root_element,
                                   const std::string& locator,
                                   const std::string& query,
                                   bool find_one,
                                   std::vector<ElementId>* elements) {
    CHECK(root_element.is_valid());
    base::Time start_time = base::Time::Now();
    while (true) {
        std::vector<ElementId> temp_elements;
        Error* error = ExecuteFindElementScriptAndParse(
                    frame, root_element, locator, query, find_one, &temp_elements);
        if (error)
            return error;

        if (temp_elements.size() > 0u) {
            elements->swap(temp_elements);
            break;
        }

        if ((base::Time::Now() - start_time).InMilliseconds() > session_->implicit_wait()) {
            if (find_one)
                return new Error(kNoSuchElement);
            break;
        }

        // non-blocking sleep for 50 ms
        {
            QEventLoop loop;
            QTimer timer;
     
            timer.setSingleShot(true);
            QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
     
            timer.start(50); //your predefined timeout
            loop.exec();
        }
        //base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(50));
    }
    return NULL;
}

Error* QWebViewCmdExecutor::ExecuteFindElementScriptAndParse(
        QWebFrame* frame,
        const ElementId& root_element,
        const std::string& locator,
        const std::string& query,
        bool find_one,
        std::vector<ElementId>* elements) {
    CHECK(root_element.is_valid());

    class FindElementsParser : public ValueParser {
    public:
        explicit FindElementsParser(std::vector<ElementId>* elements)
            : elements_(elements) { }

        virtual ~FindElementsParser() { }

        virtual bool Parse(base::Value* value) const OVERRIDE {
            if (!value->IsType(Value::TYPE_LIST))
                return false;
            ListValue* list = static_cast<ListValue*>(value);
            for (size_t i = 0; i < list->GetSize(); ++i) {
                ElementId element;
                Value* element_value = NULL;
                if (!list->Get(i, &element_value))
                    return false;
                if (!SetFromValue(element_value, &element))
                    return false;
                elements_->push_back(element);
            }
            return true;
        }
    private:
        std::vector<ElementId>* elements_;
    };

    class FindElementParser : public ValueParser {
    public:
        explicit FindElementParser(std::vector<ElementId>* elements)
            : elements_(elements) { }

        virtual ~FindElementParser() { }

        virtual bool Parse(base::Value* value) const OVERRIDE {
            if (value->IsType(Value::TYPE_NULL))
                return true;
            ElementId element;
            bool set = SetFromValue(value, &element);
            if (set)
                elements_->push_back(element);
            return set;
        }
    private:
        std::vector<ElementId>* elements_;
    };

    DictionaryValue locator_dict;
    locator_dict.SetString(locator, query);
    std::vector<ElementId> temp_elements;
    Error* error = NULL;
    if (find_one) {
        error = ExecuteScriptAndParse(
            frame,
            atoms::asString(atoms::FIND_ELEMENT),
            "findElement",
            CreateListValueFrom(&locator_dict, root_element),
            new FindElementParser(&temp_elements));
    } else {
        error = ExecuteScriptAndParse(
            frame,
            atoms::asString(atoms::FIND_ELEMENTS),
            "findElements",
            CreateListValueFrom(&locator_dict, root_element),
            new FindElementsParser(&temp_elements));
    }
    if (!error)
        elements->swap(temp_elements);
    return error;
}

Error* QWebViewCmdExecutor::GetElementRegionInView(
        QWebView* view,
        const ElementId& element,
        const Rect& region,
        bool center,
        bool verify_clickable_at_middle,
        Point* location) {

    CHECK(element.is_valid());

    Point region_offset = region.origin();
    Size region_size = region.size();

    Error* error = GetElementRegionInViewHelper(
                        GetFrame(view, session_->current_frame()),
                        element, region, center, verify_clickable_at_middle,
                        &region_offset);
    if (error)
        return error;

    for (FramePath frame_path = session_->current_frame();
        frame_path.IsSubframe();
        frame_path = frame_path.Parent()) {

        // Find the frame element for the current frame path.
        ElementId frame_element;
        std::string frameBasePath = base::StringPrintf("//*[@wd_frame_id_ = '%s']", frame_path.BaseName().value().c_str());

        QWebFrame* cur_frame = GetFrame(view, frame_path.Parent());
        std::vector<ElementId> elements;
        error = FindElementsHelper(
            cur_frame,
            ElementId(""),
            LocatorType::kXpath, frameBasePath, true, &elements);
        if (error) {
            std::string context = base::StringPrintf(
                "Could not find frame element (%s) in frame (%s)",
                frame_path.BaseName().value().c_str(),
                frame_path.Parent().value().c_str());
            error->AddDetails(context);
            return error;
        }

        frame_element = elements[0];
        // Modify |region_offset| by the frame's border.
        int border_left, border_top;
        error = GetElementBorder(
            cur_frame, frame_element, &border_left, &border_top);
        if (error)
            return error;
        region_offset.Offset(border_left, border_top);

        error = GetElementRegionInViewHelper(
            cur_frame, frame_element, Rect(region_offset, region_size),
            center, verify_clickable_at_middle, &region_offset);
        if (error)
            return error;
    }
    *location = region_offset;
    return NULL;
}

Error* QWebViewCmdExecutor::GetElementRegionInViewHelper(
        QWebFrame* frame,
        const ElementId& element,
        const Rect& region,
        bool center,
        bool verify_clickable_at_middle,
        Point* location) {
    Point temp_location;
    Error* error = ExecuteScriptAndParse(
                        frame,
                        atoms::asString(atoms::GET_LOCATION_IN_VIEW),
                        "getLocationInView",
                        CreateListValueFrom(element, center, region),
                        CreateDirectValueParser(&temp_location));

    if (error) 
        return error;

    if (verify_clickable_at_middle) {
        Point middle_point = temp_location;
        middle_point.Offset(region.width() / 2, region.height() / 2);
        error = VerifyElementIsClickable(frame, element, middle_point);
        if (error)
            return error;
    }
    *location = temp_location;
    return NULL;
}

Error* QWebViewCmdExecutor::VerifyElementIsClickable(
        QWebFrame* frame,
        const ElementId& element,
        const Point& location) {
    class IsElementClickableParser : public ValueParser {
    public:
        IsElementClickableParser(bool* clickable, std::string* message)
            : clickable_(clickable), message_(message) { }

        virtual ~IsElementClickableParser() { }

        virtual bool Parse(base::Value* value) const OVERRIDE {
            if (!value->IsType(Value::TYPE_DICTIONARY))
                return false;
            DictionaryValue* dict = static_cast<DictionaryValue*>(value);
            dict->GetString("message", message_);
            return dict->GetBoolean("clickable", clickable_);
        }

    private:
        bool* clickable_;
        std::string* message_;
    };

    bool clickable;
    std::string message;
    Error* error = ExecuteScriptAndParse(
                        frame,
                        atoms::asString(atoms::IS_ELEMENT_CLICKABLE),
                        "isElementClickable",
                        CreateListValueFrom(element, location),
                        new IsElementClickableParser(&clickable, &message));
    if (error)
        return error;

    if (!clickable) {
        if (message.empty())
            message = "element is not clickable";
        return new Error(kUnknownError, message);
    }
    if (message.length()) {
        session_->logger().Log(kWarningLogLevel, message);
    }
    return NULL;
}

Error* QWebViewCmdExecutor::GetElementBorder(QWebFrame* frame,
                                const ElementId& element,
                                int* border_left,
                                int* border_top) {
    std::string border_left_str, border_top_str;
    Error* error = GetElementEffectiveStyle(
                        frame,
                        element,
                        "border-left-width",
                        &border_left_str);
    if (error)
        return error;
    error = GetElementEffectiveStyle(
                frame, element, "border-top-width", &border_top_str);
    if (error)
        return error;

    base::StringToInt(border_left_str, border_left);
    base::StringToInt(border_top_str, border_top);
    return NULL;
}

Error* QWebViewCmdExecutor::GetElementEffectiveStyle(
                QWebFrame* frame,
                const ElementId& element,
                const std::string& prop,
                std::string* value) {

    return ExecuteScriptAndParse(
                frame,
                atoms::asString(atoms::GET_EFFECTIVE_STYLE),
                "getEffectiveStyle",
                CreateListValueFrom(element, prop),
                CreateDirectValueParser(value));
}

Error* QWebViewCmdExecutor::SwitchToFrameWithJavaScriptLocatedFrame(
                                QWebView* view,
                                QWebFrame* frame,
                                const std::string& script,
                                ListValue* args) {
    class SwitchFrameValueParser : public ValueParser {
    public:
        SwitchFrameValueParser(
            bool* found_frame, ElementId* frame)
            : found_frame_(found_frame), frame_(frame) { }

        virtual ~SwitchFrameValueParser() { }

        virtual bool Parse(base::Value* value) const OVERRIDE {
            if (value->IsType(Value::TYPE_NULL)) {
                *found_frame_ = false;
                return true;
            }
            ElementId id(value);
            if (!id.is_valid()) {
                return false;
            }
            *frame_ = id;
            *found_frame_ = true;
            return true;
        }

    private:
        bool* found_frame_;
        ElementId* frame_;
    };

    bool found_frame;
    ElementId new_frame_element;
    Error* error = ExecuteScriptAndParse(
                    frame, script, "switchFrame", args,
                    new SwitchFrameValueParser(&found_frame, &new_frame_element));
    if (error)
        return error;

    if (!found_frame)
        return new Error(kNoSuchFrame);

    std::string frame_id = GenerateRandomID();
    error = ExecuteScriptAndParse(
                frame,
                "function(elem, id) { var meta; elem.setAttribute('wd_frame_id_', id); var doc = elem.contentDocument? elem.contentDocument: elem.contentWindow.document; meta=doc.createElement('meta'); meta.name = 'wd_frame_id_'; meta.content = id; var child = doc.body.appendChild(meta);}",
                "setFrameId",
                CreateListValueFrom(new_frame_element, frame_id),
                CreateDirectValueParser(kSkipParsing));
    if (error)
        return error;

    AddIdToCurrentFrame(view, FramePath(frame_id));

    session_->frame_elements_.push_back(new_frame_element);
    FramePath frame_path = session_->current_frame();
    session_->set_current_frame(frame_path.Append(
            base::StringPrintf("%s", frame_id.c_str())));
    return NULL;
}

void QWebViewCmdExecutor::AddIdToCurrentFrame(QWebView* view, const FramePath &frame_path) {
    QWebFrame *pFrame = FindFrameByMeta(view->page()->mainFrame(), frame_path);

    pFrame->setProperty("frame_id", QString(frame_path.value().c_str()));
}

QWebFrame* QWebViewCmdExecutor::FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path) {
    if (frame_path.value().empty())
        return NULL;

    foreach(QWebFrame *childFrame, parent->childFrames())
    {
        QString frameId = childFrame->metaData().value("wd_frame_id_");
        if (frameId == QString(frame_path.value().c_str()))
        {
            return childFrame;
        }
        else
        {
            QWebFrame *pFrame = FindFrameByMeta(childFrame, frame_path);
            if (pFrame != NULL)
                return pFrame;
        }

    }
    return NULL;
}

Error* QWebViewCmdExecutor::GetElementFirstClientRect(QWebFrame* frame,
                                    const ElementId& element,
                                    Rect* rect) {

    return ExecuteScriptAndParse(
                frame,
                atoms::asString(atoms::GET_FIRST_CLIENT_RECT),
                "getFirstClientRect",
                CreateListValueFrom(element),
                CreateDirectValueParser(rect));
}

Error* QWebViewCmdExecutor::GetClickableLocation(QWebView* view, const ElementId& element, Point* location) {
    bool is_displayed = false;
    Error* error = NULL;
    IsElementDisplayed(element,
                 true /* ignore_opacity */,
                 &is_displayed,
                 &error);
    if (error)
        return error;
    if (!is_displayed)
        return new Error(kElementNotVisible, "Element must be displayed to click");

    // We try 3 methods to determine clickable location. This mostly follows
    // what FirefoxDriver does. Try the first client rect, then the bounding
    // client rect, and lastly the size of the element (via closure).
    // SVG is one case that doesn't have a first client rect.
    Rect rect;
    scoped_ptr<Error> ignore_error(GetElementFirstClientRect(GetFrame(view, session_->current_frame()),element, &rect));
    if (ignore_error.get()) {
        Rect client_rect;

        ignore_error.reset(ExecuteScriptAndParse(
            GetFrame(view, session_->current_frame()),
            "function(elem) { console.log('Testing console'); return elem.getBoundingClientRect() }",
            "getBoundingClientRect",
            CreateListValueFrom(element),
            CreateDirectValueParser(&client_rect)));

        rect = Rect(0, 0, client_rect.width(), client_rect.height());
    }
    
    if (ignore_error.get()) {
        Size size;
        Error* tmp_err = NULL;
        GetElementSize(element, &size, &tmp_err);
        ignore_error.reset(tmp_err);
        rect = Rect(0, 0, size.width(), size.height());
    }

    if (ignore_error.get()) {
        return new Error(kUnknownError,
                     "Unable to determine clickable location of element");
    }
    error = GetElementRegionInView(
        view,
        element, rect, true /* center */, true /* verify_clickable_at_middle */,
        location);

    if (error)
        return error;

    location->Offset(rect.width() / 2, rect.height() / 2);
    return NULL;
}   

Error* QWebViewCmdExecutor::ToggleOptionElement(const ElementId& element) {
    bool is_selected;
    Error* error = NULL;
    IsOptionElementSelected(element, &is_selected, &error);
    if (error)
        return error;

    SetOptionElementSelected(element, !is_selected, &error);

    return error;
}

void QWebViewCmdExecutor::AddBrowserLoggerToView(QWebView* view)
{
    BrowserLogHandler* logHandler = view->findChild<BrowserLogHandler*>();
    if (NULL == logHandler)
    {
        logHandler = new BrowserLogHandler(view);
        logHandler->SetMinLogLevel(session_->capabilities().log_levels[LogType::kBrowser]);
    }

    QObject::connect(view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), logHandler, SLOT(loadJSLogObject()));
    QObject::connect(view, SIGNAL(loadFinished(bool)),logHandler, SLOT(loadConsoleJS()), Qt::QueuedConnection);
    logHandler->loadJSLogObject(view->page()->mainFrame());
    logHandler->loadConsoleJS(view);
}

void QWebViewCmdExecutor::DrawMark(const QPoint& point) const {
    QWebView* view = QWebViewUtil::getWebView(session_, view_id_);

    QList<QCursorMark*> marks = view->findChildren<QCursorMark*>();
    QCursorMark* mark;
    if (marks.empty()) {
        mark = new QCursorMark(view);
    } else  {
        mark = marks.front();
    }
    mark->move(point);
    mark->show();
}

} //namespace webdriver 
