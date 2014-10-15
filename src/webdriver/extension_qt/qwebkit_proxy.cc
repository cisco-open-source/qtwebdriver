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

#include "qwebkit_proxy.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#else
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#endif

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkCookie>
#include <QtNetwork/QNetworkCookieJar>

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"


#include "webdriver_session.h"
#include "webdriver_util.h"
#include "frame_path.h"
#include "value_conversion_util.h"
#include "webdriver_logging.h"
#include "webdriver_server.h"
#include "webdriver_switches.h"

#include "third_party/webdriver/atoms.h"

namespace webdriver {

void QPageLoader::loadPage(QUrl url) {
    connect(webPage, SIGNAL(loadStarted()),this, SLOT(pageLoadStarted()));
    webPage->mainFrame()->load(url);
}

void QPageLoader::reloadPage() {
    connect(webPage, SIGNAL(loadStarted()),this, SLOT(pageLoadStarted()));
    webPage->mainFrame()->load(webPage->mainFrame()->url());
}

void QPageLoader::pageLoadStarted() {
    is_loading = true;
    connect(webPage, SIGNAL(loadFinished(bool)),this, SLOT(pageLoadFinished()), Qt::QueuedConnection);
}

void QPageLoader::pageLoadFinished() {
    is_loading = false;
    emit loaded();
}	

JSNotifier::JSNotifier():
    isCompleted(false) {
}

QVariant JSNotifier::getResult() {
    return res;
}

bool JSNotifier::IsCompleted() {
    return isCompleted;
}

void JSNotifier::setResult(QVariant result) {
    res = result;
    isCompleted = true;
    emit completed();
}	

BrowserLogHandler::BrowserLogHandler(QObject *parent): QObject(parent), jslogger() {
}

base::ListValue* BrowserLogHandler::getLog() {
    return jslogger.getLog();
}

void BrowserLogHandler::SetMinLogLevel(LogLevel level) {
    jslogger.SetMinLogLevel(level);
}

void BrowserLogHandler::loadJSLogObject() {
    QWebFrame *mainFrame = qobject_cast<QWebFrame*>(sender());
    if (mainFrame)
        loadJSLogObject(mainFrame);
}

void BrowserLogHandler::loadJSLogObject(QWebFrame *frame) {
    frame->addToJavaScriptWindowObject("wdconsole", &jslogger);
}

void BrowserLogHandler::loadConsoleJS() {
    QWebPage* page = qobject_cast<QWebPage*>(sender());
    if (page)
        loadConsoleJS(page);
}

void BrowserLogHandler::loadConsoleJS(const QWebPage *page) {
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
    page->mainFrame()->evaluateJavaScript(jscript);
}

JSLogger::JSLogger() {
    browserLogger.AddHandler(&browserLog);
}

base::ListValue* JSLogger::getLog() {
    base::ListValue* retVal = browserLog.entries_list()->DeepCopy();
    browserLog.clear_entries_list();
    return retVal;
}

void  JSLogger::SetMinLogLevel(LogLevel level) {
    browserLogger.set_min_log_level(level);
}

void JSLogger::log(QVariant message) {
    browserLogger.Log(kInfoLogLevel, message.toString().toStdString());
}

void JSLogger::warn(QVariant message) {
    browserLogger.Log(kWarningLogLevel, message.toString().toStdString());
}

void JSLogger::error(QVariant message) {
    browserLogger.Log(kSevereLogLevel, message.toString().toStdString());
}	

QWebkitProxy::QWebkitProxy(Session* session, QWebPage* webpage) :
				session_(session),
				page_(webpage) {}

QWebkitProxy::~QWebkitProxy() {}

Error* QWebkitProxy::GetTitle(std::string* title) {
	const char* kGetTitleScript =
        "function() {"
        "  if (document.title)"
        "    return document.title;"
        "  else"
        "    return document.URL;"
        "}";

    return ExecuteScriptAndParse(
                GetFrame(FramePath()),
                kGetTitleScript,
                "getTitle",
                new ListValue(),
                CreateDirectValueParser(title));
}

Error* QWebkitProxy::GetWindowName(std::string* name) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                "function() { return window.name; }",
                "getWindowName",
                new ListValue(),
                CreateDirectValueParser(name));
}

Error* QWebkitProxy::GetBounds(Rect *bounds) {
	const char* kGetWindowBoundsScript =
        "function() {"
        "  return {"
        "    'left': window.screenX,"
        "    'top': window.screenY,"
        "    'width': window.outerWidth,"
        "    'height': window.outerHeight"
        "  }"
        "}";

    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                kGetWindowBoundsScript,
                "getWindowBoundsScript",
                new ListValue(),
                CreateDirectValueParser(bounds));	
}

