#include "vncserverparameters.h"


ServerParameters::ServerParameters()
    : _width(0),
      _height(0),
      _pixelsFormat(NULL),
      _name(NULL)
{
}

ServerParameters::ServerParameters(qint16 width,
      qint16 height, PIXEL_FORMAT *pixelFormat, QString *desktopName)
    : _width(width),
      _height(height),
      _pixelsFormat(pixelFormat),
      _name(desktopName)
{
}

ServerParameters::~ServerParameters()
{
    delete _pixelsFormat;
    delete _name;
}

void ServerParameters::setHeight(qint16 height)
{
    _height = height;
}

void ServerParameters::setWidth(qint16 width)
{
    _width = width;
}

void ServerParameters::setPixelFromat(PIXEL_FORMAT *pixels)
{
    _pixelsFormat = pixels;
}

void ServerParameters::setName(QString *name)
{
    _name = name;
}

qint16 ServerParameters::getHeight() const
{
    return _height;
}

qint16 ServerParameters::getWidth() const
{
    return _width;
}

const ServerParameters::PIXEL_FORMAT* ServerParameters::getPixelFromat() const
{
    return _pixelsFormat;
}

const QString* ServerParameters::getName() const
{
    return _name;
}
