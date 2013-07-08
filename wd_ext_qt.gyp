{
  'includes': [
    'wd.gypi',
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

  },

  'targets': [
    {
      'target_name': 'WebDriver_extension_qt_base',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        'src/webdriver/extension_qt/q_content_type_resolver.cc',
        'src/webdriver/extension_qt/q_view_runner.cc',
        'src/webdriver/extension_qt/q_proxy_parser.cc',
        'src/webdriver/extension_qt/q_key_converter.cc',
        'src/webdriver/extension_qt/q_session_lifecycle_actions.cc',
        'src/webdriver/extension_qt/widget_view_util.cc',
        'src/webdriver/extension_qt/widget_view_handle.cc',
        'src/webdriver/extension_qt/widget_element_handle.cc',
        'src/webdriver/extension_qt/q_view_executor.cc',
        'src/webdriver/extension_qt/widget_view_creator.cc',
        'src/webdriver/extension_qt/widget_view_executor.cc',
        'src/webdriver/extension_qt/widget_view_enumerator.cc',
        'inc/extension_qt/q_view_runner.h',
        '<(INTERMEDIATE_DIR)/moc_q_view_runner.cc',
      ],

      'conditions': [

        [ '<(WD_CONFIG_XPATH) == 1', {
          'defines': [
            'WD_CONFIG_XPATH',
          ],
        } ],
       
      ],
    }, {
      'target_name': 'WebDriver_extension_qt_web',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        'src/webdriver/extension_qt/web_view_creator.cc',
        'src/webdriver/extension_qt/web_view_executor.cc',
        'src/webdriver/extension_qt/web_view_enumerator.cc',
        'src/webdriver/extension_qt/qwebviewext.cc',
        'src/webdriver/extension_qt/web_view_util.cc',
        'inc/extension_qt/web_view_executor.h',
        'inc/extension_qt/qwebviewext.h',
        '<(INTERMEDIATE_DIR)/moc_web_view_executor.cc',
        '<(INTERMEDIATE_DIR)/moc_qwebviewext.cc',
        'src/third_party/webdriver/atoms.cc',
      ],

    }
  ],
}