Error* QWebkitProxy::GoForward() {
	page_->history()->forward();
    
    return NULL;
}

Error* QWebkitProxy::GoBack() {
	page_->history()->back();
    
    return NULL;
}

Error* QWebkitProxy::Reload() {
	// TODO: take into account page load strategy
    {
        // sync reload
        QPageLoader pageLoader(page_);
        QEventLoop loop;
        //TODO: 
        //page_->stop();
        QObject::connect(&pageLoader, SIGNAL(loaded()),&loop,SLOT(quit()));
        pageLoader.reloadPage();
        if (pageLoader.isLoading()) {
            loop.exec();
        }
    }

    return NULL;
}

Error* QWebkitProxy::GetSource(std::string* source) {
	const char* kSource =
        "function() {"
        "  return new XMLSerializer().serializeToString(document);"
        "}";

    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                kSource,
                "getSource",
                new ListValue(),
                CreateDirectValueParser(source));
}

Error* QWebkitProxy::GetAttribute(const ElementId& element, const std::string& key, base::Value** value) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::GET_ATTRIBUTE),
                "getAttribute",
                CreateListValueFrom(element, key),
                CreateDirectValueParser(value));
}

Error* QWebkitProxy::ClearElement(const ElementId& element) {
	std::string script = base::StringPrintf(
        "(%s).apply(null, arguments);", atoms::asString(atoms::CLEAR).c_str());

    ListValue args;
    args.Append(element.ToValue());

    Value* value = NULL;
    Error* err = ExecuteScript(
                    GetFrame(session_->current_frame()),
                    script,
                    &args,
                    &value);
    scoped_ptr<Value> scoped_value(value);	

    return err;
}

Error* QWebkitProxy::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::IS_DISPLAYED),
                "isDisplayed",
                CreateListValueFrom(element, ignore_opacity),
                CreateDirectValueParser(is_displayed));
}

Error* QWebkitProxy::IsElementEnabled(const ElementId& element, bool* is_enabled) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::IS_ENABLED),
                "isEnabled",
                CreateListValueFrom(element),
                CreateDirectValueParser(is_enabled));
}

Error* QWebkitProxy::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal) {
	const std::string script = "function(el1, el2) { return el1 == el2; }";

    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                script,
                "elementEquals",
                CreateListValueFrom(element1, element2),
                CreateDirectValueParser(is_equal));
}

Error* QWebkitProxy::GetElementLocation(const ElementId& element, Point* location) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::GET_LOCATION),
                "getLocation",
                CreateListValueFrom(element),
                CreateDirectValueParser(location));	
}

Error* QWebkitProxy::GetElementLocationInView(const ElementId& element, Point* location) {
	Size size;
    Error* error = GetElementSize(element, &size);
    if (error)
        return error;
        
    return GetElementRegionInView(
            element,
            Rect(Point(0, 0), size),
            false, // cente,
            false, // verify_clickable_at_middle
            location);	
}

Error* QWebkitProxy::GetElementTagName(const ElementId& element, std::string* tag_name) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                "function(elem) { return elem.tagName.toLowerCase() }",
                "getElementTagName",
                CreateListValueFrom(element),
                CreateDirectValueParser(tag_name));    
}

Error* QWebkitProxy::IsOptionElementSelected(const ElementId& element, bool* is_selected) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::IS_SELECTED),
                "isSelected",
                CreateListValueFrom(element),
                CreateDirectValueParser(is_selected));
}

Error* QWebkitProxy::SetOptionElementSelected(const ElementId& element, bool selected) {
	// This wrapper ensures the script is started successfully and
    // allows for an alert to happen when the option selection occurs.
    // See selenium bug 2671.
    const char kSetSelectedWrapper[] =
        "var args = [].slice.apply(arguments);"
        "args[args.length - 1]();"
        "return (%s).apply(null, args.slice(0, args.length - 1));";
    Value* value = NULL;
    Error* error = ExecuteAsyncScript(
                GetFrame(session_->current_frame()),
                base::StringPrintf(kSetSelectedWrapper,
                         atoms::asString(atoms::CLICK).c_str()),
                CreateListValueFrom(element, selected),
                &value);
    scoped_ptr<Value> scoped_value(value);
    return error;
}

Error* QWebkitProxy::IsElementCanBeToggled(const ElementId& element, bool* can_be_toggled) {
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
    return ExecuteScriptAndParse(
            GetFrame(session_->current_frame()),
            kCanOptionBeToggledScript,
            "canOptionBeToggled",
            CreateListValueFrom(element),
            CreateDirectValueParser(can_be_toggled));
}

