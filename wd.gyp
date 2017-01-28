{
  'includes': [
    'wd.gypi',
    'wd_common.gypi',
  ],

  # TODO: move generate_wdversion.py here from build.sh

  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
      ],

      'conditions': [

        ['<(WD_CONFIG_QWIDGET_BASE) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_base'],

          'conditions': [
            [ 'OS == "linux"', { 
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_base_shared'],
            } ],
          ],
        } ],

        ['<(WD_CONFIG_QUICK) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick'],

          'conditions': [
            [ 'OS == "linux"', { 
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_quick_shared'],
            } ],
          ],
        } ],

        ['<(WD_CONFIG_WEBKIT) == 1', {
          'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_web'],

          'conditions': [
            [ 'OS == "linux"', { 
              'dependencies': ['wd_ext_qt.gyp:WebDriver_extension_qt_web_shared'],
            } ],
          ],
        } ],

        [ 'OS == "linux"', {
          'dependencies': [
            'base.gyp:chromium_base_shared',
            'wd_core.gyp:WebDriver_core_shared',
          ],
        } ],

        ['platform == "desktop"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid_noWebkit',
          ],
          'conditions': [
            ['<(WD_CONFIG_WEBKIT) == 1', {
              'dependencies': ['wd_test.gyp:test_WD_hybrid']
            } ],
          ]
        } ],

        ['platform == "desktop" and OS == "linux"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid_noWebkit_with_shared_libs',
          ],
        } ],

        [ 'OS == "android"', {
          'dependencies': [
            'wd_ext_qt.gyp:WebDriver_extension_qt_base',
            'wd_test.gyp:test_android_WD_Widgets',
            'wd_test.gyp:test_android_WD_QML',
          ],
        } ],

      ], # conditions

    }, # target_name: All
  ],
}
