#ifndef RESTYLEDLABEL_H
#define RESTYLEDLABEL_H

#include "CommonQtTestHeaders.h"

class RestyledLabel : public QLabel
{
    Q_OBJECT
public:
    RestyledLabel(const QString &text, QWidget *parent);
};

#endif // RESTYLEDLABEL_H
