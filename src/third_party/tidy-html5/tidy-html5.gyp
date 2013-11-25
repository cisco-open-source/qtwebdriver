{
  'includes': [
    '../../../wd.gypi',
    '../../../wd_common.gypi',
  ],

  'target_defaults': {
    'includes': [
      '../../../wd_build_options.gypi',
    ],
  },

  'targets': [
    {
      'target_name': 'tidy-html5',
      'type': 'static_library',
      'standalone_static_library': 1,

      'cflags': [
        '-Wno-missing-field-initializers',
      ],

      'include_dirs': [
        'include',
      ],

      'sources': [
        'src/access.c',
        'src/alloc.c',
        'src/attrask.c',
        'src/attrdict.c',
        'src/attrget.c',
        'src/attrs.c',
        'src/buffio.c',
        'src/charsets.c',
        'src/clean.c',
        'src/config.c',
        'src/entities.c',
        'src/fileio.c',
        'src/gdoc.c',
        'src/iconvtc.c',
        'src/istack.c',
        'src/lexer.c',
        'src/localize.c',
        'src/mappedio.c',
        'src/parser.c',
        'src/pprint.c',
        'src/streamio.c',
        'src/tagask.c',
        'src/tags.c',
        'src/tidylib.c',
        'src/tmbstr.c',
        'src/utf8.c',
        'src/win32tc.c',
      ],

      'conditions': [
      
        ['OS=="linux"', {
          'sources': [ ],
        } ],

        [ 'OS == "win"', {
          'sources': [ ],
        } ],

        [ 'OS == "mac" or OS=="ios"', {
          'sources': [ ],
        } ],

        ['OS=="android"', {
          'sources': [ ],
        } ],

      ], # conditions
    }
  ],
}
