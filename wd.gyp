{
  'includes': [
    'wd.gypi',
  ],

  # TODO: move generate_wdversion.py here from build.sh

  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_web',
        # TODO: put quick1 only for qt4 not qt5
        #'wd_ext_qt.gyp:WebDriver_extension_qt_quick_1',
      ],

      'conditions': [

        [ 'OS == "linux"', {
          'dependencies': [
            'base.gyp:chromium_base_shared',
            'wd_core.gyp:WebDriver_core_shared',
            'wd_ext_qt.gyp:WebDriver_extension_qt_base_shared',
            'wd_ext_qt.gyp:WebDriver_extension_qt_web_shared',
          ],
        } ],

        ['platform == "desktop"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid',
            'wd_test.gyp:test_WD_hybrid_noWebkit',
          ],
        } ],

        ['platform == "desktop" and OS == "linux"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid_noWebkit_with_shared_libs',
          ],
        } ],

      ], # conditions

    }, # target_name: All
  ],
}
