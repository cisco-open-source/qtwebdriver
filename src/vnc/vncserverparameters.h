#ifndef VNCSERVERPARAMETERS_H
#define VNCSERVERPARAMETERS_H

#include <QtCore/QTypeInfo>

class ServerParameters
{
public:

    struct PIXEL_FORMAT
    {
        qint8 bitsPerPixel;
        qint8 depth;
        qint8 bigEndianFlag;
        qint8 trueColorFlag;
        qint16 redMax;
        qint16 greenMax;
        qint16 blueMax;
        qint8 redShift;
        qint8 greenShift;
        qint8 blueShift;
        qint8 padding[3];
    };

public:

    ServerParameters();
    ServerParameters(qint16 width, qint16 height, PIXEL_FORMAT* pixelFormat, QString* desktopName);
    ~ServerParameters();

    void setWidth(qint16 width);
    void setHeight(qint16 height);
    void setPixelFromat(PIXEL_FORMAT* pixels);
    void setName(QString* name);

    qint16 getWidth() const;
    qint16 getHeight() const;
    const PIXEL_FORMAT* getPixelFromat() const;
    const QString* getName() const;

private:

    qint16 _width;
    qint16 _height;
    PIXEL_FORMAT* _pixelsFormat;
    QString *_name;
};

#endif // VNCSERVERPARAMETERS_H
