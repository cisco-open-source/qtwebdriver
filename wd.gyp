{
  'includes': [
    'wd.gypi',
  ],

  'variables': {
    'QT5%': '0',
    'WD_CONFIG_QWIDGET_VIEW%': '0',
    'WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY%': '0',
    'WD_CONFIG_QML_VIEW%': '0',
    'WD_BUILD_MONGOOSE%': '0',
    'WD_BUILD_MODP_B64%': '0',
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
            '<(QT_INC_PATH)',
          ],

          'dependencies': [
            'WebDriver',
          ],

          'sources': [
            'src/Test/main.cc',
          ],

          'conditions': [
            [ '<(QT5) == 1', {
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
              ],
            }, {
              'libraries': [
                '-L<(QT_LIB_PATH)',
                '-lQtWebKit',
                '-lQtNetwork',
                '-lQtXml',
                '-lQtGui',
                '-lQtCore',
                '-lpthread',
                '-lrt',
                '-ldl',
              ],
            } ],

            [ '<(WD_BUILD_MONGOOSE) == 0', {
              'sources': [
                'src/third_party/mongoose/mongoose.c',
              ],
            } ],

            [ '<(WD_BUILD_MODP_B64) == 0', {
              'sources': [
                'src/third_party/modp_b64/modp_b64.cc',
              ],
            } ],
          ],
        },
      ],
    } ],

    [ 'platform != "desktop"', {
      'variables': {
		'QT_BIN_PATH': '<(ROOT_PATH)/<(CISCO_QT_BIN_PATH)',
        'QT_INC_PATH': '<(ROOT_PATH)/<(CISCO_QT_INC_PATH)',
        'QT_LIB_PATH': '<(ROOT_PATH)/<(CISCO_QT_LIB_PATH)',
        'MONGOOSE_LIB_PATH': '<(ROOT_PATH)/<(CISCO_MONGOOSE_LIB_PATH)',
        'MONGOOSE_INC_PATH':  '<(ROOT_PATH)/<(CISCO_MONGOOSE_INC_PATH)',
        'MINIZIP_LIB_PATH': '<(ROOT_PATH)/<(CISCO_MINIZIP_LIB_PATH)',
        'MINIZIP_INC_PATH':  '<(ROOT_PATH)/<(CISCO_MINIZIP_INC_PATH)',
        'ZLIB_LIB_PATH': '<(ROOT_PATH)/<(CISCO_ZLIB_LIB_PATH)',
        'ZLIB_INC_PATH':  '<(ROOT_PATH)/<(CISCO_ZLIB_INC_PATH)',
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
      '__STDC_FORMAT_MACROS',
      'OS_POSIX',
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
      [ '<(WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY) == 1', {
        'defines': [
          'WD_CONFIG_QWIDGET_VIEW_ACCESSABILITY',
        ],
      } ],
      [ '<(WD_CONFIG_QML_VIEW) == 1', {
        'defines': [
          'WD_CONFIG_QML_VIEW',
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
      } ]
    ],
  },

  'targets': [
    {
      'target_name': 'WebDriver',
      'type': 'static_library',

      'standalone_static_library': 1,

      'include_dirs': [
        'inc/',
        'src/',
        '<(QT_INC_PATH)',
        '<(MONGOOSE_INC_PATH)',
        '<(MINIZIP_INC_PATH)',
        '<(ZLIB_INC_PATH)',
      ],

      'sources': [
        'src/base/at_exit.cc',
        'src/base/atomicops_internals_x86_gcc.cc',
        'src/base/base64.cc',
        'src/base/base_paths.cc',
        'src/base/base_paths_posix.cc',
        'src/base/base_switches.cc',
        'src/base/callback_internal.cc',
        'src/base/command_line.cc',
        'src/base/debug/alias.cc',
        'src/base/debug/debugger.cc',
        'src/base/debug/debugger_posix.cc',
        'src/base/environment.cc',
        'src/base/file_path.cc',
        'src/base/file_util.cc',
        'src/base/file_util_posix.cc',
        'src/base/json/json_parser.cc',
        'src/base/json/json_reader.cc',
        'src/base/json/json_writer.cc',
        'src/base/json/string_escape.cc',
        'src/base/lazy_instance.cc',
        'src/base/location.cc',
        'src/base/logging.cc',
        'src/base/memory/ref_counted.cc',
        'src/base/memory/singleton.cc',
        'src/base/memory/weak_ptr.cc',
        'src/base/message_loop.cc',
        'src/base/message_loop_proxy.cc',
        'src/base/message_loop_proxy_impl.cc',
        'src/base/message_pump.cc',
        'src/base/message_pump_default.cc',
        'src/base/metrics/bucket_ranges.cc',
        'src/base/metrics/histogram_base.cc',
        'src/base/metrics/histogram.cc',
        'src/base/metrics/histogram_samples.cc',
        'src/base/metrics/sample_vector.cc',
        'src/base/metrics/statistics_recorder.cc',
        'src/base/nix/xdg_util.cc',
        'src/base/path_service.cc',
        'src/base/pending_task.cc',
        'src/base/pickle.cc',
        'src/base/platform_file.cc',
        'src/base/platform_file_posix.cc',
        'src/base/profiler/alternate_timer.cc',
        'src/base/profiler/scoped_profile.cc',
        'src/base/profiler/tracked_time.cc',
        'src/base/rand_util.cc',
        'src/base/rand_util_posix.cc',
        'src/base/run_loop.cc',
        'src/base/safe_strerror_posix.cc',
        'src/base/scoped_temp_dir.cc',
        'src/base/string16.cc',
        'src/base/string_number_conversions.cc',
        'src/base/string_piece.cc',
        'src/base/stringprintf.cc',
        'src/base/string_split.cc',
        'src/base/string_util.cc',
        'src/base/synchronization/condition_variable_posix.cc',
        'src/base/synchronization/lock.cc',
        'src/base/synchronization/lock_impl_posix.cc',
        'src/base/synchronization/waitable_event_posix.cc',
        'src/base/sys_info_linux.cc',
        'src/base/sys_info_posix.cc',
        'src/base/sys_string_conversions_posix.cc',
        'src/base/task_runner.cc',
        'src/base/third_party/dmg_fp/dtoa.cc',
        'src/base/third_party/dmg_fp/g_fmt.cc',
        'src/base/third_party/dynamic_annotations/dynamic_annotations.c',
        'src/base/third_party/icu/icu_utf.cc',
        'src/base/third_party/nspr/prtime.cc',
        'src/base/third_party/xdg_user_dirs/xdg_user_dir_lookup.cc',
        'src/base/threading/platform_thread_posix.cc',
        'src/base/threading/post_task_and_reply_impl.cc',
        'src/base/threading/thread.cc',
        'src/base/threading/thread_checker_impl.cc',
        'src/base/threading/thread_collision_warner.cc',
        'src/base/threading/thread_local_posix.cc',
        'src/base/threading/thread_local_storage_posix.cc',
        'src/base/threading/thread_restrictions.cc',
        'src/base/threading/worker_pool.cc',
        'src/base/threading/worker_pool_posix.cc',
        'src/base/thread_task_runner_handle.cc',
        'src/base/time.cc',
        'src/base/time_posix.cc',
        'src/base/timer.cc',
        'src/base/tracked_objects.cc',
        'src/base/tracking_info.cc',
        'src/base/utf_string_conversions.cc',
        'src/base/utf_string_conversion_utils.cc',
        'src/base/values.cc',
        'src/chrome/common/automation_constants.cc',
        'src/chrome/common/automation_id.cc',
        'src/chrome/common/chrome_constants.cc',
        'src/chrome/common/chrome_switches.cc',
        'src/chrome/common/chrome_version_info.cc',
        'src/chrome/common/chrome_version_info_posix.cc',
        'src/chrome/common/zip.cc',
        'src/chrome/common/zip_internal.cc',
        'src/chrome/common/zip_reader.cc',
        'src/chrome/test/automation/automation_json_requests.cc',
        'src/chrome/test/automation/value_conversion_traits.cc',
        'src/chrome/test/webdriver/commands/alert_commands.cc',
        'src/chrome/test/webdriver/commands/appcache_status_command.cc',
        'src/chrome/test/webdriver/commands/browser_connection_commands.cc',
        'src/chrome/test/webdriver/commands/chrome_commands.cc',
        'src/chrome/test/webdriver/commands/command.cc',
        'src/chrome/test/webdriver/commands/cookie_commands.cc',
        'src/chrome/test/webdriver/commands/create_session.cc',
        'src/chrome/test/webdriver/commands/execute_async_script_command.cc',
        'src/chrome/test/webdriver/commands/execute_command.cc',
        'src/chrome/test/webdriver/commands/file_upload_command.cc',
        'src/chrome/test/webdriver/commands/find_element_commands.cc',
        'src/chrome/test/webdriver/commands/html5_location_commands.cc',
        'src/chrome/test/webdriver/commands/html5_storage_commands.cc',
        'src/chrome/test/webdriver/commands/keys_command.cc',
        'src/chrome/test/webdriver/commands/log_command.cc',
        'src/chrome/test/webdriver/commands/mouse_commands.cc',
        'src/chrome/test/webdriver/commands/navigate_commands.cc',
        'src/chrome/test/webdriver/commands/response.cc',
        'src/chrome/test/webdriver/commands/screenshot_command.cc',
        'src/chrome/test/webdriver/commands/sessions.cc',
        'src/chrome/test/webdriver/commands/session_with_id.cc',
        'src/chrome/test/webdriver/commands/set_timeout_commands.cc',
        'src/chrome/test/webdriver/commands/source_command.cc',
        'src/chrome/test/webdriver/commands/target_locator_commands.cc',
        'src/chrome/test/webdriver/commands/title_command.cc',
        'src/chrome/test/webdriver/commands/url_command.cc',
        'src/chrome/test/webdriver/commands/webdriver_command.cc',
        'src/chrome/test/webdriver/commands/webelement_commands.cc',
        'src/chrome/test/webdriver/commands/window_commands.cc',
        'src/chrome/test/webdriver/frame_path.cc',
        'src/chrome/test/webdriver/http_response.cc',
        'src/chrome/test/webdriver/webdriver_automation.h',
        'src/chrome/test/webdriver/webdriver_automation.cc',
        'src/chrome/test/webdriver/webdriver_basic_types.cc',
        'src/chrome/test/webdriver/webdriver_capabilities_parser.cc',
        'src/chrome/test/webdriver/webdriver_dispatch.cc',
        'src/chrome/test/webdriver/webdriver_element_id.cc',
        'src/chrome/test/webdriver/webdriver_error.cc',
        'src/chrome/test/webdriver/webdriver_key_converter.cc',
        'src/chrome/test/webdriver/webdriver_logging.cc',
        'src/chrome/test/webdriver/webdriver_server.cc',
        'src/chrome/test/webdriver/webdriver_session.cc',
        'src/chrome/test/webdriver/webdriver_session_manager.cc',
        'src/chrome/test/webdriver/webdriver_switches.cc',
        'src/chrome/test/webdriver/webdriver_util.cc',
        'src/content/public/common/content_switches.cc',
        'src/moc_qtaskrunner.cc',
        'src/moc_qwebviewext.cc',
        'src/moc_webdriver_automation.cc',
        'src/net/base/file_stream.cc',
        'src/net/base/file_stream_metrics.cc',
        'src/net/base/file_stream_metrics_posix.cc',
        'src/net/base/file_stream_net_log_parameters.cc',
        'src/net/base/file_stream_posix.cc',
        'src/net/base/io_buffer.cc',
        'src/net/base/net_errors.cc',
        'src/net/base/net_errors_posix.cc',
        'src/net/base/net_log.cc',
        'src/qtaskrunner.h',
        'src/qtaskrunner.cc',
        'src/qwebviewext.h',
        'src/qwebviewext.cc',
        'src/third_party/webdriver/atoms.cc',
        'src/third_party/zlib/adler32.c',
        'src/third_party/zlib/contrib/minizip/ioapi.c',
        'src/third_party/zlib/contrib/minizip/unzip.c',
        'src/third_party/zlib/crc32.c',
        'src/third_party/zlib/infback.c',
        'src/third_party/zlib/inffast.c',
        'src/third_party/zlib/inflate.c',
        'src/third_party/zlib/inftrees.c',
        'src/third_party/zlib/zutil.c',
        'src/viewfactory.cc',
      ],

      'rules': [ {
        'rule_name': 'generate_moc',
        'extension': 'h',
        'outputs': [ '<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cc' ],
        'action': [ '<(QT_BIN_PATH)/moc', '<(RULE_INPUT_PATH)', '-o', 'src/moc_<(RULE_INPUT_ROOT).cc' ],
        'message': 'Generating <(RULE_INPUT_ROOT).cc.',
      } ],

      'conditions': [
        [ '<(WD_BUILD_MONGOOSE) == 1', {
          'sources': [
            'src/third_party/mongoose/mongoose.c',
          ],
        } ],

        [ '<(WD_BUILD_MODP_B64) == 1', {
          'sources': [
            'src/third_party/modp_b64/modp_b64.cc',
          ],
        } ],
      ],
    }, {
      'target_name': 'WebDriverShared',
      'type': 'shared_library',

      'product_name': 'WebDriver',

      'dependencies': [
        'WebDriver',
      ],
    },
  ],
}
