#ifndef Q_EVENT_FILTER_H
#define Q_EVENT_FILTER_H

#include <QtCore/QObject>
#include <QtCore/qcoreevent.h>

class QRepaintEventFilter : public QObject
{
    Q_OBJECT
signals:
    void repainted();
protected:
    virtual bool eventFilter(QObject *pobject, QEvent *pevent);

public:
    explicit QRepaintEventFilter(QObject *parent = 0);
    virtual ~QRepaintEventFilter();
};

class QCheckPagePaint : public QObject {
    Q_OBJECT
public:
    explicit QCheckPagePaint() :
                QObject(NULL) {is_painting = false;}
    bool isPainting() {return is_painting;}

//signals:
//    void painted();

public slots:
    void pagePainted();

private:
    bool is_painting;

};

#endif // Q_EVENT_FILTER_H
