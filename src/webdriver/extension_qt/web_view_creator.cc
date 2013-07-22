#include "extension_qt/web_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"
#include "webdriver_error.h"

#include "web_view_util.h"
#include "extension_qt/widget_view_handle.h"
#include "q_content_type_resolver.h"
#include "q_event_filter.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QString>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

namespace webdriver {

QWebViewCreator::QWebViewCreator() {}

bool QWebViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const {
	ViewHandle* handle = NULL;

    if (factory.empty())
        return false;

	if (className.empty() || className == "WebView") {
		// get first found QWebView
        CreateViewMethod createMethod = factory.begin()->second;
        handle = new QViewHandle(static_cast<QWidget*>(createMethod()));
	} else {
    	FactoryMap::const_iterator it = factory.find(className);
        if (it != factory.end())
        {
        	CreateViewMethod createMethod = it->second;
            handle = new QViewHandle(static_cast<QWidget*>(createMethod()));
        }
    }

    if (NULL != handle && ShowView(logger, handle)) {
        *view = handle;
        return true;
    }

    // view was not created
    return false;
}

bool QWebViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const {
    if (factory.empty())
        return false;
    
    Error* tmp_err = NULL;
    if (!QWebViewUtil::isUrlSupported(url, &tmp_err)) {
        if (tmp_err) delete tmp_err;
        return false;
    }
    
    // get first found QWebView
    CreateViewMethod createMethod = factory.begin()->second;
    ViewHandle* handle = new QViewHandle(static_cast<QWidget*>(createMethod()));

    if (NULL != handle && ShowView(logger, handle)) {
        *view = handle;
        return true;
    }

	return false;
}

bool QWebViewCreator::ShowView(const Logger& logger, ViewHandle* viewHandle) const {
    QWidget* widget = (dynamic_cast<QViewHandle*>(viewHandle))->get();

    if (NULL != widget) {
        std::string objClassName(widget->metaObject()->className());
        QEventLoop loop;
        QRepaintEventFilter filter(widget);
        QCheckPagePaint painter;
        QObject::connect(&filter, SIGNAL(repainted()), &loop, SLOT(quit()));
        QObject::connect(&filter, SIGNAL(repainted()), &painter, SLOT(pagePainted()));

        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start(500);

        widget->installEventFilter(&filter);
        widget->show();
        if (!painter.isPainting())
            loop.exec();
        
        logger.Log(kInfoLogLevel, "QWebViewCreator created view(" + objClassName + ").");

        return true;
    } else {
        logger.Log(kSevereLogLevel, "QWebViewCreator, smth wrong.");
        viewHandle->Release();
    }

    return false;
}



} // namespace webdriver
