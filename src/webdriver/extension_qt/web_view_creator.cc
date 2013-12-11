#include "extension_qt/web_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"
#include "webdriver_error.h"

#include "web_view_util.h"
#include "extension_qt/widget_view_handle.h"
#include "common_util.h"
#include "q_content_type_resolver.h"
#include "q_event_filter.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QString>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

namespace webdriver {

QWebViewCreator::QWebViewCreator() {}

bool QWebViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className,
                                            const Point* position, const Size* size, ViewHandle** view) const {
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
        QWidget* widget = (dynamic_cast<QViewHandle*>(handle))->get();

        if (NULL == widget)
            return false;
        if (NULL != size && NULL != position) {
            Rect* rect = new Rect(*position, *size);
            widget->setGeometry(QCommonUtil::ConvertRectToQRect(*rect));
            delete rect;
        } else if (NULL != size) {
            widget->resize(QCommonUtil::ConvertSizeToQSize(*size));
        } else if (NULL != position) {
            int x_offset = widget->geometry().x() - widget->frameGeometry().x();
            int y_offset = widget->geometry().y() - widget->frameGeometry().y();
            widget->move(position->x() - x_offset, position->y() - y_offset);
        }
        *view = handle;
        return true;
    }

    // view was not created
    return false;
}

bool QWebViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url,
                                       const Point* position, const Size* size, ViewHandle** view) const {
    Error* tmp_err = NULL;
    if (!QWebViewUtil::isUrlSupported(url, &tmp_err)) {
        if (tmp_err) delete tmp_err;
        return false;
    }
    
    return CreateViewByClassName(logger, "", position, size, view);
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

        widget->setAttribute(Qt::WA_DeleteOnClose, true);

        return true;
    } else {
        logger.Log(kSevereLogLevel, "QWebViewCreator, smth wrong.");
        viewHandle->Release();
    }

    return false;
}



} // namespace webdriver
