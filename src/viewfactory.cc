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

QWidget* ViewFactory::create(const std::string &id)
{
    FactoryMap::iterator it = factory.find(id);
    if (it != factory.end())
    {
        qDebug()<<"[WD]:"<<"ViewFactory create:"<<QString(it->first.c_str());
        return it->second->create();
    }
    else
    {
        // get first found QWebView; only for QWebView, for NativeWD need additional implementation
        FactoryMap::iterator it = factory.begin();
        for (it; it != factory.end(); ++it)
        {
            QWidget* retWidget;

            retWidget = it->second->create();
            QWebView* retView = qobject_cast<QWebView*> (retWidget);
            if (retView)
            {
                qDebug()<<"[WD]:"<<"ViewFactory create first found registered QWebView subclass:"<<QString(it->first.c_str());
                return retView;
            }
            else
                delete retWidget;
        }
        qDebug()<<"[WD]:"<<"ViewFactory create default QWebViewExt";
        return new QWebViewExt;
    }
}
