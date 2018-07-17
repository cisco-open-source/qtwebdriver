HEADERS += \
$$INC_DIR/extension_qt/q_session_lifecycle_actions.h \
$$INC_DIR/extension_qt/q_view_executor.h \
$$INC_DIR/extension_qt/q_view_runner.h \
#$$INC_DIR/extension_qt/touch_device_manager.h \
$$INC_DIR/extension_qt/uinput_event_dispatcher.h \
$$INC_DIR/extension_qt/uinput_manager.h \
$$INC_DIR/extension_qt/vnc_event_dispatcher.h \
$$INC_DIR/extension_qt/vncclient.h \
$$INC_DIR/extension_qt/wd_event_dispatcher.h \
$$INC_DIR/extension_qt/widget_element_handle.h \
$$INC_DIR/extension_qt/widget_view_creator.h \
$$INC_DIR/extension_qt/widget_view_enumerator.h \
$$INC_DIR/extension_qt/widget_view_executor.h \
$$INC_DIR/extension_qt/widget_view_handle.h \
$$SRC_DIR/third_party/des/d3des.h \
$$SRC_DIR/webdriver/extension_qt/q_event_filter.h

SOURCES += \
#$$SRC_DIR/webdriver/extension_qt/touch_device_manager.cc \
$$SRC_DIR/webdriver/extension_qt/q_content_type_resolver.cc \
$$SRC_DIR/webdriver/extension_qt/q_view_runner.cc \
$$SRC_DIR/webdriver/extension_qt/q_proxy_parser.cc \
$$SRC_DIR/webdriver/extension_qt/q_key_converter.cc \
$$SRC_DIR/webdriver/extension_qt/q_session_lifecycle_actions.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_util.cc \
$$SRC_DIR/webdriver/extension_qt/common_util.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_handle.cc \
$$SRC_DIR/webdriver/extension_qt/widget_element_handle.cc \
$$SRC_DIR/webdriver/extension_qt/q_view_executor.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_creator.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_executor.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_enumerator.cc \
$$SRC_DIR/webdriver/extension_qt/widget_view_visualizer.cc \
$$SRC_DIR/webdriver/extension_qt/shutdown_command.cc \
$$SRC_DIR/webdriver/extension_qt/q_event_filter.cc \
$$SRC_DIR/vnc/vncclient.cc \
$$SRC_DIR/webdriver/extension_qt/uinput_manager.cc \
$$SRC_DIR/third_party/des/d3des.c \
$$SRC_DIR/vnc/vncserverparameters.cc \
$$SRC_DIR/webdriver/extension_qt/vnc_event_dispatcher.cc \
$$SRC_DIR/webdriver/extension_qt/wd_event_dispatcher.cc \
$$SRC_DIR/webdriver/extension_qt/uinput_event_dispatcher.cc
