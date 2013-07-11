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