Error* QWebkitProxy::ToggleOptionElement(const ElementId& element) {
    bool is_selected;
    Error* error = IsOptionElementSelected(element, &is_selected);
    if (error)
        return error;

    return SetOptionElementSelected(element, !is_selected);
}

Error* QWebkitProxy::GetElementSize(const ElementId& element, Size* size) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::GET_SIZE),
                "getSize",
                CreateListValueFrom(element),
                CreateDirectValueParser(size));
}

Error* QWebkitProxy::ElementSubmit(const ElementId& element) {
	std::string script = base::StringPrintf(
        "(%s).apply(null, arguments);", atoms::asString(atoms::SUBMIT).c_str());

    ListValue args;
    args.Append(element.ToValue());

    Value* result = NULL;
    Error* error = ExecuteScript(
                    GetFrame(session_->current_frame()),
                    script, &args, &result);
    scoped_ptr<Value> scoped_value(result);
    return error;
}

Error* QWebkitProxy::GetElementText(const ElementId& element, std::string* element_text) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::GET_TEXT),
                "getText",
                CreateListValueFrom(element),
                CreateDirectValueParser(element_text));
}

Error* QWebkitProxy::GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value) {
	std::string script = base::StringPrintf(
        "return (%s).apply(null, arguments);",
        atoms::asString(atoms::GET_EFFECTIVE_STYLE).c_str());

    ListValue args;
    args.Append(element.ToValue());
    args.Append(Value::CreateStringValue(property));

    return ExecuteScript(
                GetFrame(session_->current_frame()),
                script, &args, value);
}

Error* QWebkitProxy::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element) {
	std::vector<ElementId> elements;
    Error* error = FindElementsHelper(
                GetFrame(session_->current_frame()),
                root_element, locator, query, true, &elements);
    if (!error)
      *element = elements[0];
  	return error;
}

Error* QWebkitProxy::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements) {
	return FindElementsHelper(
                GetFrame(session_->current_frame()),
                root_element, locator, query, false, elements);
}


Error* QWebkitProxy::ActiveElement(ElementId* element) {
	std::string script = "function() { return document.activeElement || document.body; }";

    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                script,
                "activeElement",
                new ListValue(),
                CreateDirectValueParser(element));	
}

Error* QWebkitProxy::SetActiveElement(const ElementId& element) {
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
    //   - V8 throws a TypeError when calling setSelectionRange for a non-text
    //     input, which still have setSelectionRange defined. For chrome 29+, V8
    //     throws a DOMException with code InvalidStateError.
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
        "    try {"
        "      elem.setSelectionRange(elem.value.length, elem.value.length);"
        "    } catch (error) {"
        "      if (!(error instanceof TypeError) && !(error instanceof DOMException &&"
        "          error.code == DOMException.INVALID_STATE_ERR))"
        "        throw error;"
        "    }"
        "  }"
        "  if (elem != doc.activeElement)"
        "    throw new Error('cannot focus element');"
        "}";
    return ExecuteScriptAndParse(GetFrame(session_->current_frame()),
                                kFocusScript,
                                "focusElement",
                                CreateListValueFrom(element),
                                CreateDirectValueParser(kSkipParsing));	
}

Error* QWebkitProxy::SwitchTo() {
    AddBrowserLoggerToView();

    SetWebInspectorSupport(page_);

    // reset frame path
    session_->frame_elements_.clear();
    session_->set_current_frame(FramePath());

    return NULL;
}

Error* QWebkitProxy::SwitchToFrameWithNameOrId(const std::string& name_or_id) {
	std::string script =
        "function(arg) {"
        "  var xpath = '(/html/body//iframe|/html/frameset/frame)';"
        "  var sub = function(s) { return s.replace(/\\$/g, arg); };"
        "  xpath += sub('[@name=\"$\" or @id=\"$\"]');"
        "  return document.evaluate(xpath, document, null, "
        "      XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;"
        "}";
    return SwitchToFrameWithJavaScriptLocatedFrame(
                    page_,
                    GetFrame(session_->current_frame()),
                    script, CreateListValueFrom(name_or_id));

}

Error* QWebkitProxy::SwitchToFrameWithIndex(int index) {
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
    return SwitchToFrameWithJavaScriptLocatedFrame(
                    page_,
                    GetFrame(session_->current_frame()),
                    script, CreateListValueFrom(index));
}

Error* QWebkitProxy::SwitchToFrameWithElement(const ElementId& element) {
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
    return SwitchToFrameWithJavaScriptLocatedFrame(
            page_,
            GetFrame(session_->current_frame()),
            script, CreateListValueFrom(element));
}

