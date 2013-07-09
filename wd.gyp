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
          ],
        } ],
      ],

    }, # target_name: All
  ],
}
