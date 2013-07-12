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
     
    [ 'OS == "win"', {
      'defines': [
         '_WIN32',
         'WIN32'
         'OS_WIN',
         'NOMINMAX',
         '_UNICODE',
         '_WINSOCKAPI_',
      ],
    } ],
  ],
}