Error* QWebkitProxy::SwitchToTopFrame() {
	session_->frame_elements_.clear();
    session_->set_current_frame(FramePath());

    return NULL;
}

Error* QWebkitProxy::SwitchToTopFrameIfCurrentFrameInvalid() {
	std::vector<std::string> components;
    session_->current_frame().GetComponents(&components);
    if (session_->frame_elements_.size() != components.size()) {
        return new Error(kUnknownError,
                     "Frame element vector out of sync with frame path");
    }

    FramePath frame_path;
    // Start from the root path and check that each frame element that makes
    // up the current frame target is valid by executing an empty script.
    // This code should not execute script in any frame before making sure the
    // frame element is valid, otherwise the automation hangs until a timeout.
    for (size_t i = 0; i < session_->frame_elements_.size(); ++i) {
        scoped_ptr<Error> scoped_error(ExecuteScriptAndParse(
            GetFrame(frame_path),
            "function(){ }",
            "emptyScript",
            CreateListValueFrom(session_->frame_elements_[i]),
            CreateDirectValueParser(kSkipParsing)));
        if (scoped_error.get() && scoped_error->code() == kStaleElementReference) {
            scoped_ptr<Error> tmp(SwitchToTopFrame());
        } else if (scoped_error.get()) {
            return scoped_error.release();
        }
        frame_path = frame_path.Append(components[i]);
    }

    return NULL;
}

Error* QWebkitProxy::NavigateToURL(const std::string& url, bool sync) {
	QUrl address(QString(url.c_str()));

    base::Time start_time = base::Time::Now();
    if (sync) {
        QPageLoader pageLoader(page_);
        QEventLoop loop;
        // TODO:
        //page_->stop();
        QObject::connect(&pageLoader, SIGNAL(loaded()),&loop,SLOT(quit()));
        pageLoader.loadPage(address);
        if (pageLoader.isLoading()) {
            loop.exec();
        }
    } else {
        page_->mainFrame()->load(address);
    }
    if ((base::Time::Now() - start_time).InMilliseconds() > session_->page_load_timeout()) {
        return new Error(kTimeout, "page loading timed out");
    }


    return NULL;
}

Error* QWebkitProxy::GetURL(std::string* url) {
	return ExecuteScriptAndParse(
                    GetFrame(session_->current_frame()),
                    "function() { return document.URL }",
                    "getUrl",
                    new ListValue(),
                    CreateDirectValueParser(url));
}

Error* QWebkitProxy::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value) {
	return ExecuteScript(
                GetFrame(session_->current_frame()),
                script,
                args,
                value);
}

Error* QWebkitProxy::ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value) {
	return ExecuteAsyncScript(
                GetFrame(session_->current_frame()),
                script,
                args,
                value);
}

Error* QWebkitProxy::GetAppCacheStatus(int* status) {
	return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                atoms::asString(atoms::GET_APPCACHE_STATUS),
                "getAppcacheStatus",
                new ListValue(),
                CreateDirectValueParser(status));
}

Error* QWebkitProxy::GetCookies(const std::string& url, base::ListValue** cookies) {
	QString qUrl = url.c_str();
    QNetworkCookieJar* jar = page_->networkAccessManager()->cookieJar();

    if (NULL == jar) {
        return new Error(kUnknownError, "No active NetworkCookieJar");
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
        return new Error(kUnknownError);
    }

    *cookies = static_cast<ListValue*>(cookies_value.release());

    return NULL;
}

