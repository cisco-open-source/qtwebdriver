/****************************************************************************
** Meta object code from reading C++ file 'webdriver_automation.h'
**
** Created: Thu Nov 15 17:30:49 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "chrome/test/webdriver/webdriver_automation.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'webdriver_automation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_webdriver__JSNotifier[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   35,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_webdriver__JSNotifier[] = {
    "webdriver::JSNotifier\0\0completed()\0"
    "result\0setResult(QVariant)\0"
};

void webdriver::JSNotifier::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        JSNotifier *_t = static_cast<JSNotifier *>(_o);
        switch (_id) {
        case 0: _t->completed(); break;
        case 1: _t->setResult((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData webdriver::JSNotifier::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject webdriver::JSNotifier::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_webdriver__JSNotifier,
      qt_meta_data_webdriver__JSNotifier, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &webdriver::JSNotifier::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *webdriver::JSNotifier::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *webdriver::JSNotifier::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_webdriver__JSNotifier))
        return static_cast<void*>(const_cast< JSNotifier*>(this));
    return QObject::qt_metacast(_clname);
}

int webdriver::JSNotifier::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void webdriver::JSNotifier::completed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_webdriver__Automation[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      41,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_webdriver__Automation[] = {
    "webdriver::Automation\0\0pageLoadStarted()\0"
    "pageLoadFinished()\0"
};

void webdriver::Automation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Automation *_t = static_cast<Automation *>(_o);
        switch (_id) {
        case 0: _t->pageLoadStarted(); break;
        case 1: _t->pageLoadFinished(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData webdriver::Automation::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject webdriver::Automation::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_webdriver__Automation,
      qt_meta_data_webdriver__Automation, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &webdriver::Automation::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *webdriver::Automation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *webdriver::Automation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_webdriver__Automation))
        return static_cast<void*>(const_cast< Automation*>(this));
    return QObject::qt_metacast(_clname);
}

int webdriver::Automation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
