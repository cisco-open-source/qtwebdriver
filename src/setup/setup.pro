BUILD_DIR = ../..
include($$BUILD_DIR/common.pri)

TARGET = WebDriver_setup
TEMPLATE = lib
CONFIG += qt plugin link_prl
QT += widgets network webkit webkitwidgets

macx: LIBS += -framework Foundation -framework CoreFoundation -framework Security -framework Carbon -framework IOKit

STATIC_LIBS = -lchromium_base -lWebDriver_core -lWebDriver_extension_qt_base -lWebDriver_extension_qt_web

LIBS += -L$$BUILD_DIR/bin
linux* {
	LIBS += -Wl,-whole-archive $$STATIC_LIBS -Wl,-no-whole-archive
} else {
	LIBS += $$STATIC_LIBS
}

static_deps.target = force_rebuild
static_deps.commands = @echo forcing the linking of $${TARGET}

QMAKE_EXTRA_TARGETS += static_deps
PRE_TARGETDEPS += force_rebuild

include(setup-src.pri)