#include "extension_qt/widget_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "widget_view_util.h"

#include <QtGui/QWidget>

namespace webdriver {

QWidgetViewCreator::QWidgetViewCreator() {}

bool QWidgetViewCreator::CreateViewByClassName(Session* session, const std::string& className, ViewId* viewId) const {
	ViewHandle handle = INVALID_HANDLE;	

    if (factory.empty())
        return false;

	if (className.empty()) {
		// get first registered
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
                "QWidgetViewCreator created view(" + newView.id() +
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

bool QWidgetViewCreator::CreateViewForUrl(Session* session, const std::string& url, ViewId* viewId) const {
    if (factory.empty())
        return false;

	if (!QWidgetViewUtil::isUrlSupported(url))
        return false;
    
    ViewHandle handle = INVALID_HANDLE;
    // get first registered
    CreateViewMethod createMethod = factory.begin()->second;
    handle = createMethod();

    ViewId newView;

    if (INVALID_HANDLE != handle) {
        if (session->AddNewView(handle, &newView))  {
            *viewId = newView;
            session->logger().Log(kInfoLogLevel,
                "QWidgetViewCreator created view(" + newView.id() +
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
