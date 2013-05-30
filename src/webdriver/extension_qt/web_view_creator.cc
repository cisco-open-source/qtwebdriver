#include "extension_qt/web_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

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
            return true;
        }
    }

    // view was not created
    return false;
}

bool QWebViewCreator::CreateViewForUrl(Session* session, const std::string& url, ViewId* viewId) const {
	// TODO:
	session->logger().Log(kSevereLogLevel, "CreateViewForUrl not implemented, TBD.");
	return false;
};



} // namespace webdriver
