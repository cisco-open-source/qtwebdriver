#include "extension_qt/web_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "web_view_util.h"

#include <QtWebKit/QtWebKit>

namespace webdriver {

QWebViewCreator::QWebViewCreator() {}

bool QWebViewCreator::CreateViewByClassName(Session* session, const std::string& className, ViewId* viewId) const {
	ViewHandle handle = INVALID_HANDLE;	

    if (factory.empty())
        return false;

	if (className.empty()) {
		// get first found QWebView
        CreateViewMethod createMethod = factory.begin()->second;
        handle = createMethod();
	} else {
    	FactoryMap::const_iterator it = factory.find(className);
        if (it != factory.end())
        {
        	CreateViewMethod createMethod = it->second;
            handle = createMethod();
        }
    }

    ViewId newView;

    if (INVALID_HANDLE != handle) {
        if (session->AddNewView(handle, &newView))  {
            *viewId = newView;
            session->logger().Log(kInfoLogLevel,
                "QWebViewCreator created view(" + newView.id() +
                ") by class name - "+className);

            // show
            QWidget* widget = static_cast<QWidget*>(handle);
            widget->show();
            return true;
        }
    }

    // view was not created
    return false;
}

bool QWebViewCreator::CreateViewForUrl(Session* session, const std::string& url, ViewId* viewId) const {
    if (factory.empty())
        return false;

    if (!QWebViewUtil::isUrlSupported(url))
        return false;

    ViewHandle handle = INVALID_HANDLE;
    // get first found QWebView
    CreateViewMethod createMethod = factory.begin()->second;
    handle = createMethod();

    ViewId newView;

    if (INVALID_HANDLE != handle) {
        if (session->AddNewView(handle, &newView))  {
            *viewId = newView;
            session->logger().Log(kInfoLogLevel,
                "QWebViewCreator created view(" + newView.id() +
                ") by url - "+url);

            // show
            QWidget* widget = static_cast<QWidget*>(handle);
            widget->show();
            return true;
        }
    }
	
	return false;
};



} // namespace webdriver
