#BUILD_DIR = ../..
#include($$BUILD_DIR/common.pri)

#TARGET = chromium_base
#TEMPLATE = lib
#CONFIG += staticlib create_prl

#INCLUDEPATH += $$PWD
#DEPENDPATH += $$PWD


#INC_DIR=$$PWD/../../inc
#SRC_DIR=$$PWD/..

include(base-src.pri)

linux* {
    include(base-src-linux.pri)
}

win32:{
    include(base-src-win.pri)
}

#macx {
#	include(base-src-mac.pri)
#}

#ios {
#	include(base-src-ios.pri)
#}

#android {
#	include(base-src-android.pri)
#}
