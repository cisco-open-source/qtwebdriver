# compiler
CXX = g++
CC = gcc

# makedepend flags
DFLAGS =

OUTLIBDIR = ../lib
OUTTEST_DIR = ../bin

# output libraries
OUT_STATIC = $(OUTLIBDIR)/libWebDriver.a
OUT_SHARED = $(OUTLIBDIR)/libWebDriver.so

# output test exe
OUT_TEST = $(OUTTEST_DIR)/WebDriver

# QT library and include path
QT_LIB_PATH = /opt/qt4_nds/linux2_2/release/lib/
QT_INC_PATH = /opt/qt4_nds/linux2_2/release/include/

# library paths
INCLUDES       = -I. -I../inc/ -I$(QT_INC_PATH)
LINK          = $(CXX)
LFLAGS        = -shared -Wl,-O1 
LIBS          = -ldl -lrt -L/usr/X11R6/lib -lpthread -L$(QT_LIB_PATH) -lQtWebKit -lQtCore -lQtGui -lQtNetwork
DEFINES       = -DOS_POSIX -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -Wall -W -D_REENTRANT -fPIC $(DEFINES)

# additional compiler flags for debug mode
ifeq ($(mode),debug)
	CFLAGS += -g -DNDEBUG
	CXXFLAGS += -g -DNDEBUG
endif

# directory for object files
OBJDIR=obj

# library paths
vpath = %.c %.cpp %.cc .. base third_party/mongoose chrome/test/webdriver/ chrome/test/webdriver/commands chrome/common/ base/json chrome/test/automation/ \
	base/third_party/dmg_fp base/third_party/icu base/third_party/dynamic_annotations base/threading/ base/memory/ base/third_party/nspr \
	base/synchronization base/metrics base/profiler third_party/modp_b64 net/base content/public/common/ third_party/webdriver \
	third_party/zlib/contrib/minizip base/debug base/nix base/third_party/xdg_user_dirs third_party/zlib/

