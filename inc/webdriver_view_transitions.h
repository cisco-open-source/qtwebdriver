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

/// base class for view transition action
class URLTransitionAction {
public:
    /// handle old view 
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
    ViewTransitionManager() {};
    ~ViewTransitionManager() {};

    static URLTransitionActionPtr urlTransitionAction_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_TRANSITIONS_H_
