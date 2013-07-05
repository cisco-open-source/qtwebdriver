#include <viewfactory.h>
#include "qwebviewext.h"
#include "base/memory/singleton.h"
#include <QtCore/QDebug>

ViewFactory* ViewFactory::instance = 0;
ViewFactory::ViewFactory()
{
}

ViewFactory* ViewFactory::GetInstance()
{
    if (instance == 0)
        instance = new ViewFactory;
    return instance;
}

QWebView* ViewFactory::create(const std::string &id)
{
    FactoryMap::iterator it = factory.find(id);
    if (it != factory.end())
    {
        qDebug()<<"[WD]:"<<"ViewFactory create:"<<QString(it->first.c_str());
        return it->second->create();
    }
    else
    {
        if (id == "QWebView")
        {
            // get first found QWebView
            FactoryMap::iterator it = factory.begin();
            for (it; it != factory.end(); ++it)
            {
                qDebug()<<"[WD]:"<<"ViewFactory create first found registered QWebView subclass:"<<QString(it->first.c_str());
                return it->second->create();
            }
        }
        qDebug()<<"[WD]:"<<"ViewFactory create default QWebViewExt";
        return new QWebViewExt;
    }
}
