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
      '<(QT_INC_PATH)',
      '<(INTERMEDIATE_DIR)',
    ],

    'conditions': [
      [ '<(QT5) == 1', {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L<(QT_LIB_PATH)',
              '-lQt5Network',
              '-lQt5Widgets',
              '-lQt5Quick',
              '-lQt5Qml',
              '-lQt5Sql',
              '-lQt5Gui',
              '-lQt5Xml',
              '-lQt5OpenGL',
              '-lQt5PrintSupport',
              '-lQt5Core',
              '-lpthread',
              '-lrt',
              '-ldl',
              '-licuuc',
              '-licudata',
              '-licui18n',
              '-lQt5MultimediaWidgets',
              '-lQt5Sensors',
              '-lQt5Multimedia',
              '-lQt5Positioning',
            ],
          } ],
          ['OS=="win"', {
            'libraries': [
             '-l<(QT_LIB_PATH)/Qt5Network',
             '-l<(QT_LIB_PATH)/Qt5Gui',
             '-l<(QT_LIB_PATH)/Qt5Core',
             '-l<(QT_LIB_PATH)/Qt5Widgets',
             '-l<(QT_LIB_PATH)/Qt5Qml',
             '-l<(QT_LIB_PATH)/Qt5Quick',
             '-l<(QT_LIB_PATH)/Qt5Xml',
             '-l<(QT_LIB_PATH)/Qt5MultimediaWidgets',
             '-l<(QT_LIB_PATH)/Qt5Multimedia',
             '-luser32.lib',
             '-lws2_32.lib',
             '-lshell32.lib',
             '-ladvapi32.lib',
             '-lpsapi.lib',
             '-lkernel32.lib',
             '-luserenv.lib',
            ],
          } ],
          [ 'OS=="mac"', {
            'link_settings': {
              'libraries': [
                '<(QT_LIB_PATH)/libQt5Network.a',
                '<(QT_LIB_PATH)/libQt5Gui.a',
                '<(QT_LIB_PATH)/libQt5Core.a',
                '<(QT_LIB_PATH)/libQt5Widgets.a',
                '<(QT_LIB_PATH)/libQt5Qml.a',
                '<(QT_LIB_PATH)/libQt5Quick.a',
                '<(QT_LIB_PATH)/libQt5MultimediaWidgets.a',
                '<(QT_LIB_PATH)/libQt5Multimedia.a',
                '<(QT_LIB_PATH)/libQt5Xml.a',
                '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/ApplicationServices.framework',
                '$(SDKROOT)/System/Library/Frameworks/Security.framework',
              ],
            },
            'xcode_settings': {
              'FRAMEWORK_SEARCH_PATHS': '<(QT_LIB_PATH)',
            },
          } ],
          ['OS=="android"', {
            'libraries': [
              '-L<(QT_LIB_PATH)',
              '-lQt5Widgets',
              '-lQt5Network',
              '-lQt5Gui',
              '-lQt5Core',
              '-lQt5AndroidExtras',
              '-L<(ANDROID_LIB)',
              '-lgnustl_shared',
              '-llog',
            ],
          } ],
          [ 'OS=="ios"', {
            'link_settings': {
              'libraries': [
                '<(QT_LIB_PATH)/libQt5Network_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Gui_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Core_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Widgets_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5MultimediaWidgets_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Multimedia_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Qml_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5OpenGL_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Quick_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5Xml_iphonesimulator.a',
                '<(QT_LIB_PATH)/libQt5PlatformSupport_iphonesimulator.a',
                '<(QT_LIB_PATH)/../plugins/platforms/libqios_iphonesimulator.a',
                '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreText.framework',
                '$(SDKROOT)/System/Library/Frameworks/OpenGLES.framework',
                '$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
                '$(SDKROOT)/System/Library/Frameworks/UIKit.framework',
                'libz.dylib',
              ],
            },

            'xcode_settings': {
              'TARGETED_DEVICE_FAMILY': '2',
              'CODE_SIGN_IDENTITY': 'iPhone Developer',
              'IPHONEOS_DEPLOYMENT_TARGET': '6.1',
              'ARCHS': '$(ARCHS_STANDARD_32_BIT)',
              'CLANG_WARN_CXX0X_EXTENSIONS': 'NO',
            },
          } ],
        ],
      }, {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L<(QT_LIB_PATH)',
              '-lQtNetwork',
              '-lQtGui',
              '-lQtXml',
              '-lQtCore',
              '-lQtDeclarative',
              '-lpthread',
              '-lrt',
              '-ldl',
            ],
          } ],
          [ 'OS=="win"', {
            'libraries': [
              '-l<(QT_LIB_PATH)/QtNetwork4',
              '-l<(QT_LIB_PATH)/QtGui4',
              '-l<(QT_LIB_PATH)/QtXml4',
              '-l<(QT_LIB_PATH)/QtCore4',
              '-l<(QT_LIB_PATH)/QtDeclarative4',
              '-luser32.lib',
              '-lws2_32.lib',
              '-lshell32.lib',
              '-ladvapi32.lib',
              '-lpsapi.lib',
              '-lkernel32.lib',
              '-luserenv.lib',
            ],
          } ],
          [ 'OS=="mac"', {
            'link_settings': {
              'libraries': [
                '<(QT_LIB_PATH)/QtGui.framework',
                '<(QT_LIB_PATH)/QtCore.framework',
                '<(QT_LIB_PATH)/QtNetwork.framework',
                '<(QT_LIB_PATH)/QtDeclarative.framework',
                '<(QT_LIB_PATH)/QtXml.framework',
                '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/ApplicationServices.framework',
                '$(SDKROOT)/System/Library/Frameworks/Security.framework',
              ],
              'xcode_settings': {
                'FRAMEWORK_SEARCH_PATHS': '<(QT_LIB_PATH)',
              },
            },
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
        'src/Test/Headers.h',
        'src/Test/Samples.h',
        'src/Test/ClickTest.cc',
        'src/Test/ClickTest.h',
        '<(INTERMEDIATE_DIR)/moc_ClickTest.cc',
        'src/Test/calculator.cpp',
        'src/Test/calculator.h',
        'src/Test/button.cpp',
        'src/Test/button.h',
        '<(INTERMEDIATE_DIR)/moc_calculator.cpp',
        '<(INTERMEDIATE_DIR)/moc_button.cpp',
        'src/Test/ClickScrollingTest.cc',
        'src/Test/ClickScrollingTest.h',
        'src/Test/ClickScrollingTest.ui',
        '<(INTERMEDIATE_DIR)/moc_ClickScrollingTest.cc',
        'src/Test/WindowTest.cc',
        'src/Test/WindowTest.h',
        '<(INTERMEDIATE_DIR)/moc_WindowTest.cc',
        'src/Test/FindingTest.cc',
        'src/Test/FindingTest.h',
        '<(INTERMEDIATE_DIR)/moc_FindingTest.cc',
        'src/Test/CoordinatesTest.cc',
        'src/Test/CoordinatesTest.h',
        '<(INTERMEDIATE_DIR)/moc_CoordinatesTest.cc',
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
        'src/Test/TextHandlingTest.h',
        '<(INTERMEDIATE_DIR)/moc_TextHandlingTest.cc',
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
        'src/Test/RestyledLabel.h',
        '<(INTERMEDIATE_DIR)/moc_RestyledLabel.cc',
        'src/Test/DragableWidget.cc',
        'src/Test/DragableWidget.h',
        '<(INTERMEDIATE_DIR)/moc_DragableWidget.cc',
        'src/Test/BasicMouseInterfaceTest.cc',
        'src/Test/BasicMouseInterfaceTest.h',
        '<(INTERMEDIATE_DIR)/moc_BasicMouseInterfaceTest.cc',
        'src/Test/TouchTest.cc',
        'src/Test/TouchTest.h',
        '<(INTERMEDIATE_DIR)/moc_TouchTest.cc',
        'src/Test/MenuTest.cc',
        'src/Test/MenuTest.h',
        '<(INTERMEDIATE_DIR)/moc_MenuTest.cc',
      ],
      'conditions': [
        [ '<(QT5) == 1 and <(WD_CONFIG_PLAYER) == 1', {
          'sources': [
            'src/Test/VideoTest.h',
            'src/Test/VideoTest.cc',
            '<(INTERMEDIATE_DIR)/moc_VideoTest.cc',
          ],
        } ],
        [ 'OS != "android" and <(QT5) == 0', {
          'sources': [
            'src/Test/WindowWithDeclarativeViewTest.cc',
            'src/Test/WindowWithDeclarativeViewTest.h',
            '<(INTERMEDIATE_DIR)/moc_WindowWithDeclarativeViewTest.cc',
          ],
        } ]
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

      'defines': [ 'WD_ENABLE_WEB_VIEW=1' ],

      'sources': [
        'src/Test/main.cc',
        'src/Test/WindowWithEmbeddedViewTest.cc',
        'src/Test/WindowWithEmbeddedViewTest.h',
        '<(INTERMEDIATE_DIR)/moc_WindowWithEmbeddedViewTest.cc',
        'src/Test/WidgetAndWebViewTest.cc',
        'src/Test/WidgetAndWebViewTest.h',
        '<(INTERMEDIATE_DIR)/moc_WidgetAndWebViewTest.cc',
        'src/Test/GraphicsWebViewTest.cc',
        'src/Test/GraphicsWebViewTest.h',
        '<(INTERMEDIATE_DIR)/moc_GraphicsWebViewTest.cc',
      ],

      'conditions': [
        ['<(WD_CONFIG_QUICK) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick', 'wd_ext_qt.gyp:WebDriver_extension_qt_quick_web'],

          'conditions': [
            [ 'OS == "linux"', {
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick_shared'],
            } ],
          ],
        }, {
          'defines': [
             'QT_NO_QML',
           ],
        } ],

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
              'libraries': ['-l<(QT_LIB_PATH)/Qt5WebKit', '-l<(QT_LIB_PATH)/Qt5WebKitWidgets'],
            } ],
            [ 'OS=="mac"', {
              'link_settings': {
                'libraries': ['<(QT_LIB_PATH)/libQt5WebKit.a','<(QT_LIB_PATH)/libQt5WebKitWidgets.a',],
              },
            } ],
          ],
        }, {
          # else QT4
          'sources' : [
            'src/Test/WindowWithSeparatedDeclarativeAndWebViewsTest.cc',
            'src/Test/WindowWithSeparatedDeclarativeAndWebViewsTest.h',
            '<(INTERMEDIATE_DIR)/moc_WindowWithSeparatedDeclarativeAndWebViewsTest.cc',
          ],

          'conditions': [
            ['OS=="linux"', {
              'libraries': ['-lQtWebKit',],
            } ],
            [ 'OS=="win"', {
              'libraries': ['-l<(QT_LIB_PATH)/QtWebKit4',],
            } ],
            [ 'OS=="mac"', {
              'link_settings': {
                'libraries': ['<(QT_LIB_PATH)/QtWebKit.framework',],
              },
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
        ['<(WD_CONFIG_QUICK) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick'],

          'conditions': [
            [ 'OS == "linux"', {
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick_shared'],
            } ],
          ],
        } , {
          'defines': [
             'QT_NO_QML',
           ],
        }],

        [ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ]
      ],

    }, {
      'target_name': 'test_WD_hybrid_noWebkit_with_shared_libs',
      'type': 'executable',

      'product_name': 'WebDriver_noWebkit_sharedLibs',

      'dependencies': [
        'base.gyp:chromium_base_shared',
        'wd_core.gyp:WebDriver_core_shared',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base_shared',
        'test_widgets',
      ],

      'sources': [
        'src/Test/main.cc',
      ],

      'conditions': [
        ['<(WD_CONFIG_QUICK) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick'],

          'conditions': [
            [ 'OS == "linux"', {
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick_shared'],
            } ],
          ],
        }, {
          'defines': [
             'QT_NO_QML',
           ],
        }
        ],

        [ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],
      ],
    }, {
      'target_name': 'test_android_WD_QML',
      'type': 'shared_library',

      'product_name': 'AndroidWD_QML',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick',
        'test_widgets',
      ],
      'libraries': [
        '-lQt5Multimedia',
        '-lQt5MultimediaWidgets',
        '-lQt5Quick',
        '-lQt5Qml',
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

    },
    {
      'target_name': 'test_android_WD_Widgets',
      'type': 'shared_library',

      'product_name': 'AndroidWD_Widgets',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'test_widgets',
      ],

      'defines': [
         'QT_NO_QML',
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

    }, {
      'target_name': 'test_ios_WD',
      'type': 'executable',
      'mac_bundle': 1,

      'product_name': 'WebDriver_ios',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick',
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
    },
  ],
}
