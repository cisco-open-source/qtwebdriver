SOURCES += \
$$SRC_DIR/base/string16.cc \
$$SRC_DIR/base/time_posix.cc \
$$SRC_DIR/base/base_paths_posix.cc \
$$SRC_DIR/base/debug/debugger_posix.cc \
$$SRC_DIR/base/platform_file_posix.cc \
$$SRC_DIR/base/rand_util_posix.cc \
$$SRC_DIR/base/safe_strerror_posix.cc \
$$SRC_DIR/base/synchronization/condition_variable_posix.cc \
$$SRC_DIR/base/synchronization/lock_impl_posix.cc \
$$SRC_DIR/base/synchronization/waitable_event_posix.cc \
$$SRC_DIR/base/sys_info_posix.cc \
$$SRC_DIR/base/threading/platform_thread_posix.cc \
$$SRC_DIR/base/threading/thread_local_posix.cc \
$$SRC_DIR/base/threading/thread_local_storage_posix.cc \
$$SRC_DIR/base/threading/worker_pool_posix.cc \
$$SRC_DIR/net/base/file_stream_metrics_posix.cc \
$$SRC_DIR/net/base/file_stream_posix.cc \
$$SRC_DIR/net/base/net_errors_posix.cc \
$$SRC_DIR/base/file_util_posix.cc \
$$SRC_DIR/base/mac/mac_logging.cc \
$$SRC_DIR/base/mac/mach_logging.cc \
$$SRC_DIR/base/mac/scoped_mach_port.cc \
$$SRC_DIR/base/time_mac.cc \
$$SRC_DIR/base/sys_info_mac.cc

OBJECTIVE_SOURCES += \
$$SRC_DIR/base/sys_string_conversions_mac.mm \
$$SRC_DIR/base/base_paths_mac.mm \
$$SRC_DIR/base/file_util_mac.mm \
$$SRC_DIR/base/message_pump_mac.mm \
$$SRC_DIR/base/scoped_nsautorelease_pool.mm \
$$SRC_DIR/base/mac/foundation_util.mm \
$$SRC_DIR/base/mac/bundle_locations.mm \
$$SRC_DIR/base/threading/platform_thread_mac.mm