# source files
SRC = third_party/mongoose/mongoose.c \
		chrome/test/webdriver/webdriver_server.cc \
		chrome/test/webdriver/webdriver_dispatch.cc \
		chrome/test/webdriver/commands/response.cc \
		chrome/test/webdriver/webdriver_error.cc \
		chrome/test/webdriver/commands/command.cc \
		chrome/test/webdriver/http_response.cc \
		base/values.cc \
		chrome/test/webdriver/webdriver_automation.cc \
		chrome/test/webdriver/webdriver_session.cc \
		chrome/test/webdriver/webdriver_session_manager.cc \
		chrome/test/webdriver/webdriver_util.cc \
		chrome/test/automation/automation_json_requests.cc \
		chrome/test/webdriver/frame_path.cc \
		chrome/common/automation_id.cc \
		base/stringprintf.cc \
		base/string_number_conversions.cc \
		base/utf_string_conversions.cc \
		base/third_party/dmg_fp/g_fmt.cc \
		base/third_party/dmg_fp/dtoa.cc \
		base/utf_string_conversion_utils.cc \
		base/third_party/icu/icu_utf.cc \
		base/string16.cc \
		base/json/json_writer.cc \
		base/json/json_reader.cc \
		base/json/json_parser.cc \
		base/json/string_escape.cc \
		base/string_piece.cc \
		chrome/test/webdriver/webdriver_element_id.cc \
		chrome/test/automation/value_conversion_traits.cc \
		chrome/test/webdriver/webdriver_basic_types.cc \
		chrome/test/webdriver/commands/create_session.cc \
		base/third_party/dynamic_annotations/dynamic_annotations.c \
		base/task_runner.cc \
		base/threading/post_task_and_reply_impl.cc \
		base/memory/ref_counted.cc \
		base/callback_internal.cc \
		base/message_loop_proxy.cc \
		base/atomicops_internals_x86_gcc.cc \
		base/threading/thread_collision_warner.cc \
		base/threading/platform_thread_posix.cc \
		base/time.cc \
		base/third_party/nspr/prtime.cc \
		base/time_posix.cc \
		base/message_loop_proxy_impl.cc \
		base/threading/thread_restrictions.cc \
		base/lazy_instance.cc \
		base/threading/thread_local_posix.cc \
		base/tracked_objects.cc \
		base/synchronization/lock_impl_posix.cc \
		base/synchronization/lock.cc \
		base/message_loop.cc \
		base/run_loop.cc \
		base/pending_task.cc \
		base/metrics/statistics_recorder.cc \
		base/metrics/histogram.cc \
		base/memory/weak_ptr.cc \
		base/tracking_info.cc \
		base/thread_task_runner_handle.cc \
		base/message_pump_default.cc \
		base/pickle.cc \
		base/metrics/histogram_base.cc \
		base/metrics/sample_vector.cc \
		base/metrics/bucket_ranges.cc \
		base/location.cc \
		base/threading/thread_local_storage_posix.cc \
		base/profiler/tracked_time.cc \
		base/profiler/alternate_timer.cc \
		base/threading/thread_checker_impl.cc \
		base/synchronization/waitable_event_posix.cc \
		base/synchronization/condition_variable_posix.cc \
		base/message_pump.cc \
		base/metrics/histogram_samples.cc \
		base/threading/thread.cc \
		base/profiler/scoped_profile.cc \
		base/memory/singleton.cc \
		base/at_exit.cc \
		chrome/common/automation_constants.cc \
		chrome/test/webdriver/webdriver_capabilities_parser.cc \
		chrome/test/webdriver/webdriver_logging.cc \
		base/file_util.cc \
		base/logging.cc \
		base/file_util_posix.cc \
		base/file_path.cc \
		base/string_util.cc \
		base/platform_file_posix.cc \
		base/platform_file.cc \
		base/safe_strerror_posix.cc \
		base/sys_string_conversions_posix.cc \
		base/base64.cc \
		base/command_line.cc \
		third_party/modp_b64/modp_b64.cc \
		net/base/net_log.cc \
		net/base/file_stream.cc \
		net/base/net_errors.cc \
		net/base/net_errors_posix.cc \
		net/base/io_buffer.cc \
		net/base/file_stream_net_log_parameters.cc \
		net/base/file_stream_metrics.cc \
		net/base/file_stream_metrics_posix.cc \
		net/base/file_stream_posix.cc \
		base/threading/worker_pool.cc \
		base/threading/worker_pool_posix.cc \
		base/string_split.cc \
		chrome/test/webdriver/commands/webdriver_command.cc \
		chrome/test/webdriver/commands/session_with_id.cc \
		chrome/common/chrome_constants.cc \
		chrome/common/chrome_switches.cc \
		base/base_switches.cc \
		content/public/common/content_switches.cc \
		base/scoped_temp_dir.cc \
		chrome/test/webdriver/commands/url_command.cc \
		chrome/test/webdriver/commands/window_commands.cc \
		chrome/test/webdriver/commands/webelement_commands.cc \
		chrome/test/webdriver/commands/title_command.cc \
		chrome/test/webdriver/commands/target_locator_commands.cc \
		chrome/test/webdriver/commands/source_command.cc \
		chrome/test/webdriver/commands/set_timeout_commands.cc \
		chrome/test/webdriver/commands/screenshot_command.cc \
		chrome/test/webdriver/commands/navigate_commands.cc \
		chrome/test/webdriver/commands/mouse_commands.cc \
		chrome/test/webdriver/commands/log_command.cc \
		chrome/test/webdriver/commands/keys_command.cc \
		chrome/test/webdriver/commands/html5_storage_commands.cc \
		chrome/test/webdriver/commands/html5_location_commands.cc \
		chrome/test/webdriver/commands/find_element_commands.cc \
		chrome/test/webdriver/commands/file_upload_command.cc \
		chrome/test/webdriver/commands/execute_command.cc \
		chrome/test/webdriver/commands/execute_async_script_command.cc \
		chrome/test/webdriver/commands/cookie_commands.cc \
		chrome/test/webdriver/commands/chrome_commands.cc \
		chrome/test/webdriver/commands/browser_connection_commands.cc \
		chrome/test/webdriver/commands/appcache_status_command.cc \
		chrome/test/webdriver/commands/alert_commands.cc \
		chrome/test/webdriver/commands/sessions.cc \
		third_party/webdriver/atoms.cc \
		base/rand_util.cc \
		base/rand_util_posix.cc \
		chrome/common/zip.cc \
		chrome/common/zip_reader.cc \
		third_party/zlib/contrib/minizip/unzip.c \
		chrome/common/zip_internal.cc \
		third_party/zlib/contrib/minizip/ioapi.c \
		qtaskrunner.cc \
		chrome/common/chrome_version_info.cc \
		chrome/common/chrome_version_info_posix.cc \
		base/sys_info_posix.cc \
		base/sys_info_linux.cc \
		base/debug/alias.cc \
		base/debug/debugger.cc \
		base/debug/debugger_posix.cc \
		base/timer.cc \
		base/path_service.cc \
		base/base_paths.cc \
		base/base_paths_posix.cc \
		base/environment.cc \
		base/nix/xdg_util.cc \
		base/third_party/xdg_user_dirs/xdg_user_dir_lookup.cc \
		chrome/test/webdriver/webdriver_key_converter.cc \
		third_party/zlib/inflate.c \
		third_party/zlib/zutil.c \
		third_party/zlib/inftrees.c \
		third_party/zlib/inffast.c \
		third_party/zlib/infback.c \
		third_party/zlib/crc32.c \
		third_party/zlib/adler32.c \
		chrome/test/webdriver/webdriver_switches.cc \
		qwebviewext.cc \
		moc_webdriver_automation.cc \
		moc_qtaskrunner.cc \
		moc_qwebviewext.cc \
		Test/main.cc

# Object files
OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(patsubst %.c, $(OBJDIR)/%.o, $(patsubst %.cc, $(OBJDIR)/%.o,  $(SRC))))

OBJ_TEST = $(OBJDIR)/Test/main.o

default: $(OUT_STATIC) $(OUT_SHARED) $(OUT_TEST)

$(OBJDIR)/%.o : %.c
	@mkdir -p  $(OBJDIR)/$(dir $<)
	$(CC) -c $(INCLUDES) $(CFLAGS) $< -o $@
$(OBJDIR)/%.o : %.cpp
	@mkdir -p $(OBJDIR)/$(dir $<)
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) $< -o $@
$(OBJDIR)/%.o : %.cc
	@mkdir -p $(OBJDIR)/$(dir $<)
	$(CXX) -c $(INCLUDES) $(CXXFLAGS) $< -o $@

$(OUT_STATIC): $(OBJ)
	ar rcs $(OUT_STATIC) $(OBJ)


$(OUT_SHARED): $(OBJ)
	$(LINK) $(LFLAGS) -o $(OUT_SHARED) $(OBJ) $(LIBS)

$(OUT_TEST): $(OBJ_TEST)
	@mkdir -p $(OUTTEST_DIR)
	$(LINK) -o $(OUT_TEST) $(OBJ_TEST) $(LIBS) $(OUT_STATIC)

	
.PHONY: clean
clean:
	rm -f $(OBJ) $(OUT_STATIC) $(OUT_SHARED) Makefile.bak

