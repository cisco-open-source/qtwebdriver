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

#ifndef WEBDRIVER_QWEBKIT_PROXY_H
#define WEBDRIVER_QWEBKIT_PROXY_H

#include <string>
#include <vector>
#include <map>

#include <QtCore/QtGlobal>
#include <QtCore/QVariant>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKit/QWebHistory>
#else
#include <QtWebKit/QtWebKit>
#endif

#include "base/basictypes.h"
#include "base/string16.h"
#include "webdriver_view_id.h"
#include "webdriver_element_id.h"
#include "webdriver_logging.h"
#include "webdriver_basic_types.h"

namespace base {
class Value;    
}

class QWebPage;
class QWebFrame;

namespace webdriver {

class Session;
class Error;
class FramePath;
class ValueParser;

class QPageLoader : public QObject {
    Q_OBJECT
public:
    explicit QPageLoader(QWebPage* page) :
                QObject(NULL) {webPage = page; is_loading = false;}

    void loadPage(QUrl url);
    void reloadPage();
    bool isLoading() {return is_loading;}
    
signals:
    void loaded();
    
public slots:
    void pageLoadStarted();
    void pageLoadFinished();
private:
    bool is_loading;
    QWebPage* webPage;
    
};

//Notify automation module about end of execution of async script
class JSNotifier : public QObject {
    Q_OBJECT

public:
    JSNotifier();
    QVariant getResult();
    bool IsCompleted();


public slots:
    void setResult(QVariant result);

signals:
    void completed();

private:
    QVariant res;
    bool isCompleted;
};

class JSLogger : public QObject {
    Q_OBJECT

public:
    JSLogger();
    base::ListValue* getLog();
    void SetMinLogLevel(LogLevel level);

public slots:
    void log(QVariant message);
    void warn(QVariant message);
    void error(QVariant message);

private:
    InMemoryLog browserLog;
    Logger browserLogger;
};

class BrowserLogHandler : public QObject {
    Q_OBJECT

public:
    BrowserLogHandler(QObject* parent);
    base::ListValue* getLog();
    void SetMinLogLevel(LogLevel level);
    void loadConsoleJS(const QWebPage* page);
    void loadJSLogObject(QWebFrame *frame);

public slots:
    void loadJSLogObject();
    void loadConsoleJS();

private:
    JSLogger jslogger;
};

class QWebkitProxy {
public:
    explicit QWebkitProxy(Session* session, QWebPage* webpage);
    virtual ~QWebkitProxy();

    virtual Error* GetTitle(std::string* title);
    virtual Error* GetWindowName(std::string* name);
    virtual Error* GetBounds(Rect *bounds);
    virtual Error* GoForward();
    virtual Error* GoBack();
    virtual Error* Reload();
    virtual Error* GetSource(std::string* source);
    virtual Error* GetAttribute(const ElementId& element, const std::string& key, base::Value** value);
    virtual Error* ClearElement(const ElementId& element);
    virtual Error* IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed);
    virtual Error* IsElementEnabled(const ElementId& element, bool* is_enabled);
    virtual Error* ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal);
    virtual Error* GetElementLocation(const ElementId& element, Point* location);
    virtual Error* GetElementLocationInView(const ElementId& element, Point* location);
    virtual Error* GetClickableLocation(const ElementId& element, Point* location);
    virtual Error* GetElementTagName(const ElementId& element, std::string* tag_name);
    virtual Error* IsOptionElementSelected(const ElementId& element, bool* is_selected);
    virtual Error* SetOptionElementSelected(const ElementId& element, bool selected);
    virtual Error* IsElementCanBeToggled(const ElementId& element, bool* can_be_toggled);
    virtual Error* ToggleOptionElement(const ElementId& element);
    virtual Error* GetElementSize(const ElementId& element, Size* size);
    virtual Error* ElementSubmit(const ElementId& element);
    virtual Error* GetElementText(const ElementId& element, std::string* element_text);
    virtual Error* GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value);
    virtual Error* FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element);
    virtual Error* FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements);
    virtual Error* ActiveElement(ElementId* element);
    virtual Error* SetActiveElement(const ElementId& element);
    virtual Error* SwitchTo();
    virtual Error* SwitchToFrameWithNameOrId(const std::string& name_or_id);
    virtual Error* SwitchToFrameWithIndex(int index);
    virtual Error* SwitchToFrameWithElement(const ElementId& element);
    virtual Error* SwitchToTopFrame();
    virtual Error* SwitchToTopFrameIfCurrentFrameInvalid();
    virtual Error* NavigateToURL(const std::string& url, bool sync);
    virtual Error* GetURL(std::string* url);
    virtual Error* ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value);
    virtual Error* ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value);
    virtual Error* GetAppCacheStatus(int* status);
    virtual Error* GetCookies(const std::string& url, base::ListValue** cookies);
    virtual Error* SetCookie(const std::string& url, base::DictionaryValue* cookie_dict);
    virtual Error* DeleteCookie(const std::string& url, const std::string& cookie_name);
    virtual Error* GetStorageKeys(StorageType type, base::ListValue** keys);
    virtual Error* SetStorageItem(StorageType type, const std::string& key, const std::string& value);
    virtual Error* ClearStorage(StorageType type);
    virtual Error* GetStorageItem(StorageType type, const std::string& key, std::string* value);
    virtual Error* RemoveStorageItem(StorageType type, const std::string& key, std::string* value);
    virtual Error* GetStorageSize(StorageType type, int* size);
    virtual Error* GetBrowserLog(base::ListValue** browserLog);
    virtual Error* GetPlayerState(const ElementId& element, PlayerState *state);
    virtual Error* SetPlayerState(const ElementId& element, const PlayerState state);
    virtual Error* GetPlayerVolume(const ElementId& element, double *level);
    virtual Error* SetPlayerVolume(const ElementId& element, const double level);
    virtual Error* GetPlayingPosition(const ElementId& element, double* reletivePos);
    virtual Error* SetPlayingPosition(const ElementId& element, const double reletivePos);
    virtual Error* GetPlaybackSpeed(const ElementId& element, double* speed);
    virtual Error* SetPlaybackSpeed(const ElementId& element, const double speed);
    virtual Error* SetMute(const ElementId& element, bool mute);
    virtual Error* GetMute(const ElementId& element, bool* mute);
    virtual Error* SetOnline(bool online);
    virtual Error* IsOnline(bool* online);
    QWebFrame* GetFrame(const FramePath& frame_path);

