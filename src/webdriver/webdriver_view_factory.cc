#include "webdriver_view_factory.h"

#include "webdriver_view_id.h"
#include "webdriver_session.h"

namespace webdriver {

ViewCreator::ViewCreator() {

}

ViewFactory* ViewFactory::instance = NULL;

ViewFactory::ViewFactory() {
}

ViewFactory* ViewFactory::GetInstance() {
	if (NULL == instance) {
		instance = new ViewFactory();
	}

	return instance;
}

void ViewFactory::CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const {
	CreatorsList::const_iterator creator;

	logger.Log(kInfoLogLevel, "ViewFactory::CreateViewByClassName - " + className);

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		if ((*creator)->CreateViewByClassName(logger, className, view)) {
			// succed, return
			return;
		}
	}

	logger.Log(kSevereLogLevel, "ViewFactory::CreateViewByClassName - cant create view.");
}

void ViewFactory::CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const {
	CreatorsList::const_iterator creator;

	logger.Log(kInfoLogLevel, "ViewFactory::CreateViewForUrl - " + url);

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		if ((*creator)->CreateViewForUrl(logger, url, view)) {
			// succed, return
			return;
		}
	}

	logger.Log(kSevereLogLevel, "ViewFactory::CreateViewForUrl - cant create view.");
}

void ViewFactory::AddViewCreator(ViewCreator* creator) {
	if (NULL != creator) {
		creators_.push_back(creator);
	}
}

} // namespace webdriver 

