/****************************************************************************
** Meta object code from reading C++ file 'renderloop.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../softRenderer/renderloop.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'renderloop.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RenderLoop_t {
    QByteArrayData data[8];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RenderLoop_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RenderLoop_t qt_meta_stringdata_RenderLoop = {
    {
QT_MOC_LITERAL(0, 0, 10), // "RenderLoop"
QT_MOC_LITERAL(1, 11, 8), // "frameOut"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 14), // "unsigned char*"
QT_MOC_LITERAL(4, 36, 5), // "image"
QT_MOC_LITERAL(5, 42, 14), // "deltaFrameTime"
QT_MOC_LITERAL(6, 57, 3), // "fps"
QT_MOC_LITERAL(7, 61, 4) // "loop"

    },
    "RenderLoop\0frameOut\0\0unsigned char*\0"
    "image\0deltaFrameTime\0fps\0loop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RenderLoop[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   31,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Double, QMetaType::Int,    4,    5,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void RenderLoop::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RenderLoop *_t = static_cast<RenderLoop *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->frameOut((*reinterpret_cast< unsigned char*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->loop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RenderLoop::*_t)(unsigned char * , double , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RenderLoop::frameOut)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject RenderLoop::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RenderLoop.data,
      qt_meta_data_RenderLoop,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RenderLoop::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RenderLoop::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RenderLoop.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RenderLoop::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void RenderLoop::frameOut(unsigned char * _t1, double _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
