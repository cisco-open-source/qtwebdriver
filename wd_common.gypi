{
  'variables': {
  	'QT5%': '0',
  	'WD_CONFIG_QWIDGET_VIEW%': '0',
    'WD_CONFIG_QML_VIEW%': '0',
    'WD_BUILD_MONGOOSE%': '0',
    'WD_CONFIG_XPATH%': '1',

    'conditions': [

  		# WORKAROUND: this is for compatibility with CISCO-NDS build system
  		[ 'platform != "desktop"', {
		    	'QT_BIN_PATH': '<(CISCO_QT_BIN_PATH)',
        	'QT_INC_PATH': '<(CISCO_QT_INC_PATH)',
        	'QT_LIB_PATH': '<(CISCO_QT_LIB_PATH)',
        	'MONGOOSE_INC_PATH': '<(CISCO_MONGOOSE_INC_PATH)',
    	} ],

    	[ 'platform == "desktop"', {
        	'QT_BIN_PATH': '<(DESKTOP_QT_BIN_PATH)',
        	'QT_INC_PATH': '<(DESKTOP_QT_INC_PATH)',
        	'QT_LIB_PATH': '<(DESKTOP_QT_LIB_PATH)',
        	'MONGOOSE_INC_PATH': 'src/third_party/mongoose'
    	} ],
      [ 'platform == "android"', {
          'QT_BIN_PATH': '<(DESKTOP_QT_BIN_PATH)',
          'QT_INC_PATH': '<(DESKTOP_QT_INC_PATH)',
          'QT_LIB_PATH': '<(DESKTOP_QT_LIB_PATH)',
          'MONGOOSE_INC_PATH': 'src/third_party/mongoose',
          'ANDROID_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)/include',
          'ANDROID_LIB_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/include',
          'ANDROID_PLATFORM_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/platforms/android-9/<!(/bin/echo -n $ANDROID_ARCH)/usr/include',
      } ],
    ],
  },
}