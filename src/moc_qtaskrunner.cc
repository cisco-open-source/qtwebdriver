/****************************************************************************
** Meta object code from reading C++ file 'qtaskrunner.h'
**
** Created: Tue Oct 30 10:14:28 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtaskrunner.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtaskrunner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTaskRunner[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QTaskRunner[] = {
    "QTaskRunner\0\0task\0runTask(base::Closure)\0"
};

void QTaskRunner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QTaskRunner *_t = static_cast<QTaskRunner *>(_o);
        switch (_id) {
        case 0: _t->runTask((*reinterpret_cast< const base::Closure(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QTaskRunner::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QTaskRunner::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QTaskRunner,
      qt_meta_data_QTaskRunner, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QTaskRunner::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QTaskRunner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QTaskRunner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTaskRunner))
        return static_cast<void*>(const_cast< QTaskRunner*>(this));
    return QObject::qt_metacast(_clname);
}

int QTaskRunner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