Error* QWebkitProxy::SetCookie(const std::string& url, base::DictionaryValue* cookie_dict) {
	QList<QNetworkCookie> cookie_list;
    std::string name, value;
    std::string domain;
    std::string path = "/";
    bool secure = false;
    double expiry = 0;
    bool http_only = false;

    if (!cookie_dict->GetString("name", &name)) {
        return new Error(kUnknownError, "'name' missing or invalid");
    }

    if (!cookie_dict->GetString("value", &value)) {
        return new Error(kUnknownError, "'value' missing or invalid");
    }

    QNetworkCookie cookie(QByteArray(name.c_str()), QByteArray(value.c_str()));

    if (cookie_dict->HasKey("domain")) {
        if (!cookie_dict->GetString("domain", &domain)) {
            return new Error(kUnknownError, "optional 'domain' invalid");
        }

        session_->logger().Log(kFineLogLevel, "SetCookie - domain:" + domain);

        // TODO: check why it fails here
        //cookie.setDomain(QString(domain.c_str()));
    }

    if (cookie_dict->HasKey("path")) {
        if (!cookie_dict->GetString("path", &path)) {
            return new Error(kUnknownError, "optional 'path' invalid");
        }

        cookie.setPath(QString(path.data()));
    }

    if (cookie_dict->HasKey("secure")) {
        if (!cookie_dict->GetBoolean("secure", &secure)) {
            return new Error(kUnknownError, "optional 'secure' invalid");
        }

        cookie.setSecure(secure);
    }

    if (cookie_dict->HasKey("expiry")) {
        if (!cookie_dict->GetDouble("expiry", &expiry)) {
            return new Error(kUnknownError, "optional 'expiry' invalid");
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
            return new Error(kUnknownError, "Could not set expired cookie");
        }
    }

    if (cookie_dict->HasKey("http_only")) {
        if (!cookie_dict->GetBoolean("http_only", &http_only)) {
            return new Error(kUnknownError, "optional 'http_only' invalid");
        }

        cookie.setHttpOnly(http_only);
    }

    cookie_list.append(cookie);

    QNetworkCookieJar* jar = page_->networkAccessManager()->cookieJar();

    if (!jar) {
        jar = new QNetworkCookieJar();
        page_->networkAccessManager()->setCookieJar(jar);
    }

    if (!jar->setCookiesFromUrl(cookie_list, QUrl(url.c_str()))) {
        return new Error(kUnknownError, "Could not set the cookie");
    }

    return NULL;
}

Error* QWebkitProxy::DeleteCookie(const std::string& url, const std::string& cookie_name) {
	QString qUrl = url.c_str();
    QNetworkCookieJar* jar = page_->networkAccessManager()->cookieJar();

    if (NULL == jar) {
        return new Error(kUnknownError, "No active NetworkCookieJar");
    }

    QList<QNetworkCookie> cookies = jar->cookiesForUrl(QUrl(qUrl));

    if (cookies.isEmpty())
        return NULL;

    QList<QNetworkCookie>::Iterator it = cookies.begin();
    QList<QNetworkCookie>::Iterator end = cookies.end();

    while (it != end) {
        std::string str_name(it->name().data());

        if (str_name == cookie_name) {
            it = cookies.erase(it);

            // NOTE: use QNetworkCookieJar::deleteCookie in case QT 5.0
            QNetworkCookieJar * newCookieJar = new QNetworkCookieJar();
            newCookieJar->setCookiesFromUrl(cookies, QUrl(qUrl));
            page_->networkAccessManager()->setCookieJar(newCookieJar);
            return NULL;
        } else {
            it++;
        }
    }

    return new Error(kUnknownError, "No such cookie");
}

Error* QWebkitProxy::GetStorageKeys(StorageType type, base::ListValue** keys) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_KEYS
                                    : atoms::GET_SESSION_STORAGE_KEYS);
    return ExecuteScriptAndParse(
                    GetFrame(session_->current_frame()),
                    js,
                    "getStorageKeys",
                    new ListValue(),
                    CreateDirectValueParser(keys));
}

Error* QWebkitProxy::SetStorageItem(StorageType type, const std::string& key, const std::string& value) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::SET_LOCAL_STORAGE_ITEM
                                : atoms::SET_SESSION_STORAGE_ITEM);
    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                js,
                "setStorageItem",
                CreateListValueFrom(key, value),
                CreateDirectValueParser(kSkipParsing));
}

Error* QWebkitProxy::ClearStorage(StorageType type) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::CLEAR_LOCAL_STORAGE
                                : atoms::CLEAR_SESSION_STORAGE);
    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                js,
                "clearStorage",
                new ListValue(),
                CreateDirectValueParser(kSkipParsing));
}

Error* QWebkitProxy::GetStorageItem(StorageType type, const std::string& key, std::string* value) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_ITEM
                                : atoms::GET_SESSION_STORAGE_ITEM);
    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                js,
                "getStorageItem",
                CreateListValueFrom(key),
                CreateDirectValueParser(value));
}

Error* QWebkitProxy::RemoveStorageItem(StorageType type, const std::string& key, std::string* value) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::REMOVE_LOCAL_STORAGE_ITEM
                                : atoms::REMOVE_SESSION_STORAGE_ITEM);
    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                js,
                "removeStorageItem",
                CreateListValueFrom(key),
                CreateDirectValueParser(value));
}

Error* QWebkitProxy::GetStorageSize(StorageType type, int* size) {
	std::string js = atoms::asString(
        type == kLocalStorageType ? atoms::GET_LOCAL_STORAGE_SIZE
                                : atoms::GET_SESSION_STORAGE_SIZE);
    return ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                js,
                "getStorageSize",
                new ListValue(),
                CreateDirectValueParser(size));
}

