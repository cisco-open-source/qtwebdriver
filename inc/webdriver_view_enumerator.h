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

/*! \page page_views Views management and actions

<h1>View enumerator</h1>

Session contains map of ViewId to ViewHandle. View enumerator can update 
this map. It adds new views and removes closed.
For each view type there should be separated view enumerator - class derived 
from webdriver::AbstractViewEnumeratorImpl. Custom AbstractViewEnumeratorImpl
has only one virtual method EnumerateViews() that returns all found view of specific types.

Example of usage:
\code
webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());
webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WidgetViewEnumeratorImpl());
\endcode

*/
#ifndef WEBDRIVER_VIEW_ENUMERATOR_H_
#define WEBDRIVER_VIEW_ENUMERATOR_H_

#include <string>
#include <vector>
#include <set>

#include "base/basictypes.h"
#include "webdriver_view_id.h"


namespace webdriver {

class Session;   

/// base class for view enumerator
class AbstractViewEnumeratorImpl {
public:
    /// Get list of views in session
    /// @param session session to enumerate views
    /// @param views set with views to update
    virtual void EnumerateViews(Session* session, std::set<ViewId>* views) const = 0;
};

typedef AbstractViewEnumeratorImpl* ViewEnumeratorImplPtr;

class ViewEnumerator {
public:

    /// Updates session views list
    /// @param session session to update
    /// @param views returned list of ViewIds
    static void EnumerateViews(Session* session, std::vector<ViewId>* views);

    /// Add enumerator for custom view
    /// @param enumeratorImpl pointer to custom enumerator. No need to delete object, class will handle.
    static void AddViewEnumeratorImpl(AbstractViewEnumeratorImpl* enumeratorImpl);

private:
    ViewEnumerator() {};
    ~ViewEnumerator() {};

    static std::vector<ViewEnumeratorImplPtr> enumerators_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_ENUMERATOR_H_
