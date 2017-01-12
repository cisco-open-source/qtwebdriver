{
  'variables': {
    'QT5%': '0',
    'WD_CONFIG_QWIDGET_BASE%': '1',
    'WD_CONFIG_WEBKIT%': '1',
    'WD_CONFIG_QUICK%': '1',
    'WD_BUILD_MONGOOSE%': '1',
    'WD_CONFIG_PLAYER%': '1',
    'WD_CONFIG_ONE_KEYRELEASE%': '0',

    'QT_BIN_PATH%': '/usr/lib/qt4/bin',
    'QT_INC_PATH%': '/usr/include',
    'QT_LIB_PATH%': '/usr/lib',
    'MONGOOSE_INC_PATH%': 'src/third_party/mongoose',

    'conditions': [

      [ 'OS == "android"', {
        'ANDROID_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)/include',
        'ANDROID_LIB_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/include',
        'ANDROID_PLATFORM_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/platforms/android-9/arch-<!(/bin/echo -n $ANDROID_ARCH)/usr/include',
        'ANDROID_LIB': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)',
      } ],
      
    ],
  },
}
