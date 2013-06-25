// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <QtCore/QEvent>
#include <QtCore/QThread>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmap>
#include <QtWebKit/QtWebKit>
#include <QtCore/QDebug>
#include <QtNetwork/QtNetwork>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebFrame>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QInputDialog>
#else
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>
#include <QtGui/QInputDialog>
#endif

#include "webdriver_automation.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

//#include "base/base_paths.h"
//#include "base/basictypes.h"
//#include "base/callback.h"
//#include "base/environment.h"
//#include "base/file_path.h"
//#include "base/file_util.h"
#include "base/json/json_writer.h"
//#include "base/memory/ref_counted.h"
//#include "base/path_service.h"
#include "base/string_number_conversions.h"
#include "base/string_split.h"
/*#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"*/
#include "chrome/common/automation_constants.h"
//#include "chrome/common/automation_messages.h"
//#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_switches.h"
//#include "chrome/common/url_constants.h"
#include "chrome/test/automation/automation_json_requests.h"
/*#include "chrome/test/automation/automation_proxy.h"
#include "chrome/test/automation/browser_proxy.h"
#include "chrome/test/automation/proxy_launcher.h"
#include "chrome/test/automation/tab_proxy.h"
#include "chrome/test/base/chrome_process_util.h"*/
#include "chrome/test/webdriver/frame_path.h"
#include "chrome/test/webdriver/webdriver_basic_types.h"
#include "chrome/test/webdriver/webdriver_error.h"
#include "chrome/test/webdriver/webdriver_session_manager.h"
// #include "chrome/test/webdriver/webdriver_util.h"

// #if defined(OS_WIN)
// #include "base/win/registry.h"
// #include "base/win/windows_version.h"
// #endif

#include "viewfactory.h"
#include <iostream>

class QNetworkCookie;

namespace webdriver {

Automation::BrowserOptions::BrowserOptions()
    : command(CommandLine::NO_PROGRAM),
      detach_process(false),
      ignore_certificate_errors(false) {}

Automation::BrowserOptions::~BrowserOptions() {}

Automation::Automation(const Logger& logger)
    : logger_(logger),
      build_no_(0),
      isLoading(false),
      sessionId(0)
{
}

Automation::~Automation() {}

void Automation::Init(const BrowserOptions& options, int* build_no, Error** error)
{
    qDebug()<<"[WD]:"<<"*************INIT SESSION******************";
    BuildKeyMap();

    *build_no = build_no_;
    //Need to keep after merge sessionId remove
    qsrand(QTime::currentTime().msec()+10);
    sessionId = qrand();

    QWebView *pStartView = NULL;

    //Searching for a allready opened window
    if (!options.browser_start_window.empty())
    {
        qDebug()<<"[WD]:"<<"Browser Start Window: "<<options.browser_start_window.c_str();
        foreach(QWidget* pWidget, qApp->topLevelWidgets())
        {

            //check found widget if it is QWebView or top level widget
            QWebView* pWebView = qobject_cast<QWebView*>(pWidget);
            if ((pWebView != NULL) && (!pWebView->isHidden()))
            {
                qDebug()<<"[WD]:"<<"looking for start window: "<<pWidget<<pWidget->windowTitle();

                if ((options.browser_start_window == pWidget->windowTitle().toStdString()) || (options.browser_start_window == "*"))
                {
                    pStartView = pWebView;

                    qDebug()<<"[WD]: found view to attach: "<<pWidget<<pWidget->windowTitle();

                    break;
                }
            }
        }
    }

    //or create new one
    if (pStartView == NULL)
        pStartView = ViewFactory::GetInstance()->create(options.browser_class);

    qDebug()<<"[WD]:"<<"Using window:"<<pStartView;

    // TODO: save proxy settings for further usage
    if (pStartView != NULL)
    {
        //proxy setup
        if (options.command.HasSwitch(switches::kNoProxyServer))
        {
            qDebug()<<"[WD]:" << "No proxy";
            pStartView->page()->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
        }
        else if (options.command.HasSwitch(switches::kProxyServer))
        {
            QStringList list = QString::fromStdString(options.command.GetSwitchValueASCII(switches::kProxyServer)).split(";");
            foreach (QString s, list)
            {
                if (s.startsWith("http"))
                {
                    QString urlString;
                    urlString = s;
                    if (s.startsWith("http="))
                        urlString = s.remove(0, QString("http=").length());

                    if (!urlString.startsWith("http://"))
                        urlString.insert(0, "http://");

                    QUrl proxyUrl(urlString);
                    if (proxyUrl.isValid() && !proxyUrl.host().isEmpty())
                    {
                        int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
                        pStartView->page()->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort));
                    }
                }
            }
        }
        else if (options.command.HasSwitch(switches::kProxyPacUrl))
        {
            qWarning() << "Proxy autoconfiguration from a URL is not suported";
            *error = new Error(kBadRequest, "Proxy autoconfiguration from a URL is not suported");
            return;
        }
        else if (options.command.HasSwitch(switches::kProxyAutoDetect))
        {
            qWarning() << "Proxy autodetection with WPAD is not suported";
            *error = new Error(kBadRequest, "Proxy autodetection with WPAD is not suported");
            return;
        }
        else
        {
            //Use system proxy by default
            QUrl proxyUrl(qgetenv("http_proxy"));
            if (proxyUrl.isValid() && !proxyUrl.host().isEmpty())
            {
                int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
                pStartView->page()->networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort));
            }
        }

        qDebug()<<"[WD]:" << "hostname = " << pStartView->page()->networkAccessManager()->proxy().hostName()
                 << ", port = " << pStartView->page()->networkAccessManager()->proxy().port();
    }

    //handle initial window size and position
    if (options.command.HasSwitch(switches::kWindowSize))
    {
        QStringList list = QString::fromStdString(options.command.GetSwitchValueASCII(switches::kWindowSize)).split(",");
        if (list.size() == 2)
        {
            bool isOkW = false;
            bool isOkH = false;
            int w = list.at(0).toInt(&isOkW);
            int h = list.at(1).toInt(&isOkH);
            if (isOkW && isOkH)
                pStartView->resize(w, h);
            else
                qWarning()<<"[WD]:" << "Wrong parameter in " << switches::kWindowSize << " switch";
        }
    }
    if (options.command.HasSwitch(switches::kWindowPosition))
    {
        QStringList list = QString::fromStdString(options.command.GetSwitchValueASCII(switches::kWindowPosition)).split(",");
        if (list.size() == 2)
        {
            bool isOkX = false;
            bool isOkY = false;
            int x = list.at(0).toInt(&isOkX);
            int y = list.at(1).toInt(&isOkY);
            if (isOkX && isOkY)
                pStartView->move(x, y);
            else
               qWarning()<<"[WD]:" << "Wrong parameter in " << switches::kWindowPosition << " switch";
        }
    }

    qDebug() << "[WD]: geometry:" << pStartView->geometry();
    if (options.command.HasSwitch(switches::kStartMaximized))
        pStartView->showMaximized();

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        pStartView->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }

    pStartView->show();
}

