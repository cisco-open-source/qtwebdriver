// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_LOGGING_H_
#define WEBDRIVER_WEBDRIVER_LOGGING_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/synchronization/lock.h"
#include "base/values.h"

namespace base {
class Time;
}

namespace webdriver {

/// @enum LogLevel WebDriver logging levels.
enum LogLevel {
    kOffLogLevel = 1200,
    kSevereLogLevel = 1000,
    kWarningLogLevel = 900,
    kInfoLogLevel = 800,
    kFineLogLevel = 500,
    kAllLogLevel = -1000
};

// |name| should be a webdriver log level, such as "INFO", "SEVERE", etc.
LogLevel LogLevelFromString(const std::string& name);
std::string LogLevelToString(LogLevel level);

/// Represents a type/source of a WebDriver log.
class LogType {
public:
    /// @enum Type log source
    enum Type {
        kInvalid = -1,  /// invalid type
        kDriver,        /// driver
        kBrowser,       /// browser (console.log)
        kPerformance,   /// performance
        kNum            /// number of sources
    };

    static bool FromString(const std::string& name, LogType* log_type);

    LogType();
    explicit LogType(Type type);
    ~LogType();

    std::string ToString() const;
    Type type() const;

private:
    Type type_;
};

/// base class for log handlers
class LogHandler {
public:
    /// default constructor
    LogHandler();
    virtual ~LogHandler();

    /// add single log item
    /// @param level log level
    /// @param time timestamp
    /// @param message log message
    virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(LogHandler);
};

/// file log handler
class FileLog : public LogHandler {
public:
    static void SetGlobalLog(FileLog* log);
    static FileLog* Get();

    /// Creates a log file with the given name in the current directory.
    /// If the current directory is not writable, the system's temp directory
    /// is used.
    /// @param log_name file name
    /// @param level log level
    static FileLog* CreateFileLog(const FilePath::StringType& log_name,
                                LogLevel level);

    /// Creates a log file at the given path.
    FileLog(const FilePath& path, LogLevel level);

    virtual ~FileLog();

    virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

    bool GetLogContents(std::string* contents) const;

    /// Returns whether the log refers to an open file.
    bool IsOpen() const;

    void set_min_log_level(LogLevel level);

    /// Returns the path of the log file. The file is not guaranteed to exist.
    const FilePath& path() const;

private:
    static FileLog* singleton_;

    FilePath path_;
    file_util::ScopedFILE file_;
    base::Lock lock_;

    LogLevel min_log_level_;

    DISALLOW_COPY_AND_ASSIGN(FileLog);
};

/// stdout log handler
class StdOutLog : public LogHandler {
public:
    static void SetGlobalLog(StdOutLog* log);
    static StdOutLog* Get();

    /// Creates a log 
    StdOutLog(LogLevel level);

    virtual ~StdOutLog();

    virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

    void set_min_log_level(LogLevel level);

private:
    static StdOutLog* singleton_;

    LogLevel min_log_level_;

    DISALLOW_COPY_AND_ASSIGN(StdOutLog);
};

class InMemoryLog : public LogHandler {
public:
    InMemoryLog();
    virtual ~InMemoryLog();

    virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

    const base::ListValue* entries_list() const;
    void clear_entries_list();

private:
    base::ListValue entries_list_;
    base::Lock entries_lock_;

    DISALLOW_COPY_AND_ASSIGN(InMemoryLog);
};

/// Performance log handler.<br>
/// <b>Limitation:</b> currently we can collect Performance Logs only for QWebViewExt.<br>
/// If you need receive perf logs for other QWebViews or their subclasses,
/// you should change the network access manager before the QWebPage has used it
/// (in other cases the results of doing this are undefined) :
/// @code
/// QWebView* view = new QWebView;;
/// QNetworkAccessManager*  manager = new QNetworkAccessManagerTracer(session, view->page());
/// view->page()->setNetworkAccessManager(manager);
/// @endcode
/// before the first call to the page. After that you can connect to the window.
/// <p>In other case you can change manager in constructor custom webviews like QWebViewExt.
/// Then you can register and use it as usually :
/// @code
/// webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
/// webCreator->RegisterViewClass<CustomWebView>("CustomWebView");
/// webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);
/// @endcode
/// <p>If your CustomWebView class is overriding createWindow method of QWebView,
/// you should ensure that new windows will use needed network access manager with logs support.

class PerfLog : public InMemoryLog {
public:
    PerfLog();
    virtual ~PerfLog();
    virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

    void set_min_log_level(LogLevel level);
    LogLevel min_log_level();

private:
    LogLevel min_log_level_;

    DISALLOW_COPY_AND_ASSIGN(PerfLog);
};

/// Forwards logging messages to added logs.
class Logger {
public:
    Logger();
    explicit Logger(LogLevel level);
    ~Logger();

    void Log(LogLevel level, const std::string& message) const;
    void AddHandler(LogHandler* log_handler);

    void set_min_log_level(LogLevel level);

private:
    std::vector<LogHandler*> handlers_;
    LogLevel min_log_level_;
};


/// Logs some messages int oglobal FileLog and StdOutLog
class GlobalLogger {
public:
    static void Log(LogLevel level, const std::string& message);
    static void set_min_log_level(LogLevel level);
private:
    GlobalLogger();
    static LogLevel min_log_level_;    
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_LOGGING_H_
