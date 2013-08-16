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
         'OS_ANDROID',
         '_GLIBCXX_PERMIT_BACKWARD_HASH',
       ],
       'include_dirs': [
          '<(ANDROID_INC)',
          '<(ANDROID_LIB_INC)',
          '<(ANDROID_PLATFORM_INC)',
          '<(QT_INC_PATH)/QtGui/5.1.0/QtGui'
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
  ],
}