void Automation::Terminate()
{
  qDebug()<<"[WD]:"<<"*************TERMINATE SESSION******************";
  logger_.Log(kInfoLogLevel, "QtWebKit WebDriver shutdown");

  // clear elements map
  QMutableHashIterator<QString, ElementMap* > wnd(windowsElementMap);
  while (wnd.hasNext()) {
      wnd.next();
      ElementMap *elementMap  = wnd.value();
      if (elementMap != NULL) delete elementMap;
      wnd.remove();
  }

  foreach(QWidget* pView, qApp->topLevelWidgets())
  {
      if (!pView->isHidden())
      {
          pView->setAttribute(Qt::WA_DeleteOnClose, true);
          pView->close();
      }
  }
}

void Automation::ExecuteScript(const WebViewId &view_id, const FramePath &frame_path,
                               const std::string &script, std::string *result, bool isAsync, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QWebFrame* frame = FindFrameByPath(view->page()->mainFrame(), frame_path);
    if (frame == NULL)
        frame = view->page()->mainFrame();

//    qDebug()<<"[WD]:"<<script.c_str();

    std::string res;
    if (isAsync)
    {
        JSNotifier* jsNotify = new JSNotifier();
        frame->addToJavaScriptWindowObject("jsnotify", jsNotify );
        connect(jsNotify, SIGNAL(completed()), &loop, SLOT(quit()));
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
    qDebug()<<"[WD]:"<<result->c_str();
}

void Automation::MouseMoveDeprecated(const WebViewId &view_id, const Point &p, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QPoint point = ConvertPointToQPoint(p);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget)
    {
        qDebug() << "[WD] mouse move at view " << point << " on element: " << receiverWidget;
        point = receiverWidget->mapFrom(view, point);
    }
    else
    {
        qDebug() << "[WD] mouse move at view " << point;
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);
}

void Automation::MouseClickDeprecated(const WebViewId &view_id, const Point &p, automation::MouseButton button, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QPoint point = ConvertPointToQPoint(p);

    // Find child widget that will receive click
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget)
    {
        qDebug() << "[WD] click at view " << point << " on element: " << receiverWidget;
        point = receiverWidget->mapFrom(view, point);
    }
    else
    {
        qDebug() << "[WD] click at view " << point;
        receiverWidget = view;
    }

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, pressEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
    if (Qt::RightButton == mouseButton)
    {
        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point);
        QApplication::postEvent(receiverWidget, contextEvent);
    }
}

void Automation::MouseDragDeprecated(const WebViewId &view_id, const Point &start, const Point &end, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    // TODO: verify this

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, ConvertPointToQPoint(start), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, ConvertPointToQPoint(end), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonPress, ConvertPointToQPoint(end), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view, pressEvent);
    QApplication::postEvent(view, moveEvent);
    QApplication::postEvent(view, releaseEvent);
}

void Automation::MouseButtonUpDeprecated(const WebViewId &view_id, const Point &p, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QPoint point = ConvertPointToQPoint(p);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget)
    {
        qDebug() << "[WD] mouse button up at view " << point << " on element: " << receiverWidget;
        point = receiverWidget->mapFrom(view, point);
    }
    else
    {
        qDebug() << "[WD] mouse button up at view " << point;
        receiverWidget = view;
    }

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void Automation::MouseButtonDownDeprecated(const WebViewId &view_id, const Point &p, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QPoint point = ConvertPointToQPoint(p);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget)
    {
        qDebug() << "[WD] mouse move at view " << point << " on element: " << receiverWidget;
        point = receiverWidget->mapFrom(view, point);
    }
    else
    {
        qDebug() << "[WD] mouse move at view " << point;
        receiverWidget = view;
    }

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(receiverWidget, pressEvent);
}

void Automation::MouseDoubleClickDeprecated(const WebViewId &view_id, const Point &p, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QPoint point = ConvertPointToQPoint(p);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget)
    {
        qDebug() << "[WD] mouse move at view " << point << " on element: " << receiverWidget;
        point = receiverWidget->mapFrom(view, point);
    }
    else
    {
        qDebug() << "[WD] mouse move at view " << point;
        receiverWidget = view;
    }

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, dbEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
}

void Automation::DragAndDropFilePaths(const WebViewId &view_id, const Point &location,
                                      const std::vector<FilePath::StringType> &paths, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QMimeData data;
    QList<QUrl> urls;

#if defined(OS_POSIX)
    for (uint i = 0; i < paths.size(); i++)
        urls.append(QUrl(paths.at(i).c_str()));
#elif defined(OS_WIN)
    QString string;

    for (uint i = 0; i < paths.size(); i++)
    {
        string = QString::fromUtf16((ushort*)paths.at(i).c_str());
        urls.append(QUrl(string));
    }
#endif // OS_WIN

    data.setUrls(urls);

    // TODO: verify this

    QDragEnterEvent *dragEnterEvent = new QDragEnterEvent(ConvertPointToQPoint(location), Qt::CopyAction, &data, Qt::LeftButton,  Qt::NoModifier);
    QDragMoveEvent *dragMoveEvent = new QDragMoveEvent(ConvertPointToQPoint(location), Qt::CopyAction, &data, Qt::LeftButton,  Qt::NoModifier);
    QDropEvent *dropEvent = new QDropEvent(ConvertPointToQPoint(location), Qt::CopyAction, &data, Qt::LeftButton,  Qt::NoModifier);
    QApplication::sendEvent(view, dragEnterEvent);
    QApplication::sendEvent(view, dragMoveEvent);
    QApplication::sendEvent(view, dropEvent);
}

void Automation::SendWebKeyEvent(const WebViewId &view_id, const WebKeyEvent &key_event, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QKeyEvent keyEvent = ConvertToQtKeyEvent(key_event);
//    qDebug()<<"[WD]:"<<keyEvent.type() << keyEvent.text() << keyEvent.key();
    qApp->sendEvent(view, &keyEvent);
}

void Automation::SendNativeElementWebKeyEvent(const WebViewId &view_id, const ElementId &element, const WebKeyEvent &key_event, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    if (!pWidget->isVisible())
    {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pWidget->isEnabled())
    {
        *error = new Error(kInvalidElementState);
        return;
    }

    QKeyEvent keyEvent = ConvertToQtKeyEvent(key_event);
    qDebug()<<"[WD] SendNativeElementWebKeyEvent: " << keyEvent.type() << keyEvent.text() << keyEvent.key();
    qApp->sendEvent(pWidget, &keyEvent);

}

