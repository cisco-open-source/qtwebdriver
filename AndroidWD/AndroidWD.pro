QT       += core gui network xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AndroidWD
TEMPLATE = app

OTHER_FILES += \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/layout/splash.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-pl/strings.xml \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/version.xml \
    android/AndroidManifest.xml

HEADERS += \
    ../src/Test/XPathElementFindingTest.h \
    ../src/Test/WindowTest.h \
    ../src/Test/VisibilityTest.h \
    ../src/Test/TypingTest.h \
    ../src/Test/TextHandlingTest.h \
    ../src/Test/StaleElementReferenceTest.h \
    ../src/Test/RestyledLabel.h \
    ../src/Test/FormHandlingTest.h \
    ../src/Test/FindingTest.h \
    ../src/Test/ElementSelectingTest.h \
    ../src/Test/ElementAttributeTest.h \
    ../src/Test/DragableWidget.h \
    ../src/Test/CoordinatesTest.h \
    ../src/Test/CommonQtTestHeaders.h \
    ../src/Test/ClickTest.h \
    ../src/Test/ClickScrollingTest.h \
    ../src/Test/BasicMouseInterfaceTest.h \
    ../src/Test/BasicKeyboardInterfaceTest.h

SOURCES += \
    ../src/Test/XPathElementFindingTest.cc \
    ../src/Test/WindowTest.cc \
    ../src/Test/VisibilityTest.cc \
    ../src/Test/TypingTest.cc \
    ../src/Test/TextHandlingTest.cc \
    ../src/Test/StaleElementReferenceTest.cc \
    ../src/Test/RestyledLabel.cc \
    ../src/Test/main.cc \
    ../src/Test/FormHandlingTest.cc \
    ../src/Test/FindingTest.cc \
    ../src/Test/ElementSelectingTest.cc \
    ../src/Test/ElementAttributeTest.cc \
    ../src/Test/DragableWidget.cc \
    ../src/Test/CoordinatesTest.cc \
    ../src/Test/ClickTest.cc \
    ../src/Test/ClickScrollingTest.cc \
    ../src/Test/BasicMouseInterfaceTest.cc \
    ../src/Test/BasicKeyboardInterfaceTest.cc

FORMS += \
    ../src/Test/ElementSelectingTest.ui \
    ../src/Test/ClickScrollingTest.ui


INCLUDEPATH += ../inc/ \


CONFIG( debug, debug|release ) {
LIBS += -L../bin/android/release_dbg/
} else {
LIBS += -L../bin/android/release/
}

LIBS += -lWebDriver_extension_qt_base \
    -lWebDriver_core \
    -lchromium_base \

QMAKE_CXXFLAGS += -D_GLIBCXX_PERMIT_BACKWARD_HASH

DEFINES +=  OS_ANDROID


