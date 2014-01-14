QT       += core gui network widgets qml xml multimedia multimediawidgets quick

SOURCES += \
    ../../../src/Test/shutdown_command.cc \
    ../../../src/Test/main.cc \
    ../../../src/Test/XPathElementFindingTest.cc \
    ../../../src/Test/WindowTest.cc \
    ../../../src/Test/VisibilityTest.cc \
    ../../../src/Test/TypingTest.cc \
    ../../../src/Test/TextHandlingTest.cc \
    ../../../src/Test/StaleElementReferenceTest.cc \
    ../../../src/Test/RestyledLabel.cc \
    ../../../src/Test/FormHandlingTest.cc \
    ../../../src/Test/FindingTest.cc \
    ../../../src/Test/ElementSelectingTest.cc \
    ../../../src/Test/ElementAttributeTest.cc \
    ../../../src/Test/DragableWidget.cc \
    ../../../src/Test/CoordinatesTest.cc \
    ../../../src/Test/ClickTest.cc \
    ../../../src/Test/ClickScrollingTest.cc \
    ../../../src/Test/BasicMouseInterfaceTest.cc \
    ../../../src/Test/BasicKeyboardInterfaceTest.cc \
    ../../../src/Test/TouchTest.cc \
    ../../../src/Test/MenuTest.cc \
    #../../../src/Test/VideoTest.cc

HEADERS += \
    ../../../src/Test/shutdown_command.h \
    ../../../src/Test/XPathElementFindingTest.h \
    ../../../src/Test/WindowTest.h \
    ../../../src/Test/VisibilityTest.h \
    ../../../src/Test/TypingTest.h \
    ../../../src/Test/TextHandlingTest.h \
    ../../../src/Test/StaleElementReferenceTest.h \
    ../../../src/Test/RestyledLabel.h \
    ../../../src/Test/FormHandlingTest.h \
    ../../../src/Test/FindingTest.h \
    ../../../src/Test/ElementSelectingTest.h \
    ../../../src/Test/ElementAttributeTest.h \
    ../../../src/Test/DragableWidget.h \
    ../../../src/Test/CoordinatesTest.h \
    ../../../src/Test/CommonQtTestHeaders.h \
    ../../../src/Test/ClickTest.h \
    ../../../src/Test/ClickScrollingTest.h \
    ../../../src/Test/BasicMouseInterfaceTest.h \
    ../../../src/Test/BasicKeyboardInterfaceTest.h \
    ../../../src/Test/TouchTest.h \
    ../../../src/Test/TestVariables.h \
    ../../../src/Test/MenuTest.h \
    #../../../src/Test/VideoTest.h

INCLUDEPATH += ../../../inc

FORMS += \
    ../../../src/Test/ElementSelectingTest.ui \
    ../../../src/Test/ClickScrollingTest.ui


DESTDIR = build
OBJECTS_DIR = build
MOC_DIR = build

INCLUDEPATH += $$PWD/../../../build/Default-iphonesimulator
DEPENDPATH += $$PWD/../../../build/Default-iphonesimulator

LIBS += -L$$PWD/../../../build/Default-iphonesimulator/ -lchromium_base
PRE_TARGETDEPS += $$PWD/../../../build/Default-iphonesimulator/libchromium_base.a

LIBS += -L$$PWD/../../../build/Default-iphonesimulator/ -lWebDriver_core
PRE_TARGETDEPS += $$PWD/../../../build/Default-iphonesimulator/libWebDriver_core.a

LIBS += -L$$PWD/../../../build/Default-iphonesimulator/ -lWebDriver_extension_qt_base
PRE_TARGETDEPS += $$PWD/../../../build/Default-iphonesimulator/libWebDriver_extension_qt_base.a

LIBS += -L$$PWD/../../../build/Default-iphonesimulator/ -lWebDriver_extension_qt_quick
PRE_TARGETDEPS += $$PWD/../../../build/Default-iphonesimulator/libWebDriver_extension_qt_quick.a
