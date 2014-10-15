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

/*! \file vncserverparameters.h
*/

#ifndef VNCSERVERPARAMETERS_H
#define VNCSERVERPARAMETERS_H

#include <QtCore/QTypeInfo>
#include <QtCore/QString>

/**
  It is container for server parameters.
  Client recieve them in server response.
*/
class ServerParameters
{
public:

    /**
      Struct holds information about natural pixel format of VNC server
    */
    struct PIXEL_FORMAT
    {
        qint8 bitsPerPixel;     //!< number of bits used for each pixel value on the wire
        qint8 depth;            //!< number of useful bits in the pixel value
        qint8 bigEndianFlag;    //!< is non-zero (true) if multi-byte pixels are interpreted as big endian
        qint8 trueColorFlag;    /*!< is non-zero (true) then the last six items specify how to extract the
                                     red, green and blue intensities from the pixel value */
        qint16 redMax;          //!< the maximum red value
        qint16 greenMax;        //!< the maximum green value
        qint16 blueMax;         //!< the maximum blue value
        qint8 redShift;         /*!< the number of shifts needed to get the red value in a
                                     pixel to the least signiﬁcant bit */
        qint8 greenShift;       /*!< the number of shifts needed to get the green value in a
                                     pixel to the least signiﬁcant bit */
        qint8 blueShift;        /*!< the number of shifts needed to get the blue value in a
                                     pixel to the least signiﬁcant bit */
        qint8 padding[3];       //!< padding
    };

public:

    /**
      Default constructor
     */
    ServerParameters();

    /**
      Constructor
      @param width - monitor resolution width
      @param height - monitor resolution height
      @param pixelFormat - pointer to struct with pixel format information
      @param desktopName - pointer to string with remote desktop name
     */
    ServerParameters(qint16 width, qint16 height, PIXEL_FORMAT* pixelFormat, QString* desktopName);

    /**
      Destructor
     */
    ~ServerParameters();

    /**
      Width parameter setter
      @param width - display horizontal resolution
     */
    void setWidth(qint16 width);

    /**
      Height parameter setter
      @param height - display vertical resolution
     */
    void setHeight(qint16 height);

    /**
      Pixel format setter
      @param pixels - pointer to struct with pixels format
     */
    void setPixelFromat(PIXEL_FORMAT* pixels);

    /**
      Desktop name setter
      @param pixels - pointer to struct with pixels format
     */
    void setName(QString* name);

    /**
      Display resolution width getter
      @return horizontal display resolution
     */
    qint16 getWidth() const;

    /**
      Display resolution height getter
      @return vertical display resolution
     */
    qint16 getHeight() const;

    /**
      Display resolution height getter
      @return vertical display resolution
     */
    const PIXEL_FORMAT* getPixelFromat() const;

    /**
      Remote desktop name getter
      @return vertical display resolution
     */
    const QString* getName() const;

private:

    qint16 _width;
    qint16 _height;
    PIXEL_FORMAT* _pixelsFormat;
    QString *_name;
};

#endif // VNCSERVERPARAMETERS_H
