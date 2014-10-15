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

<h1>View factory</h1>

Each type of view should have own factory, class derived from webdriver::ViewCreator.
Factory for single view type should be registered in singleton - webdriver::ViewFactory.
View can be created by two methods:
- by class name
- by url (in this case it creates any view that can handle this url)

\remark Please note that if two factories can create view by same classname(or url) - only one view will be created from first registered factory.

CreateViewByClassName() and CreateViewForUrl() return ViewHandles. But these
handles are not registered in session. User can register them manually or use
view enumerator. After enumerating action all view created but not registered will be
added to session's views map automatically.

Example of configuration:
\code
webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");

webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();
widgetCreator->RegisterViewClass<QWidget>("QWidget");
widgetCreator->RegisterViewClass<WindowTestWidget>("WindowTestWidget");

webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);
webdriver::ViewFactory::GetInstance()->AddViewCreator(widgetCreator);
\endcode

*/
#ifndef WEBDRIVER_VIEW_FACTORY_H_
#define WEBDRIVER_VIEW_FACTORY_H_

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "webdriver_view_id.h"
#include "webdriver_logging.h"
#include "webdriver_basic_types.h"

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
    /// @param[in] position - desired position
    /// @param[in] size - desired size
    /// @param[out] view created view.
    /// @return true if handled
    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className,
                                       const Point* position, const Size* size, ViewHandle** view) const = 0;

    /// creates new view that can handle specified url
    /// @param[in] logger
    /// @param[in] url url to handle
    /// @param[in] position - desired position
    /// @param[in] size - desired size
    /// @param[out] view created view. 
    /// @return true if handled
    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url,
                                  const Point* position, const Size* size, ViewHandle** view) const = 0;

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

    /// creates new view of specified class
    /// @param[in] logger
    /// @param[in] className requested class name
    /// @param[in] position desired window position
    /// @param[in] size desired window size
    /// @param[out] view created view.
    void CreateViewByClassName(const Logger& logger, const std::string& className,
                               const Point* position, const Size* size, ViewHandle** view) const;

    /// creates new view that can handle specified url with specified window sixe and position
    /// @param[in] logger
    /// @param[in] url url to handle
    /// @param[out] view created view.
    void CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const;

    /// creates new view that can handle specified url with specified window sixe and position
    /// @param[in] logger
    /// @param[in] url url to handle
    /// @param[in] position desired window position
    /// @param[in] size desired window size
    /// @param[out] view created view.
    void CreateViewForUrl(const Logger& logger, const std::string& url,
                          const Point* position, const Size* size, ViewHandle** view) const;

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
