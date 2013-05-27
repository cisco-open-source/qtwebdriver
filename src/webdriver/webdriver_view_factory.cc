#include "webdriver_view_factory.h"

#include "webdriver_view_id.h"
#include "webdriver_session.h"

namespace webdriver {

ViewCreator::ViewCreator() {

}

ViewFactory* ViewFactory::instance = NULL;

ViewFactory* ViewFactory::GetInstance() {
	if (NULL == instance) {
		instance = new ViewFactory();
	}

	return instance;
}

void ViewFactory::CreateViewByClassName(Session* session, const std::string& className, ViewHandle* handle) const {
	CreatorsList::const_iterator creator;

	session->logger().Log(kInfoLogLevel, "ViewFactory::CreateViewByClassName - " + className);

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		if ((*creator)->CreateViewByClassName(session, className, handle)) {
			// succed, return
			return;
		}
	}

	session->logger().Log(kSevereLogLevel, "ViewFactory::CreateViewByClassName - cant create view.");

	*handle = INVALID_HANDLE;

	return;
}

void ViewFactory::CreateViewForUrl(Session* session, const std::string& url, ViewHandle* handle) const {
	CreatorsList::const_iterator creator;

	session->logger().Log(kInfoLogLevel, "ViewFactory::CreateViewForUrl - " + url);

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		if ((*creator)->CreateViewForUrl(session, url, handle)) {
			// succed, return
			return;
		}
	}

	session->logger().Log(kSevereLogLevel, "ViewFactory::CreateViewForUrl - cant create view.");

	*handle = INVALID_HANDLE;

	return;
}

void ViewFactory::AddViewCreator(ViewCreator* creator) {
	if (NULL != creator) {
		creators_.push_back(creator);
	}
}

} // namespace webdriver 

