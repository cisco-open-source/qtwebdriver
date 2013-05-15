#ifndef VIEWFACTORY_H
#define VIEWFACTORY_H

#include <map>

#include <QtCore/QGlobalStatic>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

class AbstractViewCreator
{
public:
    virtual QWebView * create() const = 0;
};

template <class C>
class ViewCreator: public AbstractViewCreator
{
public:
    virtual QWebView * create() const { return new C(); }
};

class ViewFactory
{
public:
    // Returns the singleton instance.
    static ViewFactory* GetInstance();

    template <class C>
    void add(const std::string & id)
    {
        FactoryMap::iterator it = factory.find(id);
        if (it == factory.end())
            factory[id] = new ViewCreator<C>();
    }

    QWebView * create(const std::string &id);


protected:
    typedef std::map<std::string, AbstractViewCreator*> FactoryMap;
    FactoryMap factory;

private:
    ViewFactory();
    ~ViewFactory(){}
    static ViewFactory* instance;
};







#endif // VIEWFACTORY_H
