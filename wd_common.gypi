{
  'variables': {
    'QT5%': '0',
    'WD_CONFIG_QWIDGET_VIEW%': '0',
    'WD_CONFIG_QML_VIEW%': '0',
    'WD_BUILD_MONGOOSE%': '0',

    'QT_BIN_PATH%': '/usr/lib/qt4/bin',
    'QT_INC_PATH%': '/usr/include',
    'QT_LIB_PATH%': '/usr/lib',
    'MONGOOSE_INC_PATH%': 'src/third_party/mongoose',

    'conditions': [

      [ 'platform == "android"', {
        'ANDROID_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)/include',
        'ANDROID_LIB_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/include',
        'ANDROID_PLATFORM_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/platforms/android-9/<!(/bin/echo -n $ANDROID_ARCH)/usr/include',
      } ],
      
    ],
  },
}