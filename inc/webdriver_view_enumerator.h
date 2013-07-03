/*! \page page_views Views management and actions

<h1>View enumerator</h1>

// TODO: 

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
    /// @param views returned list of ViewIds
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
