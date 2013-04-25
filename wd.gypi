{
  'variables': {
	'QT5': '0', # change to '1' for enabling Qt5
	'WD_CONFIG_QWIDGET_VIEW': '1',
    'WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY': '0',
    'WD_CONFIG_QML_VIEW': '1',
    'WD_BUILD_MONGOOSE': '1',
    'WD_BUILD_MODP_B64': '0',
    'CISCO_QT_INC_PATH':   'WEBKITQT48_SRC/dist/<(platform)/<(mode)/include',
    'CISCO_QT_LIB_PATH':   'WEBKITQT48_SRC/dist/<(platform)/<(mode)/lib/',
    'CISCO_QT_BIN_PATH':   'WEBKITQT48_SRC/dist/<(platform)/<(mode)/bin/',
    'CISCO_MONGOOSE_LIB_PATH':   'MONGOOSE/dist/<(platform)/generic/<(mode)/lib',
    'CISCO_MONGOOSE_INC_PATH':    'MONGOOSE/dist/<(platform)/generic/<(mode)/h',
    'CISCO_MINIZIP_LIB_PATH':   'MINIZIP/dist/<(platform)/generic/<(mode)/lib',
    'CISCO_MINIZIP_INC_PATH':   'MINIZIP/dist/<(platform)/generic/<(mode)/h',
    'CISCO_ZLIB_LIB_PATH':   'ZLIB/dist/<(platform)/generic/<(mode)/lib',
    'CISCO_ZLIB_INC_PATH':   'ZLIB/dist/<(platform)/generic/<(mode)/h',
  },
}
