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

QWidget* ViewFactory::create(const std::string & id)
{
    typename FactoryMap::iterator it = factory.find(id);
    if (it != factory.end())
        return it->second->create();
    qDebug()<<"[WD]:"<<"Browser class"<<QString(id.c_str())<<" is not registered. Create default one";
    return new QWebViewExt;
}
