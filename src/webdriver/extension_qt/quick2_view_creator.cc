#include "extension_qt/quick2_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"
#include "webdriver_error.h"

#include "qml_view_util.h"
#include "extension_qt/qwindow_view_handle.h"
#include "q_content_type_resolver.h"
#include "q_event_filter.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QString>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

namespace webdriver {

Quick2QmlViewCreator::Quick2QmlViewCreator() {}

bool Quick2QmlViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const {
	ViewHandle* handle = NULL;

    if (factory.empty())
        return false;

	if (className.empty() || className == "QMLView") {
		// get first found QML view
        CreateViewMethod createMethod = factory.begin()->second;
        handle = new QWindowViewHandle(static_cast<QWindow*>(createMethod()));
	} else {
    	FactoryMap::const_iterator it = factory.find(className);
        if (it != factory.end())
        {
        	CreateViewMethod createMethod = it->second;
            handle = new QWindowViewHandle(static_cast<QWindow*>(createMethod()));
        }
    }

    if (NULL != handle) {
        QWindow* pWindow = (dynamic_cast<QWindowViewHandle*>(handle))->get();
        
        if (NULL != pWindow) {
            std::string objClassName(pWindow->metaObject()->className());

            // TODO: replace this widget specific logic?
            QEventLoop loop;
            QRepaintEventFilter filter(pWindow);
            QCheckPagePaint painter;
            QObject::connect(&filter, SIGNAL(repainted()), &loop, SLOT(quit()));
            QObject::connect(&filter, SIGNAL(repainted()), &painter, SLOT(pagePainted()));

            QTimer timer;
            timer.setSingleShot(true);
            QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
            timer.start(500);

            pWindow->installEventFilter(&filter);
            
            pWindow->show();
            if (!painter.isPainting())
                loop.exec();
        
            logger.Log(kInfoLogLevel, "Quick2QmlViewCreator created view (" + objClassName +").");

            *view = handle;

            return true;
        } else {
            logger.Log(kSevereLogLevel, "Quick2QmlViewCreator, smth wrong.");
            handle->Release();
        }
    }

    // view was not created
    return false;
}

bool Quick2QmlViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const {
    if (!QQmlViewUtil::isUrlSupported(url)) {
        return false;
    }
    
    return CreateViewByClassName(logger, "", view);
}

} // namespace webdriver
