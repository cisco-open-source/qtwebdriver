BUILD_DIR = ../../..
include($$BUILD_DIR/common.pri)

TARGET = WebDriver_extension_qt_base
TEMPLATE = lib
CONFIG += staticlib qt create_prl

include(extension_qt_base-src.pri)
