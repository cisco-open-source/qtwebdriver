#ifndef COORDINATESTEST_H
#define COORDINATESTEST_H

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#else
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#endif

class CoordinatesTestWidget : public QWidget
{
public:
    CoordinatesTestWidget();
    virtual ~CoordinatesTestWidget();

private:
    QScrollArea *scrollArea;
    QLabel *zeroSizeLabel;
    QLabel *transparentLabel;
    QLabel *hiddenLabel;
    QLabel *unvisibleLabel;
};

#endif // COORDINATESTEST_H
