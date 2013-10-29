{
  'includes': [
    '../../../wd.gypi',
    '../../../wd_common.gypi',
  ],

  'target_defaults': {
    'includes': [
      '../../../wd_build_options.gypi',
      '../../../wd_qt_tools.gypi',
    ],

    'include_dirs': [
      '../include',
      'mimetypes',
      'io',
      '<(QT_INC_PATH)',
      '<(INTERMEDIATE_DIR)',
    ],

  },

  'targets': [
    {
      'target_name': 'mimetypes-qt4',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        'io/qstandardpaths.cpp',
        'mimetypes/qmimedatabase.cpp',
        'mimetypes/qmimetype.cpp',
        'mimetypes/qmimemagicrulematcher.cpp',
        'mimetypes/qmimetypeparser.cpp',
        'mimetypes/qmimemagicrule.cpp',
        'mimetypes/qmimeglobpattern.cpp',
        'mimetypes/qmimeprovider.cpp',
        'mimetypes/mimetypes.qrc',
        '<(INTERMEDIATE_DIR)/rcc_mimetypes.cc',
      ],

      'conditions': [
      
        ['OS=="linux"', {
          'sources': [ 'io/qstandardpaths_unix.cpp' ],
        } ],

        [ 'OS == "win"', {
          'sources': [ 'io/qstandardpaths_win.cpp', 'io/qsystemlibrary.cpp' ],
        } ],

        [ 'OS == "mac" or OS=="ios"', {
          'sources': [ 'io/qstandardpaths_mac.cpp' ],
        } ],

        ['OS=="android"', {
          'sources': [ 'io/qstandardpaths_unix.cpp' ],
        } ],

      ], # conditions
    }
  ],
}
