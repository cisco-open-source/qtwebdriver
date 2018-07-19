
linux*{
    LIBS+= -ldl
}


INCLUDEPATH += $$PWD/../third_party/mongoose


include(webdriver-src.pri)
