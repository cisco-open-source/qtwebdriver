{
   # do not require cygwin
  'msvs_cygwin_shell': 0,

  'msvs_configuration_attributes': {
     'CharacterSet': '1'
  },

  'cflags': [
    '-fPIC',
    '-Wall',
    '-W',
    '-Wno-unused-parameter',
  ],

  'defines': [
    'QT_NO_DEBUG',
    'QT_GUI_LIB',
    'QT_CORE_LIB',
    'QT_SHARED',
    'QT_NO_OPENGL',
  ],

  'conditions': [ 
    [ 'mode == "debug"', {
      'cflags': [
        '-O0',
        '-g',
      ],
    } ],

    [ 'mode == "release"', {
      'cflags': [
        '-O3',
      ],
      'defines': [
        'NDEBUG',
      ],
    } ],

    [ 'mode == "release_dbg"', {
      'cflags': [
        '-O3',
        '-g',
      ],
    } ],

    [ 'OS == "linux"', {
       'actions': [ {
         'action_name': 'input_dir',
         'inputs':      [],
         'outputs':     ['<(INTERMEDIATE_DIR)'],
         'action':      ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
       } ],
       'defines': [ 
         '__STDC_FORMAT_MACROS',
         'OS_POSIX',
       ],
    } ],

    [ 'OS == "android"', {
       'actions': [ {
         'action_name': 'input_dir',
         'inputs':      [],
         'outputs':     ['<(INTERMEDIATE_DIR)'],
         'action':      ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
       } ],
       'defines': [ 
         'ANDROID',
         '_GLIBCXX_PERMIT_BACKWARD_HASH',
       ],
       'include_dirs': [
          '<(ANDROID_INC)',
          '<(ANDROID_LIB_INC)',
          '<(ANDROID_PLATFORM_INC)',
          '<(QT_INC_PATH)/QtGui/<!(/bin/echo -n $QT_VERSION)/QtGui'
        ],
        'ldflags': [
          '--sysroot=<!(/bin/echo -n $ANDROID_NDK_ROOT)/platforms/android-9/arch-<!(/bin/echo -n $ANDROID_ARCH)',
        ],
    } ],

    [ 'OS == "win"', {
      'defines': [
         '_WIN32',
         'WIN32'
         'OS_WIN',
         'NOMINMAX',
         '_UNICODE',
         '_WINSOCKAPI_',
      ],

      'msvs_settings': {
        'VCCLCompilerTool': {
          'TreatWChar_tAsBuiltInType': 'false',
        },
      },

    } ],

    [ 'OS == "mac"', {
      'xcode_settings': {
        'SDKROOT': 'macosx10.8',
        'FRAMEWORK_SEARCH_PATHS': '<(QT_LIB_PATH)',
      },
    } ],

    [ 'OS == "mac" or OS == "ios"', {
      'actions': [ {
        'action_name': 'create_input_dir',
        'inputs':      [],
        'outputs':     [],
        'action':      ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
       } ],

      'defines': [
         'OS_POSIX',
      ],

      'xcode_settings': {
        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
      },
    } ],

    [ 'OS=="ios"', {
      'xcode_settings': {
        'SDKROOT': 'iphonesimulator',
        'CLANG_WARN_CXX0X_EXTENSIONS': 'NO',
      },
    }],

    [ '<(WD_CONFIG_PLAYER) == 1', {
     'defines': [ 'WD_ENABLE_PLAYER=1' ],
    }],

    [ '<(WD_CONFIG_ONE_KEYRELEASE) == 1', {
     'defines': [ 'WD_ENABLE_ONE_KEYRELEASE=1' ],
    }]
  ],
}
