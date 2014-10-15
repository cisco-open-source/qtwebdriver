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

#include "webdriver_view_executor.h"

#include "webdriver_session.h"
namespace webdriver {

ViewCmdExecutor::ViewCmdExecutor(Session* session, ViewId viewId)
	: session_(session),
      view_id_(viewId) {}

ViewCmdExecutor::~ViewCmdExecutor() {}

ViewCmdExecutorCreator::ViewCmdExecutorCreator() {}

ViewCmdExecutorFactory* ViewCmdExecutorFactory::instance = NULL;

ViewCmdExecutorFactory::ViewCmdExecutorFactory() {}

ViewCmdExecutorFactory* ViewCmdExecutorFactory::GetInstance() {
	if (NULL == instance) {
		instance = new ViewCmdExecutorFactory();
	}
	return instance;
}

void ViewCmdExecutorFactory::AddViewCmdExecutorCreator(ViewCmdExecutorCreator* creator) {
	if (NULL != creator) {
		creators_.push_back(creator);
        typeslist_.push_back(creator->GetViewTypeName());
	}
}

const ViewCmdExecutorFactory::SupportedViewTypesList& ViewCmdExecutorFactory::getSupportedViewTypesList() const {
    return typeslist_;
}

ViewCmdExecutor* ViewCmdExecutorFactory::CreateExecutor(Session* session) const {
	return CreateExecutor(session, session->current_view());
}

ViewCmdExecutor* ViewCmdExecutorFactory::CreateExecutor(Session* session, ViewId viewId) const {
	CreatorsList::const_iterator creator;
	ViewCmdExecutor* executor = NULL;

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		executor = (*creator)->CreateExecutor(session, viewId);
		if (NULL != executor) {
			// succed, return
			return executor;
		}
	}

	session->logger().Log(kSevereLogLevel, "ViewFactory::CreateExecutor - cant create executor.");

	return NULL;
}

bool ViewCmdExecutorFactory::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
	CreatorsList::const_iterator creator;

	for (creator = creators_.begin(); creator < creators_.end(); ++creator)	{
		if ((*creator)->CanHandleView(session, viewId, viewType)) {
			// succed, return
			return true;
		}
	}

	session->logger().Log(kSevereLogLevel, "ViewFactory::CanHandleView - no executor found for view.");

	return false;
}

} // namespace webdriver
