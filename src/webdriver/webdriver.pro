BUILD_DIR = ../..
include($$BUILD_DIR/common.pri)

TARGET = WebDriver_core
TEMPLATE = lib
CONFIG += staticlib create_prl

version.target = $$SRC_DIR/webdriver/webdriver_version.cc
version.depends = FORCE
version.commands = cd $$ROOT_DIR; python generate_wdversion.py;

PRE_TARGETDEPS += $$SRC_DIR/webdriver/webdriver_version.cc
QMAKE_EXTRA_TARGETS += version

INCLUDEPATH += $$SRC_DIR/third_party/mongoose

include(webdriver-src.pri)
