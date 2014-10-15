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
    CreateViewByClassName(logger, className, NULL, NULL, view);
}

void ViewFactory::CreateViewByClassName(const Logger& logger, const std::string& className,
                                        const Point* position, const Size* size, ViewHandle** view) const {
	CreatorsList::const_iterator creator;

	logger.Log(kInfoLogLevel, "ViewFactory::CreateViewByClassName - " + className);

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
        if ((*creator)->CreateViewByClassName(logger, className, position, size, view)) {
			// succed, return
			return;
		}
	}

    logger.Log(kSevereLogLevel, "ViewFactory::CreateViewByClassName - cant create view.");
}

void ViewFactory::CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const {
    CreateViewForUrl(logger, url, NULL, NULL, view);
}

void ViewFactory::CreateViewForUrl(const Logger& logger, const std::string& url,
                                   const Point* position, const Size* size, ViewHandle** view) const {

    CreatorsList::const_iterator creator;

    for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
        if ((*creator)->CreateViewForUrl(logger, url, position, size, view)) {
            // succed, return
            return;
        }
    }
}

void ViewFactory::AddViewCreator(ViewCreator* creator) {
	if (NULL != creator) {
		creators_.push_back(creator);
	}
}

} // namespace webdriver 