Error* QWebkitProxy::GetBrowserLog(base::ListValue** browserLog) {
	BrowserLogHandler* logHandler = page_->findChild<BrowserLogHandler*>();
    if (NULL == logHandler)
        return NULL;

    *browserLog = logHandler->getLog();
    return NULL;
}

Error* QWebkitProxy::GetPlayerState(const ElementId& element, PlayerState *state) {
    bool isPaused;

    std::string tagName;
    Error *error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    base::Value* isPausedValue = NULL;
    error = GetAttribute(element, std::string("paused"), &isPausedValue);
    scoped_ptr<base::Value> scoped_isPausedValue(isPausedValue);
    if (error)
        return error;

    isPaused = !isPausedValue->IsType(base::Value::TYPE_NULL);

    if(!isPaused){
        *state = Playing;
    } else {
        std::string currentTimeString;
        base::Value *currentTimeValue;
        error = GetAttribute(element, std::string("currentTime"), &currentTimeValue);
        scoped_ptr<base::Value> scoped_currentTimeValue(currentTimeValue);
        if (error)
            return error;
        currentTimeValue->GetAsString(&currentTimeString);
        double currentTime = 0;
        base::StringToDouble(currentTimeString, &currentTime);
        if(currentTime == 0){
            *state = Stopped;
        } else {
            *state = Paused;
        }
    }

    return NULL;
}

Error* QWebkitProxy::SetPlayerState(const ElementId& element, const PlayerState state) {
    Value* value = NULL;
	Error* error = NULL;

    std::string tagName;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    switch(state){
    case Playing:
            error = ExecuteScriptAndParse(
                        GetFrame(session_->current_frame()),
                        "function(elem) { elem.play(); }",
                        "play",
                        CreateListValueFrom(element),
                        CreateDirectValueParser(&value));
            break;
    case Paused:
    case Stopped:
            error = ExecuteScriptAndParse(
                        GetFrame(session_->current_frame()),
                        "function(elem) { elem.pause(); }",
                        "pause",
                        CreateListValueFrom(element),
                        CreateDirectValueParser(&value));
            break;
    }
    scoped_ptr<Value> scoped_value(value);
    if (state == Stopped){
        error = SetPlayingPosition(element, 0.0);
    }

    return error;
}

Error* QWebkitProxy::GetPlayerVolume(const ElementId& element, double *volumeLevel) {
    base::Value* volumeValue = NULL;

    std::string tagName;
    Error* error;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = GetAttribute(element, std::string("volume"), &volumeValue);
    scoped_ptr<base::Value> scoped_volume_value(volumeValue);
    if(error)
        return error;
    std::string volumeString;
    volumeValue->GetAsString(&volumeString);
    base::StringToDouble(volumeString, volumeLevel);

    return NULL;
}

Error* QWebkitProxy::SetPlayerVolume(const ElementId& element, const double level) {
	Value* value = NULL;

    std::string tagName;
    Error* error;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = ExecuteScriptAndParse(
                        GetFrame(session_->current_frame()),
                        "function(elem, level) { elem.volume = level; }",
                        "setVolume",
                        CreateListValueFrom(element, level),
                        CreateDirectValueParser(&value));
    scoped_ptr<Value> scoped_value(value);
    return error;
}

Error* QWebkitProxy::GetPlayingPosition(const ElementId& element, double* reletivePos) {
    base::Value* positionValue = NULL;

    std::string tagName;
    Error* error;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = GetAttribute(element, std::string("currentTime"), &positionValue);
    scoped_ptr<base::Value> scoped_position_value(positionValue);
    if(error)
        return error;
    std::string positionString;
    positionValue->GetAsString(&positionString);
    base::StringToDouble(positionString, reletivePos);

    return NULL;
}

Error* QWebkitProxy::SetPlayingPosition(const ElementId& element, const double pos) {
	Value* value = NULL;

    std::string tagName;
    Error* error;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                "function(elem, time) { elem.currentTime = time; }",
                "setPosition",
                CreateListValueFrom(element, pos),
                CreateDirectValueParser(&value));
    scoped_ptr<Value> scoped_value(value);

    return error;
}

Error *QWebkitProxy::GetPlaybackSpeed(const ElementId &element, double *speed)
{
    base::Value* speedValue = NULL;
    std::string tagName;
    Error* error;
    error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = GetAttribute(element, std::string("playbackRate"), &speedValue);
    scoped_ptr<base::Value> scoped_position_value(speedValue);
    if(error)
        return error;
    std::string speedString;
    speedValue->GetAsString(&speedString);
    base::StringToDouble(speedString, speed);

    return NULL;
}

