#ifndef WEBDRIVER_VIEW_EXECUTOR_H
#define WEBDRIVER_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "webdriver_view_id.h"


namespace webdriver {

class Session;
class Error;
class Rect;


/// base class for custom view's executors
class ViewCmdExecutor {
public:
    explicit ViewCmdExecutor(Session* session, ViewId viewId);
    ~ViewCmdExecutor() {};

    virtual void GetTitle(std::string* title, Error **error) = 0;
    virtual void GetBounds(Rect *bounds, Error **error) = 0;
    virtual void SetBounds(const Rect& bounds, Error** error) = 0;
    virtual void Maximize(Error** error) = 0;

  

private:
    Session* session_;
    ViewId view_id_;

    DISALLOW_COPY_AND_ASSIGN(ViewCmdExecutor);
};

/// base class for custom cmd executor creators
class ViewCmdExecutorCreator {
public:
    ViewCmdExecutorCreator();
    ~ViewCmdExecutorCreator(){}

    /// custom method, creates executor for specified view
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @return new ViewCmdExecutor object, NULL - if cant handle
    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const = 0;

    /// check if view suppported
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @param[out] viewType returned type of view
    /// @return true - if there is executor for such view
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const = 0;
private:

    DISALLOW_COPY_AND_ASSIGN(ViewCmdExecutorCreator);
};

/// This class used for managing commands related to view
class ViewCmdExecutorFactory
{
public:
    /// Returns the singleton instance.
    static ViewCmdExecutorFactory* GetInstance();

    /// creates executor for specified view
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @return new executor, NULL - if cant create.
    ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;

    /// creates executor for current view in session
    /// @param session pointer to session
    /// @return new ViewCmdExecutor object, NULL - if cant handle
    ViewCmdExecutor* CreateExecutor(Session* session) const;

    /// check if view suppported
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @param[out] viewType returned type of view
    /// @return true - if there is executor for such view
    bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const;

    /// add new view's eexcutor creator
    /// @param creator pointer to custom creator. No need to delete object
    void AddViewCmdExecutorCreator(ViewCmdExecutorCreator* creator);

private:
    typedef ViewCmdExecutorCreator* ViewCmdExecutorCreatorPtr;
    typedef std::vector<ViewCmdExecutorCreatorPtr> CreatorsList;
    CreatorsList creators_;

    ViewCmdExecutorFactory();
    ~ViewCmdExecutorFactory(){}
    static ViewCmdExecutorFactory* instance;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_EXECUTOR_H
