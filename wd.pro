TEMPLATE = subdirs

SUBDIRS = base webdriver extension_qt_base extension_qt_web setup

base.subdir = src/base
webdriver.subdir = src/webdriver
setup.subdir = src/setup
extension_qt_base.file = src/webdriver/extension_qt/extension_qt_base.pro
extension_qt_web.file = src/webdriver/extension_qt/extension_qt_web.pro

setup.depends = base webdriver extension_qt_base extension_qt_web
