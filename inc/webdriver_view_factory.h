#ifndef WEBDRIVER_VIEW_FACTORY_H_
#define WEBDRIVER_VIEW_FACTORY_H_

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "webdriver_view_id.h"
#include "webdriver_logging.h"

namespace webdriver {

typedef void* (*CreateViewMethod)(void);

template <class C>
void* createView(void) { return static_cast<void*>(new C());}

/// base class for custom view's creators
class ViewCreator {
public:
    ViewCreator();
    virtual ~ViewCreator(){}

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
    /// @param[in] logger
    /// @param[in] className requested class name
    /// @param[out] view created view.
    /// @return true if handled
    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const = 0;

    /// creates new view that can handle specified url
    /// @param[in] logger
    /// @param[in] url url to handle
    /// @param[out] view created view. 
    /// @return true if handled
    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const = 0;    

protected:
    typedef std::map<std::string, CreateViewMethod> FactoryMap;
    FactoryMap factory;
private:
    DISALLOW_COPY_AND_ASSIGN(ViewCreator);
};

/// This class used for creating new views
class ViewFactory
{
public:
    /// Returns the singleton instance.
    static ViewFactory* GetInstance();

    /// creates new view of specified class
    /// @param[in] logger
    /// @param[in] className requested class name
    /// @param[out] view created view.
    void CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const;

    /// creates new view that can handle specified url
    /// @param[in] logger
    /// @param[in] url url to handle
    /// @param[out] view created view.
    void CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const;


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
