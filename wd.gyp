{
  'includes': [
    'wd.gypi',
  ],

  'variables': {
    'QT5%': '0',
    'WD_CONFIG_QWIDGET_VIEW%': '0',
    'WD_CONFIG_QML_VIEW%': '0',
    'WD_BUILD_MONGOOSE%': '0',
    'WD_CONFIG_XPATH%': '1',
    'MONGOOSE_INC_PATH%': 'src/third_party/mongoose',
  },

  'conditions': [
    [ 'platform == "desktop"', {
      'variables': {
        'QT_BIN_PATH': '<(DESKTOP_QT_BIN_PATH)',
        'QT_INC_PATH': '<(DESKTOP_QT_INC_PATH)',
        'QT_LIB_PATH': '<(DESKTOP_QT_LIB_PATH)'
      },

      'targets': [
        {
          'target_name': 'WebDriverTest',
          'type': 'executable',

          'product_name': 'WebDriver',

          'include_dirs': [
            'inc/',
            'src/',
            '<(INTERMEDIATE_DIR)',
            '<(QT_INC_PATH)',
          ],

          'dependencies': [
            'base.gyp:chromium_base',
            'wd_core.gyp:WebDriver_core',
            'WebDriver_extension_qt',
          ],


          'sources': [
            'src/Test/main.cc',
            'src/Test/ClickTest.cc',
            'src/Test/ClickTest.h',
            '<(INTERMEDIATE_DIR)/moc_ClickTest.cc',
            'src/Test/ClickScrollingTest.cc',
            'src/Test/ClickScrollingTest.h',
            'src/Test/ClickScrollingTest.ui',
            '<(INTERMEDIATE_DIR)/moc_ClickScrollingTest.cc',
            'src/Test/WindowTest.cc',
            'src/Test/FindingTest.cc',
            'src/Test/CoordinatesTest.cc',
            'src/Test/ElementAttributeTest.cc',
            'src/Test/ElementAttributeTest.h',
            '<(INTERMEDIATE_DIR)/moc_ElementAttributeTest.cc',
            'src/Test/ElementSelectingTest.cc',
            'src/Test/ElementSelectingTest.h',
            'src/Test/ElementSelectingTest.ui',
            '<(INTERMEDIATE_DIR)/moc_ElementSelectingTest.cc',
            'src/Test/TypingTest.cc',
            'src/Test/TypingTest.h',
            '<(INTERMEDIATE_DIR)/moc_TypingTest.cc',
            'src/Test/BasicKeyboardInterfaceTest.cc',
            'src/Test/BasicKeyboardInterfaceTest.h',
            '<(INTERMEDIATE_DIR)/moc_BasicKeyboardInterfaceTest.cc',
            'src/Test/TextHandlingTest.cc',
            'src/Test/FormHandlingTest.cc',
            'src/Test/FormHandlingTest.h',
            '<(INTERMEDIATE_DIR)/moc_FormHandlingTest.cc',
            'src/Test/img.xpm',
            'src/Test/XPathElementFindingTest.cc',
            'src/Test/XPathElementFindingTest.h',
            '<(INTERMEDIATE_DIR)/moc_XPathElementFindingTest.cc',
            'src/Test/StaleElementReferenceTest.cc',
            'src/Test/StaleElementReferenceTest.h',
            '<(INTERMEDIATE_DIR)/moc_StaleElementReferenceTest.cc',
            'src/Test/VisibilityTest.cc',
            'src/Test/VisibilityTest.h',
            '<(INTERMEDIATE_DIR)/moc_VisibilityTest.cc',
            'src/Test/RestyledLabel.cc',
            'src/Test/DragableWidget.cc',
            'src/Test/BasicMouseInterfaceTest.cc'
          ],

          'conditions': [
            [ '<(QT5) == 1', {
              'conditions': [
                ['OS=="linux"', {
                  'libraries': [
                    '-L<(QT_LIB_PATH)',
                    '-lQt5WebKitWidgets',
                    '-lQt5OpenGL',
                    '-lQt5PrintSupport',
                    '-lQt5WebKit',
                    '-lQt5Script',
                    '-lQt5Network',
                    '-lQt5V8',
                    '-lQt5Widgets',
                    '-lQt5Quick',
                    '-lQt5Qml',
                    '-lQt5Sql',
                    '-lQt5Declarative',
                    '-lQt5XmlPatterns',
                    '-lQt5Xml',
                    '-lQt5Gui',
                    '-lQt5Core',
                    '-lpthread',
                    '-lrt',
                    '-ldl',
                    '-licuuc',
                    '-licudata',
                    '-licui18n',
                  ],
                } ],
                [ 'OS=="win"', {
                  #TODO: set here Qt5 libs
                  'libraries': [
                    '-l<(QT_LIB_PATH)/QtWebKit4',
                    '-l<(QT_LIB_PATH)/QtNetwork4',
                    '-l<(QT_LIB_PATH)/QtXml4',
                    '-l<(QT_LIB_PATH)/QtGui4',
                    '-l<(QT_LIB_PATH)/QtCore4',
                  ],
                } ],
              ],
            }, {
              'conditions': [
                ['OS=="linux"', {
                  'libraries': [
                    '-L<(QT_LIB_PATH)',
                    '-lQtWebKit',
                    '-lQtNetwork',
                    '-lQtXml',
                    '-lQtXmlPatterns',
                    '-lQtGui',
                    '-lQtCore',
                    '-lpthread',
                    '-lrt',
                    '-ldl',
                  ],
                } ],
                [ 'OS=="win"', {
                  'libraries': [
                    '-l<(QT_LIB_PATH)/QtWebKit4',
                    '-l<(QT_LIB_PATH)/QtNetwork4',
                    '-l<(QT_LIB_PATH)/QtXml4',
                    '-l<(QT_LIB_PATH)/QtGui4',
                    '-l<(QT_LIB_PATH)/QtCore4',
                    '-luser32.lib',
                    '-lws2_32.lib',
                    '-lshell32.lib',
                    '-ladvapi32.lib',
                    '-lpsapi.lib',
                    '-lkernel32.lib',
                    '-luserenv.lib',
                  ],
                } ],
              ],
            } ],

            [ '<(WD_BUILD_MONGOOSE) == 0', {
              'sources': [
                'src/third_party/mongoose/mongoose.c',
              ],
            } ],

          ],
        },
      ],
    } ],

    [ 'platform != "desktop"', {
      'variables': {
		    'QT_BIN_PATH': '<(CISCO_QT_BIN_PATH)',
        'QT_INC_PATH': '<(CISCO_QT_INC_PATH)',
        'QT_LIB_PATH': '<(CISCO_QT_LIB_PATH)',
        'MONGOOSE_INC_PATH':  '<(CISCO_MONGOOSE_INC_PATH)',
      },
    } ],
  ],

  'target_defaults': {
    # do not require cygwin
    'msvs_cygwin_shell': 0,

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
      [ '<(WD_CONFIG_QWIDGET_VIEW) == 1', {
        'defines': [
          'WD_CONFIG_QWIDGET_VIEW',
        ],
      } ],
      [ '<(WD_CONFIG_QML_VIEW) == 1', {
        'defines': [
          'WD_CONFIG_QML_VIEW',
        ],
      } ],
      [ '<(WD_CONFIG_XPATH) == 1', {
        'defines': [
          'WD_CONFIG_XPATH',
        ],
      } ],
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
    'actions': [{
      'action_name' : 'input_dir',
      'inputs' : [],
      'outputs': [
        '<(INTERMEDIATE_DIR)',
      ],
      'action': ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
    }],

    'rules': [         
    {
      'rule_name': 'generate_ui',
      'extension': 'ui',
      'outputs': [ '<(RULE_INPUT_DIRNAME)/ui_<(RULE_INPUT_ROOT).h' ],
      'action': [ '<(QT_BIN_PATH)/uic', '<(RULE_INPUT_PATH)', '-o', '<(INTERMEDIATE_DIR)/ui_<(RULE_INPUT_ROOT).h' ],
      'message': 'Generating ui_<(RULE_INPUT_ROOT).h',
    },
    {
      'rule_name': 'generate_moc',
      'extension': 'h',
      'outputs': [ '<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cc' ],
      'action': [ '<(QT_BIN_PATH)/moc', '<(RULE_INPUT_PATH)', '-o', '<(INTERMEDIATE_DIR)/moc_<(RULE_INPUT_ROOT).cc' ],
      'message': 'Generating <(RULE_INPUT_ROOT).cc.',
    } ],
  },

  'targets': [
    {
      'target_name': 'WebDriver_extension_qt',
      'type': 'static_library',
      'standalone_static_library': 1,
      'msvs_configuration_attributes': {
        'CharacterSet': '1'
      },

      'include_dirs': [
        'inc/',
        'src/',
        '<(QT_INC_PATH)',
        '<(INTERMEDIATE_DIR)',
      ],

      'sources': [

        # TODO: files below belong to custom implementation
        'src/webdriver/extension_qt/web_view_creator.cc',
        'src/webdriver/extension_qt/widget_view_creator.cc',
        'src/webdriver/extension_qt/q_view_executor.cc',
        'src/webdriver/extension_qt/web_view_executor.cc',
        'src/webdriver/extension_qt/widget_view_executor.cc',
        'src/webdriver/extension_qt/web_view_enumerator.cc',
        'src/webdriver/extension_qt/widget_view_enumerator.cc',
        'src/webdriver/extension_qt/q_view_runner.cc',
        'src/webdriver/extension_qt/q_proxy_parser.cc',
        'src/webdriver/extension_qt/q_session_lifecycle_actions.cc',
        'src/webdriver/extension_qt/qwebviewext.cc',
        'src/webdriver/extension_qt/q_key_converter.cc',
        'src/webdriver/extension_qt/widget_view_util.cc',
        'src/webdriver/extension_qt/web_view_util.cc',
        'src/webdriver/extension_qt/widget_view_handle.cc',
        'src/webdriver/extension_qt/widget_element_handle.cc',
        'src/webdriver/extension_qt/q_content_type_resolver.cc',
        'inc/extension_qt/q_view_runner.h',
        'inc/extension_qt/web_view_executor.h',
        'inc/extension_qt/qwebviewext.h',
        '<(INTERMEDIATE_DIR)/moc_q_view_runner.cc',
        '<(INTERMEDIATE_DIR)/moc_web_view_executor.cc',
        '<(INTERMEDIATE_DIR)/moc_qwebviewext.cc',
#        'src/net/base/file_stream.cc',
#        'src/net/base/file_stream_metrics.cc',
#        'src/net/base/file_stream_net_log_parameters.cc',
#        'src/net/base/io_buffer.cc',
#        'src/net/base/net_errors.cc',
#        'src/net/base/net_log.cc',
        'src/third_party/webdriver/atoms.cc',
      ],

    }, {
      # TODO: rewrite this target
      'target_name': 'WebDriverShared',
      'type': 'shared_library',

      'product_name': 'WebDriver',

      'dependencies': [
        'WebDriver_extension_qt',
      ],
    },
  ],
}
