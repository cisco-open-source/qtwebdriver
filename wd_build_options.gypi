{
	# do not require cygwin
  'msvs_cygwin_shell': 0,

  'msvs_configuration_attributes': {
        'CharacterSet': '1'
      },

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
    ],
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
    ['OS=="linux"', {
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
    } ]
  ],

	# TODO: extract this to separated gypi file  
  'actions': [{
    'action_name' : 'input_dir',
    'inputs' : [],
    'outputs': [
      '<(INTERMEDIATE_DIR)',
    ],
    'action': ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
  }],
}