protected:
    Session* session_;
    QWebPage* page_;

    QWebFrame* FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path);
    QWebFrame* FindFrameByPath(QWebFrame* parent, const FramePath &frame_path);

    void AddIdToCurrentFrame(QWebPage* page, const FramePath &frame_path);

    Error* ExecuteScript(QWebFrame* frame,
                        const std::string& script,
                        const base::ListValue* const args,
                        base::Value** value);

    Error* ExecuteAsyncScript(QWebFrame* frame,
                            const std::string& script,
                            const base::ListValue* const args,
                            base::Value** value);

    Error* ExecuteScriptAndParse(QWebFrame* frame,
                                const std::string& anonymous_func_script,
                                const std::string& script_name,
                                const base::ListValue* args,
                                const ValueParser* parser);

    Error* ExecuteScriptAndParseValue(QWebFrame* frame,
                                    const std::string& script,
                                    base::Value** script_result, bool isAsync);

    Error* ExecuteScriptImpl(QWebFrame* frame,
                               const std::string &script,
                               std::string *result,
                               bool isAsync);

    Error* FindElementsHelper(QWebFrame* frame,
                            const ElementId& root_element,
                            const std::string& locator,
                            const std::string& query,
                            bool find_one,
                            std::vector<ElementId>* elements);

    Error* ExecuteFindElementScriptAndParse(
                QWebFrame* frame,
                const ElementId& root_element,
                const std::string& locator,
                const std::string& query,
                bool find_one,
                std::vector<ElementId>* elements);

    Error* GetElementEffectiveStyle(
                QWebFrame* frame,
                const ElementId& element,
                const std::string& prop,
                std::string* value);

    Error* GetElementRegionInViewHelper(
                QWebFrame* frame,
                const ElementId& element,
                const Rect& region,
                bool center,
                bool verify_clickable_at_middle,
                Point* location);

    Error* GetElementRegionInView(
                const ElementId& element,
                const Rect& region,
                bool center,
                bool verify_clickable_at_middle,
                Point* location);

    Error* VerifyElementIsClickable(
                QWebFrame* frame,
                const ElementId& element,
                const Point& location);

    Error* GetElementBorder(QWebFrame* frame,
                const ElementId& element,
                int* border_left,
                int* border_top);

    Error* GetElementFirstClientRect(QWebFrame* frame,
                const ElementId& element,
                Rect* rect);

    Error* SwitchToFrameWithJavaScriptLocatedFrame(
                                QWebPage* page,
                                QWebFrame* frame,
                                const std::string& script,
                                base::ListValue* args);

    void AddBrowserLoggerToView();
    void SetWebInspectorSupport(QWebPage *page);


private:
    DISALLOW_COPY_AND_ASSIGN(QWebkitProxy);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QWEBKIT_PROXY_H
