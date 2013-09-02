QT       += core gui network

SOURCES += \
    ../../../src/Test/shutdown_command.cc \
    ../../../src/Test/main.cc \

HEADERS += \
    ../../../src/Test/shutdown_command.h \

INCLUDEPATH += ../../../inc
#DEFINES +=

ios: include($$[QT_HOST_DATA/get]/mkspecs/common/ios/qiosapp.conf)