Error *QWebkitProxy::SetPlaybackSpeed(const ElementId &element, const double speed)
{
    Value* value = NULL;

    std::string tagName;
    Error *error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = ExecuteScriptAndParse(
                GetFrame(session_->current_frame()),
                "function(elem, speed) { elem.playbackRate = speed; }",
                "setPlaybackRate",
                CreateListValueFrom(element, speed),
                CreateDirectValueParser(&value));
    scoped_ptr<Value> scoped_value(value);

    return error;
}

Error* QWebkitProxy::SetMute(const ElementId& element, bool mute) {
	Value* value = NULL;

    std::string tagName;
    Error *error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = ExecuteScriptAndParse(
                        GetFrame(session_->current_frame()),
                        "function(elem, mute) { elem.muted = mute; }",
                        "setVolume",
                        CreateListValueFrom(element, mute),
                CreateDirectValueParser(&value));
    scoped_ptr<Value> scoped_value(value);

    return error;
}

Error* QWebkitProxy::GetMute(const ElementId& element, bool* mute) {
    base::Value* isMutedValue = NULL;

    std::string tagName;
    Error *error = GetElementTagName(element, &tagName);
    if(error)
        return error;

    if(tagName != "video" && tagName != "audio"){
        error = new Error(kInvalidElementState);
        return error;
    }

    error = GetAttribute(element, std::string("muted"), &isMutedValue);
    scoped_ptr<Value> scoped_value(isMutedValue);
    if(error)
        return error;
    *mute = !isMutedValue->IsType(base::Value::TYPE_NULL);

    return NULL;
}

Error* QWebkitProxy::SetOnline(bool online) {
#ifndef QT_NO_BEARERMANAGEMENT
    QNetworkAccessManager *manager = page_->networkAccessManager();
    if (online){
        manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    } else {
        manager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
    }
    return NULL;
#else
    session_->logger().Log(kWarningLogLevel, "In QWebkitProxy::SetOnline() defined QT_NO_BEARERMANAGEMENT");
    return new Error(kCommandNotSupported, "Can't change online mode");
#endif //QT_NO_BEARERMANAGEMENT
}

Error* QWebkitProxy::IsOnline(bool* online) {
#ifndef QT_NO_BEARERMANAGEMENT
    QNetworkAccessManager *manager = page_->networkAccessManager();
    if (manager->networkAccessible() == QNetworkAccessManager::NotAccessible) {
        *online = false;
    } else {
        *online = true;
    }
    return NULL;
#else
    session_->logger().Log(kWarningLogLevel, "In QWebkitProxy::IsOnline() defined QT_NO_BEARERMANAGEMENT");
    return new Error(kCommandNotSupported, "Can't check online mode");
#endif //QT_NO_BEARERMANAGEMENT
}

QWebFrame* QWebkitProxy::GetFrame(const FramePath& frame_path) {
    QWebFrame* frame = FindFrameByPath(page_->mainFrame(), frame_path);
    if (frame == NULL)
        frame = page_->mainFrame();

    return frame;
}

