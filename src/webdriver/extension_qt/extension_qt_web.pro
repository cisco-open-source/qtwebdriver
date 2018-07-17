BUILD_DIR = ../../..
include($$BUILD_DIR/common.pri)

TARGET = WebDriver_extension_qt_web
TEMPLATE = lib
CONFIG += staticlib create_prl

include(extension_qt_web-src.pri)
