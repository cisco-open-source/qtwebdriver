#ifndef COORDINATESTEST_H
#define COORDINATESTEST_H

#include "CommonQtTestHeaders.h"

class CoordinatesTestWidget : public QWidget
{
    Q_OBJECT
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
