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
        'base.gyp:chromium_base_shared',
        'wd_core.gyp:WebDriver_core',
        'wd_core.gyp:WebDriver_core_shared',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base',
        'wd_ext_qt.gyp:WebDriver_extension_qt_base_shared',
        'wd_ext_qt.gyp:WebDriver_extension_qt_web',
        'wd_ext_qt.gyp:WebDriver_extension_qt_web_shared',
      ],

      'conditions': [
        ['platform == "desktop"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid',
            'wd_test.gyp:test_WD_hybrid_noWebkit',
          ],
        } ],
      ],

    }, # target_name: All
  ],
}
