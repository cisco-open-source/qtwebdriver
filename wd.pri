
#QMAKE_PROJECT_DEPTH = 0

INCLUDEPATH += $$PWD/inc
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/Test

DEFINES += QT_NO_SAMPLES

system(cd $$PWD && python generate_wdversion.py)

include(common.pri)

INC_DIR=$$PWD/inc
SRC_DIR=$$PWD/src

include(src/base/base.pri)
include(src/webdriver/webdriver.pri)
include(src/webdriver/extension_qt/extension_qt_base.pri)
include(src/webdriver/extension_qt/extension_qt_quick.pri)

win32:LIBS += Advapi32.lib Ws2_32.lib Ole32.lib

