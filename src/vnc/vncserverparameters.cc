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

#include "extension_qt/vncserverparameters.h"


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