void Automation::SendNativeKeyEvent(const WebViewId &view_id, ui::KeyboardCode key_code,
                                    int modifiers, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    Qt::KeyboardModifier mods = static_cast<Qt::KeyboardModifier>(modifiers);
    QKeyEvent pressKeyEvent(QEvent::KeyPress, key_code, mods);
    QKeyEvent releaseKeyEvent(QEvent::KeyRelease, key_code, mods);

    QApplication::sendEvent(view, &pressKeyEvent);
    QApplication::sendEvent(view, &releaseKeyEvent);
}

//void Automation::SendWebMouseEvent(const WebViewId &view_id, const WebMouseEvent &event, Error **error)
//{
//  WebViewLocator view_locator;
//  *error = ConvertViewIdToLocator(view_id, &view_locator);
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendWebMouseEventJSONRequest(
//          automation(), view_locator, event, &auto_error)) {
//    *error = Error::FromAutomationError(auto_error);
//  }
//}

void Automation::CaptureEntirePageAsPNG(const WebViewId &view_id, const FilePath &path, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();
    QPixmap pixmap = QPixmap::grabWidget(view);

    bool saved = false;

#if defined(OS_POSIX)
    saved = pixmap.save(path.value().c_str());
#elif defined(OS_WIN)
    saved = pixmap.save(QString::fromUtf16((ushort*)path.value().c_str()));
#endif // OS_WIN

    if (false == saved)
    {
        automation::Error autoError("screen wasn't captured");

        Error *pointerToError = Error::FromAutomationError(autoError);
        error = &pointerToError;
    }
}

//#if !defined(NO_TCMALLOC) && (defined(OS_LINUX) || defined(OS_CHROMEOS))
//void Automation::HeapProfilerDump(const WebViewId &view_id, const std::string &reason, Error **error)
//{
//  WebViewLocator view_locator;
//  *error = ConvertViewIdToLocator(view_id, &view_locator);
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendHeapProfilerDumpJSONRequest(
//          automation(), view_locator, reason, &auto_error)) {
//    *error = Error::FromAutomationError(auto_error);
//  }
//}
//#endif  // !defined(NO_TCMALLOC) && (defined(OS_LINUX) || defined(OS_CHROMEOS))

void Automation::NavigateToURL(const WebViewId &view_id, const std::string &url, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());
//TODO: Need review when Automation::WaitForAllViewsToStopLoading(Error** error)
//      will be ready
    QUrl address(QString(url.c_str()));

    qDebug()<<"[WD]:"<<address;

    view->stop();
    connect(view, SIGNAL(loadFinished(bool)),this, SLOT(pageLoadFinished()), Qt::QueuedConnection);
    connect(view, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()), Qt::QueuedConnection);
    connect(view, SIGNAL(loadStarted()),this, SLOT(pageLoadStarted()));
    view->load(address);
    if (isLoading)
        loop.exec();

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        view->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }
}


void Automation::NavigateToURLAsync(const WebViewId &view_id, const std::string &url, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QUrl address(QString(url.c_str()));
    view->load(address);

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        view->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }
}

void Automation::GoForward(const WebViewId &view_id, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QWebHistory *history = view->history();
    history->forward();

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        view->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }
}

void Automation::GoBack(const WebViewId &view_id, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QWebHistory *history = view->history();
    history->back();

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        view->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }
}

void Automation::Reload(const WebViewId &view_id, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    view->reload();

    if (SessionManager::GetInstance()->is_wi_enabled())
    {
        view->page()->setProperty("_q_webInspectorServerPort", SessionManager::GetInstance()->get_wi_port());
    }
}

void Automation::GetCookies(const WebViewId &view_id, const std::string &url, base::ListValue **cookies, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QString qUrl = url.c_str();
    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (NULL == jar)
    {
        automation::Error auto_error(automation::kInvalidId, "No active NetworkCookieJar");
        Error *pError = Error::FromAutomationError(auto_error);
        error = &pError;
        return;
    }

    QList<QNetworkCookie> cookies_list = jar->cookiesForUrl(QUrl(qUrl));

    ListValue* list = new ListValue();
    for (int i = 0; i < cookies_list.size(); ++i)
    {
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
     if (!cookies_value->IsType(Value::TYPE_LIST))
     {
         automation::Error auto_error(automation::kInvalidId);
         Error *pError = Error::FromAutomationError(auto_error);
         error = &pError;
         return;
     }

     *cookies = static_cast<ListValue*>(cookies_value.release());
}

void Automation::DeleteCookie(const WebViewId &view_id, const std::string &url, const std::string &cookie_name, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QString qUrl = url.c_str();
    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (NULL == jar)
    {
        automation::Error auto_error(automation::kInvalidId, "No active NetworkCookieJar");
        Error *pError = Error::FromAutomationError(auto_error);
        error = &pError;
        return;
    }

    QList<QNetworkCookie> cookies = jar->cookiesForUrl(QUrl(qUrl));

    if (cookies.isEmpty())
        return;

    QList<QNetworkCookie>::Iterator it = cookies.begin();
    QList<QNetworkCookie>::Iterator end = cookies.end();

    while (it != end)
    {
        std::string str_name(it->name().data());

        if (str_name == cookie_name)
        {
            it = cookies.erase(it);

            // NOTE: use QNetworkCookieJar::deleteCookie in case QT 5.0
            QNetworkCookieJar * newCookieJar = new QNetworkCookieJar();
            newCookieJar->setCookiesFromUrl(cookies, QUrl(qUrl));
            view->page()->networkAccessManager()->setCookieJar(newCookieJar);
            return;
        }
        else
        {
            it++;
        }
    }

    automation::Error auto_error(automation::kInvalidId, "No such cookie");
    Error *pError = Error::FromAutomationError(auto_error);
    error = &pError;
}

void Automation::SetCookie(const WebViewId &view_id, const std::string &url, base::DictionaryValue *cookie_dict, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    QList<QNetworkCookie> cookie_list;
    std::string name, value;
    std::string domain;
    std::string path = "/";
    bool secure = false;
    double expiry = 0;
    bool http_only = false;

    if (!cookie_dict->GetString("name", &name))
    {
        automation::Error auto_error(automation::kInvalidId, "'name' missing or invalid");
        Error *pError = Error::FromAutomationError(auto_error);
        error = &pError;
        return;
    }

    if (!cookie_dict->GetString("value", &value))
    {
        automation::Error auto_error(automation::kInvalidId, "'value' missing or invalid");
        Error *pError = Error::FromAutomationError(auto_error);
        error = &pError;
        return;
    }

    QNetworkCookie cookie(QByteArray(name.c_str()), QByteArray(value.c_str()));

    if (cookie_dict->HasKey("domain"))
    {
        if (!cookie_dict->GetString("domain", &domain))
        {
            automation::Error auto_error(automation::kInvalidId, "optional 'domain' invalid");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }

        qDebug() << "[WD]:" << "domain.c_str()=[" << domain.c_str() <<"]";

        // TODO: check why it fails here
        //cookie.setDomain(QString(domain.c_str()));
    }

    if (cookie_dict->HasKey("path"))
    {
        if (!cookie_dict->GetString("path", &path))
        {
            automation::Error auto_error(automation::kInvalidId, "optional 'path' invalid");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }

        cookie.setPath(QString(path.data()));
    }

    if (cookie_dict->HasKey("secure"))
    {
        if (!cookie_dict->GetBoolean("secure", &secure))
        {
            automation::Error auto_error(automation::kInvalidId, "optional 'secure' invalid");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }

        cookie.setSecure(secure);
    }

    if (cookie_dict->HasKey("expiry"))
    {
        if (!cookie_dict->GetDouble("expiry", &expiry))
        {
            automation::Error auto_error(automation::kInvalidId, "optional 'expiry' invalid");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }

        time_t time = (base::Time::FromDoubleT(expiry)).ToTimeT();
        //qDebug()<<"[WD]:" << "time=[" << time <<"]";

        QDateTime qtime;
        qtime.setTime_t(time);

        if (qtime > QDateTime::currentDateTime())
        {
            qDebug()<<"[WD]:"<< "Adding cookie";
            cookie.setExpirationDate(qtime);
        }
        else
        {
            qDebug()<<"[WD]:" << "cookie expired";
            automation::Error auto_error(automation::kInvalidId, "Could not set expired cookie");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }
    }

    if (cookie_dict->HasKey("http_only"))
    {
        if (!cookie_dict->GetBoolean("http_only", &http_only))
        {
            automation::Error auto_error(automation::kInvalidId, "optional 'http_only' invalid");
            Error *pError = Error::FromAutomationError(auto_error);
            error = &pError;
            return;
        }

        cookie.setHttpOnly(http_only);
    }

    cookie_list.append(cookie);

    QNetworkCookieJar* jar = view->page()->networkAccessManager()->cookieJar();

    if (!jar)
    {
        jar = new QNetworkCookieJar(this);
        view->page()->networkAccessManager()->setCookieJar(jar);
    }

    if (!jar->setCookiesFromUrl(cookie_list, QUrl(url.c_str())))
    {
        automation::Error autoError(automation::kInvalidId, "Could not set the cookie");
        *error = Error::FromAutomationError(autoError);
    }
}

void Automation::GetViews(std::vector<WebViewInfo>* views,
                          Error** error)
{
    std::string extension_id;
    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        if (!pWidget->isHidden())
        {
            QWebView* pView = qobject_cast<QWebView*>(pWidget);
            if (pView != NULL || pWidget->isTopLevel())
            {
                WebViewId pWebView = WebViewId::ForQtView(pWidget);
                views->push_back(WebViewInfo(pWebView, extension_id));
            }
        }
    }
}

