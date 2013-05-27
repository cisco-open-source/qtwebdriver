#ifndef WEBDRIVER_VIEW_FACTORY_H_
#define WEBDRIVER_VIEW_FACTORY_H_

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "webdriver_view_id.h"


namespace webdriver {

class Session;
/*
namespace internal {
class AbstractViewCreator
{
public:
    virtual ViewHandle create() const = 0;
};

template <class C>
class ViewCreator: public AbstractViewCreator
{
public:
    virtual ViewHandle create() const { return static_cast<ViewHandle>(new C()); }
};
*/

/// base class for custom view's creators
class ViewCreator
{
private:
    typedef ViewHandle (ViewCreator::*CreateViewMethod)(void);
public:
    ViewCreator();
    ~ViewCreator(){}

    /// register class for specific class name
    /// @tparam <C> class for view
    /// @param id class name
    template <class C>
    void RegisterViewClass(const std::string & id)
    {
        FactoryMap::iterator it = factory.find(id);
        if (it == factory.end())
            factory[id] = &createView<C>;
    }

    /// creates new view of specified class
    /// @param[in] session pointer to session
    /// @param[in] className requested class name
    /// @param[out] handle returned handle, INVALID_HANDLE - if view was not created.
    /// @return true if handled
    virtual bool CreateViewByClassName(Session* session, const std::string& className, ViewHandle* handle) const = 0;

    /// creates new view that can handle specified url
    /// @param[in] session pointer to session
    /// @param[in] url url to handle
    /// @param[out] handle returned handle, INVALID_HANDLE - if view was not created.
    /// @return true if handled
    virtual bool CreateViewForUrl(Session* session, const std::string& url, ViewHandle* handle) const = 0;    

private:
    template <class C>
    void createView(void) const { return static_cast<ViewHandle>(new C());}

    typedef std::map<std::string, CreateViewMethod> FactoryMap;
    FactoryMap factory;

    DISALLOW_COPY_AND_ASSIGN(ViewCreator);
};

/// This class used for creating new views
class ViewFactory
{
public:
    /// Returns the singleton instance.
    static ViewFactory* GetInstance();

    /// creates new view of specified class
    /// @param[in] session pointer to session
    /// @param[in] className requested class name
    /// @param[out] handle returned handle, INVALID_HANDLE - if view was not created.
    void CreateViewByClassName(Session* session, const std::string& className, ViewHandle* handle) const;

    /// creates new view that can handle specified url
    /// @param[in] session pointer to session
    /// @param[in] url url to handle
    /// @param[out] handle returned handle, INVALID_HANDLE - if view was not created.
    void CreateViewForUrl(Session* session, const std::string& url, ViewHandle* handle) const;    


    /// add new view's creator
    /// @param creator pointer to custom creator. No need to delete object
    void AddViewCreator(ViewCreator* creator);

private:
    typedef ViewCreator* ViewCreatorPtr;
    typedef std::vector<ViewCreatorPtr> CreatorsList;
    CreatorsList creators_;

    ViewFactory();
    ~ViewFactory(){}
    static ViewFactory* instance;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_FACTORY_H_
