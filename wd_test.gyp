{
  'includes': [
    'wd.gypi',
    'wd_common.gypi',
  ],

  'target_defaults': {
    'includes': [
      'wd_build_options.gypi',
      'wd_qt_tools.gypi',
    ],

    # TODO: review include_dirs
    'include_dirs': [
      'inc/',
      'src/',
      '<(QT_INC_PATH)',
      '<(INTERMEDIATE_DIR)',
    ],

    'conditions': [
      [ '<(QT5) == 1', {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L<(QT_LIB_PATH)',
#              '-lQt5WebKitWidgets',
              '-lQt5OpenGL',
              '-lQt5PrintSupport',
#              '-lQt5WebKit',
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
#            	'-l<(QT_LIB_PATH)/QtWebKit4',
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
#              '-lQtWebKit',
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
#              '-l<(QT_LIB_PATH)/QtWebKit4',
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
    ], # conditions

  }, # target_defaults

  'targets': [
    {
      'target_name': 'test_widgets',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
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

    }, {
      'target_name': 'test_WD_hybrid',
      'type': 'executable',

      'product_name': 'WebDriver',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_web',
        'test_widgets',
      ],

      'defines': [ 'WD_TEST_ENABLE_WEB_VIEW=1' ],
      
      'sources': [
        'src/Test/main.cc',
      ],

      'conditions': [
      	[ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],

        [ '<(QT5) == 1', {
          'conditions': [
            ['OS=="linux"', {
              'libraries': ['-lQt5WebKitWidgets', '-lQt5WebKit',],
            } ],
            [ 'OS=="win"', {
              #TODO: set here Qt5 libs
              'libraries': ['-l<(QT_LIB_PATH)/QtWebKit4',],
            } ],
          ],
        }, {
          'conditions': [
            ['OS=="linux"', {
              'libraries': ['-lQtWebKit',],
            } ],
            [ 'OS=="win"', {
              'libraries': ['-l<(QT_LIB_PATH)/QtWebKit4',],
            } ],
          ],
        } ],
      ],

    }, {
      'target_name': 'test_WD_hybrid_noWebkit',
      'type': 'executable',

      'product_name': 'WebDriver_noWebkit',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'test_widgets',
      ],
      
      'sources': [
        'src/Test/main.cc',
      ],

      'conditions': [
        [ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],
      ],

    }
  ],
}
