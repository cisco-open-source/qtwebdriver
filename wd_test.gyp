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
              #'-L/opt/lib',
              '-lQt5OpenGL',
              '-lQt5PrintSupport',
              '-lQt5Script',
              '-lQt5Network',
              '-lQt5V8',
              '-lQt5Widgets',
              '-lQt5Quick',
              '-lQt5Qml',
              '-lQt5Sql',
              '-lQt5Gui',
              '-lQt5Xml',
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
                'Foundation.framework',
                'CoreFoundation.framework',
                'ApplicationServices.framework',
                'Security.framework',
              ],
            }, 
          } ], 
          ['OS=="android"', {
            'libraries': [		
              '-L<(QT_LIB_PATH)',
              '-lQt5Widgets',
              '-lQt5Network',
              '-lQt5Gui',
              '-lQt5Core',
              '-lQt5Quick',
              '-lQt5Qml',
              '-lQt5Xml',
              '-L<(ANDROID_LIB)',
              '-lgnustl_shared',
              '-llog',
            ],
          } ],
          [ 'OS=="ios"', {
            'link_settings': {
              'libraries': [
                '<(QT_LIB_PATH)/libQt5Network.a',
                '<(QT_LIB_PATH)/libQt5Gui.a',
                '<(QT_LIB_PATH)/libQt5Core.a',
                '<(QT_LIB_PATH)/libQt5Widgets.a',
                '<(QT_LIB_PATH)/libQt5Qml.a',
                '<(QT_LIB_PATH)/libQt5Quick.a',
                '<(QT_LIB_PATH)/libQt5V8.a',
                #'<(QT_LIB_PATH)/libQt5OpenGL.a',
                #'<(QT_LIB_PATH)/libQt5OpenGLExtensions.a',
                #'<(QT_LIB_PATH)/libQt5PlatformSupport.a',
                #'<(QT_LIB_PATH)/libQt5Script.a',
                #'<(QT_LIB_PATH)/../plugins/platforms/libqios.a',
                #'<(QT_LIB_PATH)/../plugins/platforms/libquikit.a',
                #'/Developer/Qt4iOS/ios-qml/IOSPluginLibs/ios-clang-release/lib/libqmlcomponectsios.a',
                '$(SDKROOT)/System/Library/Frameworks/Foundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreGraphics.framework',
                '$(SDKROOT)/System/Library/Frameworks/CoreText.framework',
                '$(SDKROOT)/System/Library/Frameworks/UIKit.framework',
              ],
            }, 
     
            'xcode_settings': {
              'TARGETED_DEVICE_FAMILY': '1,2',
              'CODE_SIGN_IDENTITY': 'iPhone Developer',
              'IPHONEOS_DEPLOYMENT_TARGET': '5.0',
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
            },
          } ],
          [ 'OS=="ios"', {
            'link_settings': {
              'libraries': [
                '<(QT_LIB_PATH)/QtGui.framework',
                '<(QT_LIB_PATH)/QtCore.framework',
                '<(QT_LIB_PATH)/QtNetwork.framework',
                '<(QT_LIB_PATH)/QtDeclarative.framework',
                #'<(QT_LIB_PATH)/../plugins/platforms/libqios.a',
                '<(QT_LIB_PATH)/../plugins/platforms/libcruikit.a',
                '/Developer/Qt4iOS/ios-qml/IOSPluginLibs/ios-clang-release/lib/libqmlcomponectsios.a',
                '<(SDK_ROOT)/System/Library/Frameworks/Foundation.framework',
                '<(SDK_ROOT)/System/Library/Frameworks/CoreFoundation.framework',
                '<(SDK_ROOT)/System/Library/Frameworks/CoreGraphics.framework',
                '<(SDK_ROOT)/System/Library/Frameworks/CoreText.framework',
                '<(SDK_ROOT)/System/Library/Frameworks/UIKit.framework',
              ],
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
        'src/Test/BasicMouseInterfaceTest.cc',
      ],
      'conditions': [
        [ '<(QT5) == 1', {
          'sources': [
            'src/Test/VideoTest.h',
            'src/Test/VideoTest.cc',
            '<(INTERMEDIATE_DIR)/moc_VideoTest.cc',
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
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick',
        'test_widgets',
      ],

      'defines': [ 'WD_TEST_ENABLE_WEB_VIEW=1' ],
      
      'sources': [
        'src/Test/main.cc',
        'src/Test/shutdown_command.cc',
        'src/Test/WindowWithEmbeddedViewTest.cc',
        'src/Test/WindowWithEmbeddedViewTest.h',
        '<(INTERMEDIATE_DIR)/moc_WindowWithEmbeddedViewTest.cc',
        'src/Test/WidgetAndWebViewTest.cc',
        'src/Test/WidgetAndWebViewTest.h',
        '<(INTERMEDIATE_DIR)/moc_WidgetAndWebViewTest.cc',
      ],

      'conditions': [
      	[ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],
       
        [ 'OS=="ios"', {
          'mac_bundle': 1, 
        } ],

        [ '<(QT5) == 1', {
          'conditions': [
            ['OS=="linux"', {
              'libraries': ['-lQt5WebKitWidgets', '-lQt5WebKit',],
            } ],
            [ 'OS=="win"', {
              'libraries': ['-l<(QT_LIB_PATH)/Qt5WebKit', '-l<(QT_LIB_PATH)/Qt5WebKitWidgets'],
            } ],
            [ 'OS=="mac" or OS=="ios"', {
              'link_settings': {
                'libraries': ['<(QT_LIB_PATH)/libQt5WebKit.a','<(QT_LIB_PATH)/libQt5WebKitWidgets.a',],
              },
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
            [ 'OS=="mac" or OS=="ios"', {
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
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick',
        'test_widgets',
      ],
      
      'sources': [
        'src/Test/main.cc',
        'src/Test/shutdown_command.cc',
      ],

      'conditions': [
        [ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],

        [ 'OS=="ios"', {
          'mac_bundle': 1, 
        } ],
      ],

    } , {
      'target_name': 'test_WD_hybrid_noWebkit_with_shared_libs',
      'type': 'executable',

      'product_name': 'WebDriver_noWebkit_sharedLibs',

      'dependencies': [
        'base.gyp:chromium_base_shared',
        'wd_core.gyp:WebDriver_core_shared',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base_shared',
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick_shared',
        'test_widgets',
      ],
      
      'sources': [
        'src/Test/main.cc',
        'src/Test/shutdown_command.cc',
      ],

      'conditions': [
        [ '<(WD_BUILD_MONGOOSE) == 0', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],
      ],
    } , {
      'target_name': 'test_android_WD_noWebkit',
      'type': 'shared_library',

      'product_name': 'WebDriver_noWebkit_android',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_quick',
        'test_widgets',
      ],
      
      'sources': [
        'src/Test/main.cc',
        'src/Test/shutdown_command.cc',
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