QWebFrame* QWebkitProxy::FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path) {
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

QWebFrame* QWebkitProxy::FindFrameByPath(QWebFrame* parent, const FramePath &frame_path) {
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

void QWebkitProxy::AddIdToCurrentFrame(QWebPage* page, const FramePath &frame_path) {
    QWebFrame *pFrame = FindFrameByMeta(page_->mainFrame(), frame_path);

    pFrame->setProperty("frame_id", QString(frame_path.value().c_str()));
}

Error* QWebkitProxy::ExecuteScript(QWebFrame* frame,
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

Error* QWebkitProxy::ExecuteAsyncScript(QWebFrame* frame,
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

Error* QWebkitProxy::ExecuteScriptAndParse(QWebFrame* frame,
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
    if (!parser->Parse(value.get())) {
        std::string error_msg = base::StringPrintf("%s returned invalid value: %s",
            script_name.c_str(), JsonStringify(value.get()).c_str());
        return new Error(kUnknownError, error_msg);
    }
    return NULL;
}

Error* QWebkitProxy::ExecuteScriptAndParseValue(QWebFrame* frame,
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

Error* QWebkitProxy::ExecuteScriptImpl(QWebFrame* frame,
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

Error* QWebkitProxy::FindElementsHelper(QWebFrame* frame,
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

        if (!temp_elements.empty()) {
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

Error* QWebkitProxy::ExecuteFindElementScriptAndParse(
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

Error* QWebkitProxy::GetElementEffectiveStyle(
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

Error* QWebkitProxy::GetElementRegionInViewHelper(
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

Error* QWebkitProxy::GetElementRegionInView(
        const ElementId& element,
        const Rect& region,
        bool center,
        bool verify_clickable_at_middle,
        Point* location) {

    CHECK(element.is_valid());

    Point region_offset = region.origin();
    Size region_size = region.size();

    Error* error = GetElementRegionInViewHelper(
                        GetFrame(session_->current_frame()),
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

        QWebFrame* cur_frame = GetFrame(frame_path.Parent());
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

Error* QWebkitProxy::VerifyElementIsClickable(
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

Error* QWebkitProxy::GetElementBorder(QWebFrame* frame,
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

Error* QWebkitProxy::GetElementFirstClientRect(QWebFrame* frame,
                                    const ElementId& element,
                                    Rect* rect) {

    return ExecuteScriptAndParse(
                frame,
                atoms::asString(atoms::GET_FIRST_CLIENT_RECT),
                "getFirstClientRect",
                CreateListValueFrom(element),
                CreateDirectValueParser(rect));
}

Error* QWebkitProxy::GetClickableLocation(const ElementId& element, Point* location) {
    bool is_displayed = false;
    Error* error = IsElementDisplayed(element,
                        true /* ignore_opacity */,
                        &is_displayed);
    if (error)
        return error;
    if (!is_displayed)
        return new Error(kElementNotVisible, "Element must be displayed to click");

    // We try 3 methods to determine clickable location. This mostly follows
    // what FirefoxDriver does. Try the first client rect, then the bounding
    // client rect, and lastly the size of the element (via closure).
    // SVG is one case that doesn't have a first client rect.
    Rect rect;

    scoped_ptr<Error> ignore_error(GetElementFirstClientRect(GetFrame(session_->current_frame()),element, &rect));

    // getFirstClientRect by atoms sometimes doesn't throw Error but returns invalid position outside element.
    bool invalid_pos = (int)rect.y() < 0 || (int)rect.x() < 0;

    if (ignore_error.get() || invalid_pos) {
        Rect client_rect;

        ignore_error.reset(ExecuteScriptAndParse(
            GetFrame(session_->current_frame()),
            "function(elem) { console.log('Testing console'); return elem.getBoundingClientRect() }",
            "getBoundingClientRect",
            CreateListValueFrom(element),
            CreateDirectValueParser(&client_rect)));

        rect = Rect(0, 0, client_rect.width(), client_rect.height());
    }
    
    if (ignore_error.get()) {
        Size size;
        ignore_error.reset(GetElementSize(element, &size));
        rect = Rect(0, 0, size.width(), size.height());
    }

    if (ignore_error.get()) {
        return new Error(kUnknownError,
                     "Unable to determine clickable location of element");
    }
    error = GetElementRegionInView(
        element, rect, true /* center */, true /* verify_clickable_at_middle */,
        location);

    if (error)
        return error;

    location->Offset(rect.width() / 2, rect.height() / 2);

    return NULL;
}   

Error* QWebkitProxy::SwitchToFrameWithJavaScriptLocatedFrame(
                                QWebPage* page,
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

    AddIdToCurrentFrame(page, FramePath(frame_id));

    session_->frame_elements_.push_back(new_frame_element);
    FramePath frame_path = session_->current_frame();
    session_->set_current_frame(frame_path.Append(
            base::StringPrintf("%s", frame_id.c_str())));

    return NULL;
}

void QWebkitProxy::AddBrowserLoggerToView() {
    BrowserLogHandler* logHandler = page_->findChild<BrowserLogHandler*>();

    if (NULL == logHandler) {
        logHandler = new BrowserLogHandler(page_);
        logHandler->SetMinLogLevel(session_->capabilities().log_levels[LogType::kBrowser]);
    }

    QObject::connect(page_->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), logHandler, SLOT(loadJSLogObject()));
    QObject::connect(page_, SIGNAL(loadFinished(bool)),logHandler, SLOT(loadConsoleJS()), Qt::QueuedConnection);
    logHandler->loadJSLogObject(page_->mainFrame());
    logHandler->loadConsoleJS(page_);
}

void QWebkitProxy::SetWebInspectorSupport(QWebPage *page)
{
    page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();

    if (cmdLine.HasSwitch(webdriver::Switches::kWIServer))
    {
        if (cmdLine.HasSwitch(webdriver::Switches::kWIPort))
        {
            std::string wiPort = cmdLine.GetSwitchValueASCII(webdriver::Switches::kWIPort);
            int port = QString(wiPort.c_str()).toInt();
            page->setProperty("_q_webInspectorServerPort", port);
        }
        else
        {
            page->setProperty("_q_webInspectorServerPort", 9222);
        }
    }
}


}  // namespace webdriver



