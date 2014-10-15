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

#ifndef WEBDRIVER_VIEW_TRANSITIONS_H_
#define WEBDRIVER_VIEW_TRANSITIONS_H_

#include <string>
#include <vector>
#include <set>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "webdriver_view_id.h"


namespace webdriver {

class Session;   

/// Base class for view transition action that can be applied to old view,
/// in case of URLCommand that creates new view for other content. 
class URLTransitionAction {
public:
    virtual ~URLTransitionAction() {}

    /// Behavior of handling old view.
    /// @param session session to work within
    /// @param viewId viewId not able to handle requested URL
    virtual void HandleOldView(Session* session, const ViewId& viewId) const = 0;
};

/// transition action - close old view
class URLTransitionAction_CloseOldView : public URLTransitionAction {
public:
    virtual void HandleOldView(Session* session, const ViewId& viewId) const;
};

typedef scoped_ptr<URLTransitionAction> URLTransitionActionPtr;

/// Manager that stores custom view transition action.
/// Registered custom action will be used in URLCommand processing.
class ViewTransitionManager {
public:

    /// handle old view 
    /// @param session session to work within
    /// @param viewId viewId not able to handle requested URL
    static void HandleOldView(Session* session, const ViewId& viewId);

    /// set url transition action
    /// @param transitionAction pointer to custom action. Manager takes ownership on passed object.
    static void SetURLTransitionAction(URLTransitionAction* transitionAction);

private:
    ViewTransitionManager() {}
    ~ViewTransitionManager() {}

    static URLTransitionActionPtr urlTransitionAction_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_TRANSITIONS_H_