void Automation::DoesViewExist(WebViewId *view_id, bool *does_exist, Error **error)
{
    error = NULL;
    *does_exist = false;

    foreach(QWidget* pWidget, qApp->allWidgets())
    {
        QWebView* pView = qobject_cast<QWebView*>(pWidget);
        if (pView != NULL || pWidget->isTopLevel())
        {
            QVariant automationIdVar = pWidget->property("automationId");
            int automationId;
            base::StringToInt(view_id->GetId().id(), &automationId);
            if (automationIdVar.isValid() && (automationIdVar.toInt() == automationId))
            {
                *does_exist = true;
                *view_id = WebViewId::ForQtView(pWidget);
                break;
            }
        }
    }
}

void Automation::CloseView(const WebViewId &view_id, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    ElementMap* elementsMap = NULL;
    QString elementKey(view_id.GetId().id().c_str());
    if (windowsElementMap.contains(elementKey))
    {
        elementsMap = windowsElementMap.value(elementKey);
        if (elementsMap != NULL)
        {
            windowsElementMap.remove(elementKey);
            delete elementsMap;
        }
    }

    QWidget *view = view_id.GetView();
    logger_.Log(kWarningLogLevel, "Automation::CloseView");

    view->setAttribute(Qt::WA_DeleteOnClose, true);
    view->close();
}

void Automation::GetViewBounds(const WebViewId &view_id, Rect *bounds, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    *bounds = ConvertQRectToRect(view->geometry());
}

void Automation::GetViewTitle(const WebViewId &view_id, std::string* title, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    *title = view->windowTitle().toStdString();
}


void Automation::SetViewBounds(const WebViewId &view_id, const Rect &bounds, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    view->setGeometry(ConvertRectToQRect(bounds));
}

void Automation::MaximizeView(const WebViewId &view_id, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    view->setGeometry(QApplication::desktop()->rect());
}

QWidget* Automation::GetNativeElement(const WebViewId &view_id, const ElementId &element)
{
    // get elements map for this view. If doesnt exist create new one
    ElementMap* elementsMap = NULL;
    QString viewKey(view_id.GetId().id().c_str());
    if (windowsElementMap.contains(viewKey))
    {
        elementsMap = windowsElementMap.value(viewKey);
    }

    if (elementsMap == NULL)
    {
        qWarning() << "[WD] GetNativeElement: elementsMap for view not found.";
        return NULL;
    }

    // get widget for key
    QPointer<QWidget> rootWidget = elementsMap->value(QString(element.id().c_str()));
    if (rootWidget != NULL)
    {
        // found
        return rootWidget.data();
    }

    // TODO: add kStaleElementReference error
    qWarning() << "[WD] GetNativeElement: element " << element.id().c_str() << " not found.";

    return NULL;
}

void Automation::GetNativeElementSize(const WebViewId& view_id,
                       const ElementId& element,
                       Size* size,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    *size = Size(pWidget->width(), pWidget->height());
}

