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

#include "webdriver_view_enumerator.h"

#include "webdriver_session.h"
#include "base/stringprintf.h"


namespace webdriver {

std::vector<ViewEnumeratorImplPtr> ViewEnumerator::enumerators_;

void ViewEnumerator::EnumerateViews(Session* session, std::vector<ViewId>* views) {
	std::vector<ViewEnumeratorImplPtr>::const_iterator impl;
	std::set<ViewId> views_set;

	for (impl = enumerators_.begin(); impl < enumerators_.end(); ++impl) {
		(*impl)->EnumerateViews(session, &views_set);
	}

	std::copy(views_set.begin(), views_set.end(), std::back_inserter(*views));

	// update session to remove dangling sessionIds
	session->logger().Log(kFineLogLevel, base::StringPrintf("EnumerateViews found %d views.", (int)views_set.size()));
	session->UpdateViews(views_set);
}

void ViewEnumerator::AddViewEnumeratorImpl(AbstractViewEnumeratorImpl* enumeratorImpl) {
	if (NULL != enumeratorImpl) {
		enumerators_.push_back(enumeratorImpl);
	}
}

} // namespace webdriver