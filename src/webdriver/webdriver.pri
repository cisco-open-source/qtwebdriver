#BUILD_DIR = ../..
#include($$BUILD_DIR/common.pri)

#TARGET = WebDriver_core
#EMPLATE = lib
#CONFIG += staticlib create_prl

#version.target = $$SRC_DIR/webdriver/webdriver_version.cc
#version.depends = FORCE
#version.commands = cd $$ROOT_DIR; python generate_wdversion.py;

#PRE_TARGETDEPS += $$SRC_DIR/webdriver/webdriver_version.cc
#QMAKE_EXTRA_TARGETS += version

INCLUDEPATH += $$PWD/../third_party/mongoose


#INC_DIR=$$PWD/../../inc
#SRC_DIR=$$PWD/..

include(webdriver-src.pri)