void Automation::GetNativeElementWithFocus(const WebViewId& view_id,
                       ElementId* element,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();
    QWidget *focusWidget = QApplication::focusWidget();

    if (NULL == focusWidget || view == focusWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    // TODO: do we need to check if focusWidget is child of view?

    // get elements map for this view. If doesnt exist create new one
    QString viewKey(view_id.GetId().id().c_str());
    ElementMap* elementsMap = NULL;
    if (windowsElementMap.contains(viewKey))
    {
        elementsMap = windowsElementMap.value(viewKey);
    }
    else
    {
        elementsMap = new ElementMap();
        windowsElementMap.insert(viewKey, elementsMap);
    }

    // generate element id and save it in map
    QString elementKey = GenerateElementKey(focusWidget);
    elementsMap->insert(elementKey, QPointer<QWidget>(focusWidget));
    *element = ElementId(elementKey.toStdString());

    qDebug() << "[WD] GetNativeElementWithFocus, found:" << focusWidget << " key:" << elementKey;
}

void Automation::GetNativeElementLocation(const WebViewId& view_id,
                       const ElementId& element,
                       Point* location,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();
    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    qDebug() << "[WD] GetNativeElementLocation: " << pos;

    // TODO: replace with smth more correct :)
    *location = Point(pos.x(), pos.y());
}

void Automation::GetNativeElementProperty(const WebViewId& view_id,
                       const ElementId& element,
                       const std::string& name,
                       base::Value** value,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    qDebug() << "[WD] GetNativeElementProperty: " << name.c_str();

    QVariant propertyValue = pWidget->property(name.c_str());

    if (!propertyValue.isValid())
    {
        qDebug() << "[WD] GetNativeElementProperty: " << name.c_str() << " not found.";
        *value = NULL;
        return;
    }

    qDebug() << "[WD] GetNativeElementProperty: " << name.c_str() << " value: " << propertyValue;

    // convert QVariant to base::Value
    Value* val = NULL;

    switch (propertyValue.type())
    {
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
        qWarning() << "[WD] GetNativeElementProperty: " << name.c_str() << ", cant handle type " <<  propertyValue.type();
    }

    if (NULL == val)
    {
        *value = NULL;
    }
    else
    {
        scoped_ptr<Value> ret_value(val);
        *value = static_cast<Value*>(ret_value.release());
    }
}

void Automation::NativeElementEquals(const WebViewId& view_id,
                       const ElementId& element1,
                       const ElementId& element2,
                       bool* is_equals,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget1 = GetNativeElement(view_id, element1);
    QWidget *pWidget2 = GetNativeElement(view_id, element2);


    qDebug() << "[WD] NativeElementEquals: el1: " << pWidget1 << " el2: " << pWidget2 << " equals:" << (pWidget1 == pWidget2);

    *is_equals = (pWidget1 == pWidget2);
}

void Automation::GetNativeElementClickableLocation(const WebViewId& view_id,
                       const ElementId& element,
                       Point* location,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();
    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    if (!pWidget->isVisible())
    {
        *error = new Error(kElementNotVisible);
        return;
    }

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    // TODO: get pos in view, not in widget
    pos.setX(pos.x() + pWidget->width()/2);
    pos.setY(pos.y() + pWidget->height()/2);

    qDebug() << "[WD] GetNativeElementClickableLocation: " << pos;

    *location = Point(pos.x(), pos.y());
}

void Automation::GetNativeElementLocationInView(const WebViewId& view_id,
                       const ElementId& element,
                       Point* location,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();
    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    qDebug() << "[WD] GetNativeElementLocationInView: " << pos;

    // TODO: replace with smth more correct :)
    // TODO: get location in view
    *location = Point(pos.x(), pos.y());
}

void Automation::ClearNativeElement(const WebViewId& view_id,
                       const ElementId& element,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    if (!pWidget->isVisible())
    {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pWidget->isEnabled())
    {
        *error = new Error(kInvalidElementState);
        return;
    }

    // check if we can clear element
    QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit*>(pWidget);
    if (NULL != plainTextEdit)
    {
        plainTextEdit->clear();
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(pWidget);
    if (NULL != lineEdit)
    {
        lineEdit->clear();
        return;
    }

    QComboBox *comboBox = qobject_cast<QComboBox*>(pWidget);
    if (NULL != comboBox)
    {
        if (!comboBox->isEditable())
        {
            *error = new Error(kInvalidElementState);
            return;
        }

        comboBox->clearEditText();
        return;
    }

    *error = new Error(kNoSuchElement);
    return;
}

void Automation::IsNativeElementDisplayed(const WebViewId& view_id,
                       const ElementId& element,
                       bool ignore_opacity,
                       bool* is_displayed,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    // TODO: take in account ignore_opacity argument
    *is_displayed = pWidget->isVisible();
}

void Automation::IsNativeElementEnabled(const WebViewId& view_id,
                       const ElementId& element,
                       bool* is_enabled,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    *is_enabled = pWidget->isEnabled();
}

void Automation::IsNativeElementSelected(const WebViewId& view_id,
                       const ElementId& element,
                       bool* is_selected,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    QCheckBox *checkBox = qobject_cast<QCheckBox*>(pWidget);
    if (NULL != checkBox)
    {
        *is_selected = checkBox->isChecked();
        return;
    }

    QRadioButton *radioButton = qobject_cast<QRadioButton*>(pWidget);
    if (NULL != radioButton)
    {
        *is_selected = radioButton->isChecked();
        return;
    }

    *error = new Error(kInvalidElementState);
}

void Automation::GetNativeElementText(const WebViewId &view_id,
                       const ElementId& element,
                       std::string* element_text,
                       Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

   QWidget *pWidget = GetNativeElement(view_id, element);

    if (NULL == pWidget)
    {
        *error = new Error(kNoSuchElement);
        return;
    }

    QComboBox *comboBox = qobject_cast<QComboBox*>(pWidget);
    if (NULL != comboBox)
    {
        *element_text = comboBox->currentText().toStdString();
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(pWidget);
    if (NULL != lineEdit)
    {
        *element_text = lineEdit->text().toStdString();
        return;
    }

    QPlainTextEdit *plainText = qobject_cast<QPlainTextEdit*>(pWidget);
    if (NULL != plainText)
    {
        *element_text = plainText->toPlainText().toStdString();
        return;
    }

    *error = new Error(kNoSuchElement);
}


void Automation::FindNativeElement(const WebViewId& view_id,
                       const ElementId& root_element,
                       const std::string& locator,
                       const std::string& query,
                       ElementId* element,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    std::vector<ElementId> elements;

    FindNativeElements(view_id, root_element, locator, query, &elements, error);
    if (*error == NULL)
        *element = elements[0];
}

void Automation::FindNativeElements(const WebViewId& view_id,
                       const ElementId& root_element,
                       const std::string& locator,
                       const std::string& query,
                       std::vector<ElementId>* elements,
                       Error** error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    // get elements map for this view. If doesnt exist create new one
    QString viewKey(view_id.GetId().id().c_str());
    ElementMap* elementsMap = NULL;
    if (windowsElementMap.contains(viewKey))
    {
        elementsMap = windowsElementMap.value(viewKey);
    }
    else
    {
        elementsMap = new ElementMap();
        windowsElementMap.insert(viewKey, elementsMap);
    }

    qDebug() << "[WD] FindNativeElements, loc:" << locator.c_str() << " query:" << query.c_str();

    QString elementKey(root_element.id().c_str());
    QWidget *parent = view_id.GetView();
    if (!elementKey.isEmpty())
    {
        // get widget for key
        QPointer<QWidget> rootWidget = elementsMap->value(QString(elementKey));
        if (rootWidget != NULL)
            parent = rootWidget.data();
    }

    // list all child widgets and find matched locator
    QList<QWidget*> childs = parent->findChildren<QWidget*>();
    foreach(QWidget *child, childs)
    {
        if (FilterNativeWidget(child, locator, query))
        {
            // generate element id and save it in map
            elementKey = GenerateElementKey(child);
            elementsMap->insert(elementKey, QPointer<QWidget>(child));
            (*elements).push_back(ElementId(elementKey.toStdString()));

            qDebug() << "[WD] FindNativeElements, found:" << child << " key:" << elementKey;
        }
    }
}

QString Automation::GenerateElementKey(const QWidget* widget)
{
    char key[16];
    qsrand(QTime::currentTime().msec());

#if defined(OS_WIN)
    _snprintf(key, 16, ":qtw:%08x", qrand());
#else
	snprintf(key, 16, ":qtw:%08x", qrand());
#endif

    return QString(key);
}

bool Automation::FilterNativeWidget(const QWidget* widget, const std::string& locator, const std::string& query)
{
    if (locator == LocatorType::kClassName)
    {
        if (query == widget->metaObject()->className())
            return true;
    }
#ifdef WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY
    else if (locator == LocatorType::kId)
    {
        if (query == widget->accessibleName().toStdString())
            return true;
    }
#endif
    else if (locator == LocatorType::kName)
    {
        if (query == widget->windowTitle().toStdString())
            return true;
    }
    else if (locator == LocatorType::kXpath)
    {
        qWarning() << "[WD] unimplemented locator:" << locator.c_str();
        // TODO: implement if possible
        return false;
    }
    else
    {
        qWarning() << "[WD] unsupported locator:" << locator.c_str();
        // LocatorType::kLinkText
        // LocatorType::kPartialLinkText
        // LocatorType::kCss
        // LocatorType::kTagName
    }

    return false;
}


void Automation::GetAppModalDialogMessage(const WebViewId& view_id, std::string* message, Error** error)
{
  *error = CheckAlertsSupported();
  if (*error)
    return;

  if(!checkView(view_id))
  {
      *error = new Error(kNoSuchWindow);
      return;
  }

  QWidget *view = view_id.GetView();

  // QMessageBox::information(pWeb, "Alert", message->c_str(), QMessageBox::Ok);
  QMessageBox *msgBox = view->findChild<QMessageBox*>();
  if (NULL != msgBox)
  {
      std::string text = msgBox->text().toStdString();
      *message = text;
  }
  else
  {
      QInputDialog *msgbox = view->findChild<QInputDialog*>();

      if (NULL != msgbox)
      {
          std::string txt = msgbox->labelText().toStdString();
          *message = txt;
      }
      else
      {
        *error = new Error(kNoAlertOpenError);
      }
  }

  // automation::Error auto_error;
  /*if (!SendGetAppModalDialogMessageJSONRequest(
          automation(), message, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }*/
}

void Automation::AcceptOrDismissAppModalDialog(const WebViewId& view_id, bool accept, Error** error)
{
  *error = CheckAlertsSupported();
  if (*error)
    return;

  if(!checkView(view_id))
  {
      *error = new Error(kNoSuchWindow);
      return;
  }

  QWidget *view = view_id.GetView();

  // automation::Error auto_error;

  QMessageBox *msgBox = view->findChild<QMessageBox*>();

  if(NULL != msgBox)
  {
      if(accept)
      {
        msgBox->accept();
      }
      else
      {
          msgBox->close();
      }
  }  
  else
  {
      QInputDialog *msgbox = view->findChild<QInputDialog*>();
      if(NULL != msgbox)
      {
          if(accept)
          {
            msgbox->accept();
          }
          else
          {
              msgbox->close();
          }
      }
      else
      {
        *error = new Error(kNoAlertOpenError);
      }
  }
  /*if (!SendAcceptOrDismissAppModalDialogJSONRequest(
          automation(), accept, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }*/
}

void Automation::AcceptPromptAppModalDialog(const WebViewId& view_id,
                                            const std::string& prompt_text,
                                            Error** error)
{
  *error = CheckAlertsSupported();
  if (*error)
    return;

  if(!checkView(view_id))
  {
      *error = new Error(kNoSuchWindow);
      return;
  }

  QWidget *view = view_id.GetView();

  // automation::Error auto_error;

  QMessageBox *msgBox = view->findChild<QMessageBox*>();

  if(NULL != msgBox)
  {
      msgBox->accept();
  }
  else
  {
      QInputDialog *msgbox = view->findChild<QInputDialog*>();
      if(NULL != msgbox)
      {
           msgbox->accept();
      }
      else
      {
        *error = new Error(kNoAlertOpenError);
      }
  }

  /*if (!SendAcceptPromptAppModalDialogJSONRequest(
          automation(), prompt_text, &auto_error)) {
    *error = Error::FromAutomationError(auto_error);
  }*/
}

//void Automation::GetBrowserVersion(std::string* version)
//{
//  *version = automation()->server_version();
//}

//void Automation::GetChromeDriverAutomationVersion(int* version, Error** error)
//{
//  automation::Error auto_error;
//  if (!SendGetChromeDriverAutomationVersion(automation(), version, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::WaitForAllViewsToStopLoading(Error** error)
//{
//   automation::Error auto_error;
//  if (!SendWaitForAllViewsToStopLoadingJSONRequest(automation(), &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::InstallExtension(const FilePath &path, std::string *extension_id, Error **error)
//{
//  *error = CheckNewExtensionInterfaceSupported();
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendInstallExtensionJSONRequest(
//          automation(), path, false /* with_ui */, extension_id,
//          &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::GetExtensionsInfo(
//    base::ListValue* extensions_list, Error** error)
//{
//  *error = CheckNewExtensionInterfaceSupported();
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendGetExtensionsInfoJSONRequest(
//          automation(), extensions_list, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::IsPageActionVisible(const WebViewId &tab_id, const std::string &extension_id,
//                                     bool *is_visible, Error **error)
//{
//  *error = CheckNewExtensionInterfaceSupported();
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendIsPageActionVisibleJSONRequest(
//          automation(), tab_id, extension_id, is_visible, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::SetExtensionState(
//    const std::string& extension_id,
//    bool enable,
//    Error** error)
//{
//  *error = CheckNewExtensionInterfaceSupported();
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendSetExtensionStateJSONRequest(
//          automation(), extension_id, enable /* enable */,
//          false /* allow_in_incognito */, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::ClickExtensionButton(
//    const std::string& extension_id,
//    bool browser_action,
//    Error** error)
//{
//  automation::Error auto_error;
//  if (!SendClickExtensionButtonJSONRequest(
//          automation(), extension_id, browser_action, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::UninstallExtension(
//    const std::string& extension_id, Error** error)
//{
//  *error = CheckNewExtensionInterfaceSupported();
//  if (*error)
//    return;

//  automation::Error auto_error;
//  if (!SendUninstallExtensionJSONRequest(
//          automation(), extension_id, &auto_error))
//    *error = Error::FromAutomationError(auto_error);
//}

//void Automation::SetLocalStatePreference(const std::string &pref, base::Value *value, Error **error)
//{
//  scoped_ptr<Value> scoped_value(value);
//  // In version 927, SetLocalStatePrefs was changed from taking a browser
//  // handle to a browser index.
//  if (build_no_ >= 927) {
//    automation::Error auto_error;
//    if (!SendSetLocalStatePreferenceJSONRequest(
//            automation(), pref, scoped_value.release(), &auto_error))
//      *error = Error::FromAutomationError(auto_error);
//  } else {
//    std::string request, response;
//    DictionaryValue request_dict;
//    request_dict.SetString("command", "SetLocalStatePrefs");
//    request_dict.SetString("path", pref);
//    request_dict.Set("value", scoped_value.release());
//    base::JSONWriter::Write(&request_dict, &request);
//    if (!automation()->GetBrowserWindow(0)->SendJSONRequest(
//            request, -1, &response)) {
//      *error = new Error(kUnknownError, base::StringPrintf(
//          "Failed to set local state pref '%s'", pref.c_str()));
//    }
//  }
//}

//void Automation::SetPreference(const std::string &pref, base::Value *value, Error **error)
//{
//    scoped_ptr<Value> scoped_value(value);
//    // Chrome 17 is on the 963 branch. The first released 18 build should have
//    // the new SetPrefs method which uses a browser index instead of handle.
//    if (build_no_ >= 964) {
//      automation::Error auto_error;
//      if (!SendSetPreferenceJSONRequest(
//              automation(), pref, scoped_value.release(), &auto_error))
//        *error = Error::FromAutomationError(auto_error);
//    } else {
//      std::string request, response;
//      DictionaryValue request_dict;
//      request_dict.SetString("command", "SetPrefs");
//      request_dict.SetInteger("windex", 0);
//      request_dict.SetString("path", pref);
//      request_dict.Set("value", scoped_value.release());
//      base::JSONWriter::Write(&request_dict, &request);
//      if (!automation()->GetBrowserWindow(0)->SendJSONRequest(
//              request, -1, &response)) {
//        *error = new Error(kUnknownError, base::StringPrintf(
//            "Failed to set pref '%s'", pref.c_str()));
//      }
//    }
//}

void Automation::GetGeolocation(scoped_ptr<base::DictionaryValue> *geolocation, Error **error)
{
  *error = CheckGeolocationSupported();
  if (*error)
    return;

  if (geolocation_.get()) {
    geolocation->reset(geolocation_->DeepCopy());
  } else {
    *error = new Error(kUnknownError,
                       "Location must be set before it can be retrieved");
  }
}

void Automation::OverrideGeolocation(const DictionaryValue* geolocation,
                                     Error** error)
{
  *error = CheckGeolocationSupported();
  if (*error)
    return;

  // TODO: use QtLocation class in QT 5.0 in the future
//  automation::Error auto_error;
//  if (SendOverrideGeolocationJSONRequest(
//          automation(), geolocation, &auto_error)) {
//    geolocation_.reset(geolocation->DeepCopy());
//  } else {
//    *error = Error::FromAutomationError(auto_error);
//  }
}

//AutomationProxy* Automation::automation() const
//{
//    return NULL;
//    // return launcher_->automation();
//}

Error* Automation::DetermineBuildNumber()
{
  /*std::string version = automation()->server_version();
  std::vector<std::string> split_version;
  base::SplitString(version, '.', &split_version);
  if (split_version.size() != 4) {
    return new Error(
        kUnknownError, "Browser version has unrecognized format: " + version);
  }
  if (!base::StringToInt(split_version[2], &build_no_)) {
    return new Error(
        kUnknownError, "Browser version has unrecognized format: " + version);
  }*/
  return NULL;
}

Error* Automation::CheckVersion(int min_required_build_no, const std::string &error_msg)
{
  /* if (build_no_ < min_required_build_no)
    return new Error(kUnknownError, error_msg);*/
  return NULL;
}

Error* Automation::CheckAlertsSupported() {
  return CheckVersion(
      768, "Alerts are not supported for this version of Chrome");
}

Error* Automation::CheckAdvancedInteractionsSupported()
{
  const char* message =
      "Advanced user interactions are not supported for this version of Chrome";
  return CheckVersion(750, message);
}

Error* Automation::CheckNewExtensionInterfaceSupported()
{
  const char* message =
      "Extension interface is not supported for this version of Chrome";
  return CheckVersion(947, message);
}

Error* Automation::CheckGeolocationSupported()
{
  const char* message =
      "Geolocation automation interface is not supported for this version of "
      "Chrome.";
  return CheckVersion(1119, message);
}

Error* Automation::CheckMaximizeSupported()
{
  const char* message =
      "Maximize automation interface is not supported for this version of "
      "Chrome.";
  return CheckVersion(1160, message);
}

QWebView *Automation::ConvertViewIdToPointer(const WebViewId& view_id)
{
    //AutomationId id = view_id.GetId();
    return NULL;
}

QPoint Automation::ConvertPointToQPoint(const Point &p)
{
    QPoint resultPoint;
    resultPoint.setX(p.x());
    resultPoint.setY(p.y());

    return resultPoint;
}

Rect Automation::ConvertQRectToRect(const QRect &rect)
{
    return Rect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect Automation::ConvertRectToQRect(const Rect &rect)
{
    QRect resultRect;
    resultRect.setX(rect.x());
    resultRect.setY(rect.y());
    resultRect.setWidth(rect.width());
    resultRect.setHeight(rect.height());

    return resultRect;
}

Qt::MouseButton Automation::ConvertMouseButtonToQtMouseButton(automation::MouseButton button)
{
    Qt::MouseButton result = Qt::NoButton;

    switch(button)
    {
        case automation::kLeftButton: result = Qt::LeftButton; break;
        case automation::kMiddleButton: result = Qt::MidButton; break;
        case automation::kRightButton: result = Qt::RightButton; break;
        case automation::kNoButton: result = Qt::NoButton; break;
        default: result = Qt::NoButton;
    }

    return result;
}

QWebFrame* Automation::FindFrameByMeta(QWebFrame* parent, const FramePath &frame_path)
{
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

void Automation::AddIdToCurrentFrame(const WebViewId &view_id, const FramePath &frame_path, Error **error)
{
    // TODO: review
    error = NULL;
    QWebView *view = qobject_cast<QWebView*>(view_id.GetView());

    if(NULL == view)
    {
        automation::Error autoError(automation::kInvalidId);
        Error *pointerToError = Error::FromAutomationError(autoError);
        error = &pointerToError;
        return;
    }

    QWebFrame *pFrame = FindFrameByMeta(view->page()->mainFrame(), frame_path);

    pFrame->setProperty("frame_id", QString(frame_path.value().c_str()));
}

void Automation::SetAlertPromptText(const WebViewId& view_id, const std::string &text, Error **error)
{
    if(!checkView(view_id))
    {
        *error = new Error(kNoSuchWindow);
        return;
    }

    QWidget *view = view_id.GetView();

    QInputDialog *alert = view->findChild<QInputDialog*>();

    if (NULL != alert)
    {
        alert->setTextValue(text.c_str());
    }
    else
    {
        // in python ELEMENT_NOT_VISIBLE = 11
        // kNoAlertError = 27
        *error = new Error(kElementNotVisible);
    }
}

QWebFrame* Automation::FindFrameByPath(QWebFrame* parent, const FramePath &frame_path)
{
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

QKeyEvent Automation::ConvertToQtKeyEvent(const WebKeyEvent &key_event)
{
    QEvent::Type type;
    int keyCode;
    Qt::KeyboardModifiers modifiers;
    QString text;

    if ((key_event.type == automation::kKeyDownType) || (key_event.type == automation::kRawKeyDownType))
        type = QEvent::KeyPress;
    else
        type = QEvent::KeyRelease;

    if (key_event.modifiers & automation::kShiftKeyMask)
        modifiers |= Qt::ShiftModifier;
    if (key_event.modifiers & automation::kControlKeyMask)
        modifiers |= Qt::ControlModifier;
    if (key_event.modifiers & automation::kAltKeyMask)
        modifiers |= Qt::AltModifier;
    if (key_event.modifiers & automation::kMetaKeyMask)
        modifiers |= Qt::MetaModifier;

    if (keyMap.contains(key_event.key_code))
        keyCode = keyMap.value(key_event.key_code);
    else
        keyCode = key_event.key_code;
    text = key_event.unmodified_text.c_str();

    return QKeyEvent(type, keyCode, modifiers, text);
}

void Automation::BuildKeyMap()
{
    keyMap.insert(ui::VKEY_HELP,        Qt::Key_Help);
    keyMap.insert(ui::VKEY_BACK,        Qt::Key_Backspace);
    keyMap.insert(ui::VKEY_TAB,         Qt::Key_Tab);
    keyMap.insert(ui::VKEY_CLEAR,       Qt::Key_Clear);
    keyMap.insert(ui::VKEY_RETURN,      Qt::Key_Return);
    keyMap.insert(ui::VKEY_SHIFT,       Qt::Key_Shift);
    keyMap.insert(ui::VKEY_CONTROL,     Qt::Key_Control);
    keyMap.insert(ui::VKEY_MENU,        Qt::Key_Menu);
    keyMap.insert(ui::VKEY_PAUSE,       Qt::Key_Pause);
    keyMap.insert(ui::VKEY_ESCAPE,      Qt::Key_Escape);
    keyMap.insert(ui::VKEY_SPACE,       Qt::Key_Space);
    keyMap.insert(ui::VKEY_PRIOR,       Qt::Key_PageUp);
    keyMap.insert(ui::VKEY_NEXT,        Qt::Key_PageDown);
    keyMap.insert(ui::VKEY_END,         Qt::Key_End);
    keyMap.insert(ui::VKEY_HOME,        Qt::Key_Home);
    keyMap.insert(ui::VKEY_LEFT,        Qt::Key_Left);
    keyMap.insert(ui::VKEY_UP,          Qt::Key_Up);
    keyMap.insert(ui::VKEY_RIGHT,       Qt::Key_Right);
    keyMap.insert(ui::VKEY_DOWN,        Qt::Key_Down);
    keyMap.insert(ui::VKEY_INSERT,      Qt::Key_Insert);
    keyMap.insert(ui::VKEY_DELETE,      Qt::Key_Delete);
    keyMap.insert(ui::VKEY_OEM_1,       Qt::Key_Semicolon);
    keyMap.insert(ui::VKEY_OEM_PLUS,    Qt::Key_Equal);
    keyMap.insert(ui::VKEY_NUMPAD0,     Qt::Key_0);
    keyMap.insert(ui::VKEY_NUMPAD1,     Qt::Key_1);
    keyMap.insert(ui::VKEY_NUMPAD2,     Qt::Key_2);
    keyMap.insert(ui::VKEY_NUMPAD3,     Qt::Key_3);
    keyMap.insert(ui::VKEY_NUMPAD4,     Qt::Key_4);
    keyMap.insert(ui::VKEY_NUMPAD5,     Qt::Key_5);
    keyMap.insert(ui::VKEY_NUMPAD6,     Qt::Key_6);
    keyMap.insert(ui::VKEY_NUMPAD7,     Qt::Key_7);
    keyMap.insert(ui::VKEY_NUMPAD8,     Qt::Key_8);
    keyMap.insert(ui::VKEY_NUMPAD9,     Qt::Key_9);
    keyMap.insert(ui::VKEY_MULTIPLY,    Qt::Key_Asterisk);
    keyMap.insert(ui::VKEY_ADD,         Qt::Key_Plus);
    keyMap.insert(ui::VKEY_OEM_COMMA,   Qt::Key_Comma);
    keyMap.insert(ui::VKEY_SUBTRACT,    Qt::Key_Minus);
    keyMap.insert(ui::VKEY_DECIMAL,     Qt::Key_Period);
    keyMap.insert(ui::VKEY_DIVIDE,      Qt::Key_Slash);
    keyMap.insert(ui::VKEY_F1,          Qt::Key_F1);
    keyMap.insert(ui::VKEY_F2,          Qt::Key_F2);
    keyMap.insert(ui::VKEY_F3,          Qt::Key_F3);
    keyMap.insert(ui::VKEY_F4,          Qt::Key_F4);
    keyMap.insert(ui::VKEY_F5,          Qt::Key_F5);
    keyMap.insert(ui::VKEY_F6,          Qt::Key_F6);
    keyMap.insert(ui::VKEY_F7,          Qt::Key_F7);
    keyMap.insert(ui::VKEY_F8,          Qt::Key_F8);
    keyMap.insert(ui::VKEY_F9,          Qt::Key_F9);
    keyMap.insert(ui::VKEY_F10,         Qt::Key_F10);
    keyMap.insert(ui::VKEY_F11,         Qt::Key_F11);
    keyMap.insert(ui::VKEY_F12,         Qt::Key_F12);
    keyMap.insert(ui::VKEY_UNKNOWN,     Qt::Key_unknown);
}

bool Automation::checkView(const WebViewId &view_id)
{
    //Rework to check only pointer
    foreach(QWidget* pView, qApp->allWidgets())
    {
        QVariant automationIdVar = pView->property("automationId");
        int automationId;
        base::StringToInt(view_id.GetId().id(), &automationId);
        if (automationIdVar.isValid() && (automationIdVar.toInt() == automationId))
            //Need to check type of widget
            return true;
    }
    return false;
}

void Automation::pageLoadStarted()
{
    isLoading = true;
}

void Automation::pageLoadFinished()
{
    isLoading = false;
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

//bool Automation::eventFilter(QObject *obj, QEvent *event)
// {
//     if (obj == pWeb) {
//         if (event->type() == QEvent::Drop) {
//             qDebug()<<"[WD]:"<<&event;
//             return false;
//         }
//         else
//             return false;
//     } else {

//         return false;
//     }
// }

}  // namespace webdriver
