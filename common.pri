#INCLUDEPATH = $$SRC_DIR $$INC_DIR

#DESTDIR = $${BUILD_DIR}/bin
#CONFIG += c++11

#CONFIG(release) {
#	DEFINES += NDEBUG
#}

!win* {
	QMAKE_CFLAGS += -Wall -W -Wno-unused-parameter -fPIC
	QMAKE_CXXFLAGS += -Wall -W -Wno-unused-parameter -fPIC
}

clang {
	QMAKE_CFLAGS += -Wno-unknown-warning-option
	QMAKE_CXXFLAGS += -Wno-unknown-warning-option
}

macx {
	QMAKE_MAC_SDK = macosx
}

linux* {
	DEFINES += __STDC_FORMAT_MACROS OS_POSIX
}

win* {
	DEFINES += _WIN32 OS_WIN NOMINMAX _CRT_RAND_S WIN32 _UNICODE _WINSOCKAPI_
}

android {
	DEFINES += ANDROID _GLIBCXX_PERMIT_BACKWARD_HASH
}

macx | ios {
	DEFINES += OS_POSIX
}
