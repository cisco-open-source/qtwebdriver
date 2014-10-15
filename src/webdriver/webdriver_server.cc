// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_server.h"
#include "webdriver_route_table.h"
#include "commands/command.h"
#include "commands/response.h"

#include "mongoose.h"

#include <iostream>

//#include "base/command_line.h"
//#include "base/format_macros.h"
#include "base/file_util.h"
#include "base/json/json_reader.h"
#include "base/memory/scoped_ptr.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/sys_info.h"
#include "base/string_number_conversions.h"
#include "http_response.h"
#include "webdriver_logging.h"
#include "webdriver_switches.h"
#include "webdriver_util.h"
#include "webdriver_session_manager.h"

#include "webdriver_route_patterns.h"
#include "url_command_wrapper.h"
#include "versioninfo.h"
#include "webdriver_switches.h"

#include "base/bind.h"
#include "webdriver_session.h"
#include "webdriver_view_enumerator.h"
#include "webdriver_view_executor.h"

namespace webdriver {

Server::Server()
    : options_(NULL),
      routeTable_(NULL),
      mg_ctx_(NULL),
      state_(STATE_UNCONFIGURED) {
}

Server::~Server() {}

int Server::Configure(const CommandLine &options) {
    int ret_val = 0;

    if (state_ != STATE_UNCONFIGURED) {
        std::cerr << "Init failure: can't init twice." << std::endl;    
        return 1;
    }

    options_.reset(new CommandLine(CommandLine::NO_PROGRAM));
    options_->InitFromArgv(options.argv());

    ret_val = ParseConfigToOptions();
    if (ret_val) {
        std::cerr << "Init failure: can't parse config file, skip options in config." << std::endl;
    }

    ret_val = InitLogging();
    if (ret_val) {
        std::cerr << "Init failure: can't init logging." << std::endl;
        return ret_val;
    }

    ret_val = InitMongooseOptions();
    if (ret_val) {
        std::cerr << "Init failure: can't init mongoose options." << std::endl;
        return ret_val;
    }

    if (options_->HasSwitch(webdriver::Switches::kUrlBase))
        url_base_ = options_->GetSwitchValueASCII(webdriver::Switches::kUrlBase);

    std::string driver_info = "*** Webdriver ****\nVersion:    "+ VersionInfo::Name() + "-" + VersionInfo::Version() +
                            "\nBuild Time: "+ VersionInfo::BuildDateTime() ;
    GlobalLogger::Log(kInfoLogLevel, driver_info);

    if (options.HasSwitch(webdriver::Switches::kWhiteList))
    {
        FilePath xmlPath(options_->GetSwitchValueNative(webdriver::Switches::kWhiteList));
        accessValidor.setWhiteList(xmlPath);
    }


    // set default route table
    routeTable_.reset(new DefaultRouteTable());

    state_ = STATE_IDLE;

    return 0;
}

int Server::Reset() {
    if (state_ == STATE_UNCONFIGURED)
        return 0; // nothing todo

    if (state_ != STATE_IDLE)
        return 1; // cant reset not idle 

    mg_options_.clear();
    routeTable_.reset(NULL);
    options_.reset(NULL);
    FileLog::SetGlobalLog(NULL);
    StdOutLog::SetGlobalLog(NULL);

    state_ = STATE_UNCONFIGURED;

    return 0;
}

int Server::SetRouteTable(RouteTable* routeTable) {
    if (state_ == STATE_IDLE) {
        routeTable_.reset(new RouteTable(*routeTable));
        return 0;
    }
    return 1;
}

int Server::Start() {
    if (state_ != STATE_IDLE) {
        return 1;
    }
    
    GlobalLogger::Log(kInfoLogLevel, "Server::Start()");

    {
        GlobalLogger::Log(kFineLogLevel, ">>>>>>>>>>>>> Server::Start routes: >>>>>>>>>>>");
        std::vector<std::string> routes = routeTable_->GetRoutes();
        std::vector<std::string>::const_iterator route;
        for (route = routes.begin(); route < routes.end(); ++route) {
            GlobalLogger::Log(kFineLogLevel, *route);
        }
        GlobalLogger::Log(kFineLogLevel, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    }

    scoped_array<const char*> opts(new const char*[mg_options_.size() + 1]);
    for (size_t i = 0; i < mg_options_.size(); ++i) {
        opts[i] = mg_options_[i].c_str();
    }
    opts[mg_options_.size()] = NULL;

    // Initialize SHTTPD context.
    mg_ctx_ = mg_start((mg_callback_t)&ProcessHttpRequestCb,
                        this,
                        opts.get());
    if (mg_ctx_ == NULL) {
        std::cerr << "Port already in use. Exiting..." << std::endl;
#if defined(OS_WIN)
        return WSAEADDRINUSE;
#else
        return EADDRINUSE;
#endif
    }

    state_ = STATE_RUNNING;

    return 0;
}

int Server::Stop(bool force) {
    typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;        

    if (state_ != STATE_RUNNING)
        return 0; // nothing todo

    // get all sessions and terminate
    std::map<std::string, Session*> sessions = SessionManager::GetInstance()->GetSessions();
    std::map<std::string, Session*>::iterator sessionsItr = sessions.begin();

    GlobalLogger::Log(kInfoLogLevel, "Server::Stop().");

    if ((false == force) && (!sessions.empty())) {
        GlobalLogger::Log(kInfoLogLevel, "Server::Stop() force = false, still sessions are active, failed.");        
        return 1;
    }

    mg_stop(mg_ctx_);

    mg_ctx_ = NULL;

    while (sessionsItr != sessions.end()) {
        Session* session = (*sessionsItr).second;

        GlobalLogger::Log(kInfoLogLevel, "Server::Stop(): force close session("+session->id()+").");

        // close all views
        std::vector<ViewId> views;

        session->RunSessionTask(base::Bind(
            &ViewEnumerator::EnumerateViews,
            session,
            &views));

        for (size_t i = 0; i < views.size(); ++i) {
            Error* error = NULL;
            ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, views.at(i)));

            if (NULL == executor.get()) {
                GlobalLogger::Log(kWarningLogLevel, "Server::Stop(): cant terminate view(no executor), skip.");
                continue;
            }

            session->RunSessionTask(base::Bind(
                    &ViewCmdExecutor::Close,
                    base::Unretained(executor.get()),
                    &error));

            if (error) {
                GlobalLogger::Log(kWarningLogLevel, "Server::Stop(): cant terminate view(close failed), skip.");
                delete error;
                continue;
            }
        }

        // Session manages its own lifetime, so do not call delete.
        session->Terminate();

        ++sessionsItr;
    }

    state_ = STATE_IDLE;

    return 0;
}

// Maximum safe size of HTTP response message. Any larger than this,
// the message may not be transferred at all.
const size_t kMaxHttpMessageSize = 1024 * 1024 * 16;  // 16MB

// mongoose callback
void* Server::ProcessHttpRequestCb(int event_raised,
                         struct mg_connection* connection,
                         const struct mg_request_info* request_info) {
    bool handler_result_code = false;
    if (event_raised == MG_NEW_REQUEST) {
        handler_result_code =
            reinterpret_cast<Server*>(request_info->user_data)->
                ProcessHttpRequest(connection, request_info);
    }

    return reinterpret_cast<void*>(handler_result_code);
}

bool Server::ProcessHttpRequest(struct mg_connection* connection,
                        const struct mg_request_info* request_info) {
    std::vector<std::string> path_segments;
    base::DictionaryValue* parameters = NULL;
    Response response;
    std::string uri(request_info->uri);
    std::string method(request_info->request_method);
    std::string matched_route;

    // Overwrite mongoose's default handler for /favicon.ico to always return a
    // 204 response so we don't spam the logs with 404s.
    if (uri == "/favicon.ico") {
        SendNoContentResponse(connection, request_info);
        return true;
    }

    GlobalLogger::Log(kFineLogLevel, ">>>>> ProcessHttpRequest - got new request: "+
                        method + "  " + uri);

    // remove url_base from uri
    if (uri.length() >= url_base_.length())
        uri = uri.substr(url_base_.length());

    AbstractCommandCreator* cmdCreator = routeTable_->GetRouteForURL(uri, &matched_route);
    if (NULL == cmdCreator)
    {
        GlobalLogger::Log(kWarningLogLevel, "<<<<< ProcessHttpRequest - no route for url: " + uri);
        return false;
    }

    if (ParseRequestInfo(request_info,
                            connection,
                            &method,
                            &path_segments,
                            &parameters,
                            &response)) {
        Command* command = cmdCreator->create(path_segments, parameters);
        DispatchCommand(
                matched_route,
                command,
                method,
                &response);
    }
    SendResponse(connection,
                request_info->request_method,
                response);

    GlobalLogger::Log(kFineLogLevel, "<<<<< ProcessHttpRequest - processing finished: " + uri);
    
    return true;
}

void Server::SendNoContentResponse(struct mg_connection* connection,
                           const struct mg_request_info* request_info) {
    WriteHttpResponse(connection, HttpResponse(HttpResponse::kNoContent));
}

void Server::SendResponse(struct mg_connection* const connection,
                  const std::string& request_method,
                  const Response& response) {
    HttpResponse http_response;
    PrepareHttpResponse(request_method, response, &http_response);
    WriteHttpResponse(connection, http_response);
}

void Server::WriteHttpResponse(struct mg_connection* connection,
                       const HttpResponse& response) {
    HttpResponse modified_response(response);
    modified_response.AddHeader("connection", "close");
    std::string data;
    modified_response.GetData(&data);
    mg_write(connection, data.data(), data.length());
}

const CommandLine& Server::GetCommandLine() const
{
    return *(options_.get());
}

void Server::PrepareHttpResponse(const std::string& request_method,
                                 const Response& command_response,
                                 HttpResponse* const http_response) {
    ErrorCode status = command_response.GetStatus();

    // TODO(jleyba): kSeeOther, kBadRequest, kSessionNotFound,
    // and kMethodNotAllowed should be detected before creating
    // a command_response, and should thus not need conversion.
    if (status == kSeeOther) {
      const Value* const value = command_response.GetValue();
      std::string location;
      if (!value->GetAsString(&location)) {
          // This should never happen.
          http_response->set_status(HttpResponse::kInternalServerError);
          http_response->set_body("Unable to set 'Location' header: response "
                                  "value is not a string: " +
                                  command_response.ToJSON());
          return;
      }
      http_response->AddHeader("Location", location);
    }
    if ((request_method == "OPTIONS" && status == kSuccess) ||
        status == kMethodNotAllowed) {
      const Value* const value = command_response.GetValue();
      if (!value->IsType(Value::TYPE_LIST)) {
          // This should never happen.
          http_response->set_status(HttpResponse::kInternalServerError);
          http_response->set_body(
                      "Unable to set 'Allow' header: response value was "
                      "not a list of strings: " + command_response.ToJSON());
          return;
      }

      const ListValue* const list_value =
              static_cast<const ListValue* const>(value);
      std::vector<std::string> allowed_methods;
      for (size_t i = 0; i < list_value->GetSize(); ++i) {
          std::string method;
          if (list_value->GetString(i, &method)) {
              allowed_methods.push_back(method);
          } else {
              // This should never happen.
              http_response->set_status(HttpResponse::kInternalServerError);
              http_response->set_body(
                          "Unable to set 'Allow' header: response value was "
                          "not a list of strings: " + command_response.ToJSON());
              return;
          }
      }
      http_response->AddHeader("Allow", JoinString(allowed_methods, ','));
    }

    switch (status) {
    case kSuccess:
        http_response->set_status(HttpResponse::kOk);
        break;

    case kSeeOther:
        http_response->set_status(HttpResponse::kSeeOther);
        break;

    case kBadRequest:
    case kSessionNotFound:
        http_response->set_status(status);
        break;

    case kMethodNotAllowed:
        http_response->set_status(HttpResponse::kMethodNotAllowed);
        break;

    case kCommandNotSupported:
        http_response->set_status(HttpResponse::kNotImplemented);
        break;

    // All other errors should be treated as generic 500s. The client
    // will be responsible for inspecting the message body for details.
    case kInternalServerError:
    default:
        http_response->set_status(HttpResponse::kInternalServerError);
        break;
    }

    http_response->SetMimeType("application/json; charset=utf-8");

    // Sets access control headers to allow cross-origin resource sharing from any origin.
    http_response->AddHeader("Access-Control-Allow-Origin", "*");
    http_response->AddHeader("Access-Control-Allow-Methods", "DELETE,GET,HEAD,POST");
    http_response->AddHeader("Access-Control-Allow-Headers", "Accept,Content-Type");

    http_response->set_body(command_response.ToJSON());
}	

bool Server::ParseRequestInfo(const struct mg_request_info* const request_info,
                      struct mg_connection* const connection,
                      std::string* method,
                      std::vector<std::string>* path_segments,
                      DictionaryValue** parameters,
                      Response* const response) {
    *method = request_info->request_method;
    if (*method == "HEAD")
        *method = "GET";
    else if (*method == "PUT")
        *method = "POST";

    std::string uri(request_info->uri);
    if (uri.length() >= url_base_.length())
        uri = uri.substr(url_base_.length());

    base::SplitString(uri, '/', path_segments);

    if (!accessValidor.isAllowed(request_info->remote_ip, uri, *method))
    {
        response->SetError(new Error(
                               kUnknownError,
                               "Command is forbidden for this origin"));
        return false;
    }

    if (*method == "POST") {
        std::string json;
        ReadRequestBody(request_info, connection, &json);
        if (json.length() > 0) {
            std::string error_msg;
            scoped_ptr<Value> params(base::JSONReader::ReadAndReturnError(
                                         json, base::JSON_ALLOW_TRAILING_COMMAS, NULL, &error_msg));
            if (!params.get()) {
                response->SetError(new Error(
                                       kBadRequest,
                                       "Failed to parse command data: " + error_msg +
                                       "\n  Data: " + json));
                return false;
            }
            if (!params->IsType(Value::TYPE_DICTIONARY)) {
                response->SetError(new Error(
                                       kBadRequest,
                                       "Data passed in URL must be a dictionary. Data: " + json));
                return false;
            }
            *parameters = static_cast<DictionaryValue*>(params.release());
        }
    }
    return true;
}

void Server::ReadRequestBody(const struct mg_request_info* const request_info,
                     struct mg_connection* const connection,
                     std::string* request_body) {
    int content_length = 0;
    // 64 maximum header count hard-coded in mongoose.h
    for (int header_index = 0; header_index < 64; ++header_index) {
        if (request_info->http_headers[header_index].name == NULL) {
            break;
        }
        if (strcmp(request_info->http_headers[header_index].name,
                   "Content-Length") == 0) {
            content_length = atoi(request_info->http_headers[header_index].value);
            break;
        }
    }
    if (content_length > 0) {
        request_body->resize(content_length);
        int bytes_read = 0;
        while (bytes_read < content_length) {
            bytes_read += mg_read(connection,
                                  &(*request_body)[bytes_read],
                                  content_length - bytes_read);
        }
    }
}

const RouteTable& Server::GetRouteTable() const
{
    return *routeTable_;
}

const std::string& Server::url_base() const
{
    return url_base_;
}

Server* Server::GetInstance()
{
    return Singleton<Server>::get();
}

void Server::DispatchCommand(const std::string& matched_route,
                             Command* command_ptr,
                             const std::string& method,
                             Response* response) {
#if defined(OS_MACOSX)
    // An autorelease pool must exist on any thread where Objective C is used,
    // even implicitly. Otherwise the warning:
    //   "Objects autoreleased with no pool in place."
    // is printed for every object deallocated.  Since every incoming command to
    // chrome driver is allocated a new thread, the release pool is declared here.
    base::mac::ScopedNSAutoreleasePool autorelease_pool;
#endif

    if (matched_route == CommandRoutes::kUrlCmd) {
        command_ptr = new UrlCommandWrapper(command_ptr);
    }

    CHECK(method == "GET" ||
          method == "POST" ||
          method == "DELETE" ||
          method == "OPTIONS");
    scoped_ptr<Command> command(command_ptr);

    if ((method == "GET" && !command->DoesGet()) ||
        (method == "POST" && !command->DoesPost()) ||
        (method == "DELETE" && !command->DoesDelete())) {
        response->SetStatus(kMethodNotAllowed);
        response->SetValue(ListCommandSupportedMethods(*command));
        return;
    }

    if (method == "OPTIONS") {
        response->SetStatus(kSuccess);
        response->SetValue(ListCommandSupportedMethods(*command));
        return;
    }

    if (!command->Init(response))
        return;

    if (method == "POST") {
        command->ExecutePost(response);
    } else if (method == "GET") {
        command->ExecuteGet(response);
    } else if (method == "DELETE") {
        command->ExecuteDelete(response);
    } else {
        NOTREACHED();
    }
    command->Finish(response);
}

ListValue* Server::ListCommandSupportedMethods(const Command& command) {
    ListValue* methods = new ListValue;
    if (command.DoesPost())
        methods->Append(Value::CreateStringValue("POST"));
    if (command.DoesGet()) {
        methods->Append(Value::CreateStringValue("GET"));
        methods->Append(Value::CreateStringValue("HEAD"));
    }
    if (command.DoesDelete())
        methods->Append(Value::CreateStringValue("DELETE"));
    return methods;
}

int Server::InitMongooseOptions() {
    std::string port = "9517";
    std::string root = "./web";
    int http_threads = 4;

    if (options_->HasSwitch(webdriver::Switches::kPort))
        port = options_->GetSwitchValueASCII(webdriver::Switches::kPort);
    // The 'root' flag allows the user to specify a location to serve files from.
    // If it is not given, a callback will be registered to forbid all file
    // requests.
    if (options_->HasSwitch(webdriver::Switches::kRoot))
        root = options_->GetSwitchValueASCII(webdriver::Switches::kRoot);
    if (options_->HasSwitch(webdriver::Switches::kHttpThread)) {
        if (!base::StringToInt(options_->GetSwitchValueASCII(webdriver::Switches::kHttpThread),
                           &http_threads)) {
            GlobalLogger::Log(kSevereLogLevel, "'http-threads' option must be an integer");
            return 1;
        }
    }

    mg_options_.push_back("listening_ports");
    mg_options_.push_back(port);
    mg_options_.push_back("num_threads");
    mg_options_.push_back(base::IntToString(http_threads));
    if (!root.empty()) {
        mg_options_.push_back("document_root");
        mg_options_.push_back(root);

        if (!file_util::PathExists(FilePath::FromUTF8Unsafe(root))) {
            GlobalLogger::Log(kSevereLogLevel, "Root '" + root + "' does not exist!");
        }
    }
    mg_options_.push_back("extra_mime_types");
    mg_options_.push_back(".xhtml=application/xhtml+xml,.qml=text/x-qml");

    ParseMongooseConfig();

    return 0;
}

int Server::InitLogging() {
    FilePath log_path;

    if (options_->HasSwitch(webdriver::Switches::kLogPath))
        log_path = options_->GetSwitchValuePath(webdriver::Switches::kLogPath);

    // Init global file log.
    FileLog* fileLog;

    if (log_path.empty()) {
        fileLog = FileLog::CreateFileLog(FILE_PATH_LITERAL("webdriver.log"), kAllLogLevel);
    } else {
        fileLog = new FileLog(log_path, kAllLogLevel);
    }

    if (NULL == fileLog || !fileLog->IsOpen()) {
        std::cerr << "ERROR: InitLogging failed.";
        return 1;
    }

    FileLog::SetGlobalLog(fileLog);

    // Init global stdout log.
    StdOutLog* stdLog = new StdOutLog(kWarningLogLevel);
    if (NULL == stdLog) {
        std::cerr << "ERROR: InitLogging failed.";
        return 1;
    }

    StdOutLog::SetGlobalLog(stdLog);

    if (options_->HasSwitch(webdriver::Switches::kVerbose)) {
        stdLog->set_min_log_level(kAllLogLevel);
    }

    // check if silence mode
    if (options_->HasSwitch(webdriver::Switches::kSilence)) {
        stdLog->set_min_log_level(kOffLogLevel);
    }

    return 0;
}

int Server::ParseConfigToOptions() {
    if (options_->HasSwitch(webdriver::Switches::kConfig))
    {
        //parse json config file and set value
        std::string config_json;
        FilePath configPath(options_->GetSwitchValueNative(webdriver::Switches::kConfig));

        if (file_util::ReadFileToString(configPath, &config_json))
        {
            scoped_ptr<Value> value(base::JSONReader::ReadAndReturnError(
                config_json, base::JSON_ALLOW_TRAILING_COMMAS, NULL, NULL));
            if (!value.get())
            {
                std::cout << "Failed to parse config file" << std::endl;
                return 1;
            }

            if (value->GetType() != Value::TYPE_DICTIONARY)
            {
                std::cerr << "Config file: result of parsing json is not a dictionary " + JsonStringify(value.get()) << std::endl;
                return 2;
            }

            DictionaryValue* result_dict = static_cast<DictionaryValue*>(value.get());

            int port;
            std::string root;
            std::string url_base;
            int http_threads;
            std::string log_path;
            if (result_dict->GetInteger(webdriver::Switches::kPort, &port))
                options_->AppendSwitchASCII(webdriver::Switches::kPort, base::IntToString(port));
            if (result_dict->GetString(webdriver::Switches::kRoot, &root))
                options_->AppendSwitchASCII(webdriver::Switches::kRoot, root);
            if (result_dict->GetString(webdriver::Switches::kUrlBase, &url_base))
                options_->AppendSwitchASCII(webdriver::Switches::kUrlBase, url_base);
            if (result_dict->GetInteger(webdriver::Switches::kHttpThread, &http_threads))
                options_->AppendSwitchASCII(webdriver::Switches::kHttpThread, base::IntToString(http_threads));
            if (result_dict->GetString(webdriver::Switches::kLogPath, &log_path))
                options_->AppendSwitchASCII(webdriver::Switches::kLogPath, log_path);

            return 0;
        }
        else
        {
            std::cerr << "can't read config file" << std::endl;
            return 1;
        }
    }

    return 0;
}

int Server::ParseMongooseConfig() {
    if (options_->HasSwitch(webdriver::Switches::kWebServerCfg))
    {
        //parse json config file and set value
        std::string mongoose_cfg;
        FilePath configPath(options_->GetSwitchValueNative(webdriver::Switches::kWebServerCfg));

        if (file_util::ReadFileToString(configPath, &mongoose_cfg))
        {
            scoped_ptr<Value> value(base::JSONReader::ReadAndReturnError(
                mongoose_cfg, base::JSON_ALLOW_TRAILING_COMMAS, NULL, NULL));
            if (!value.get())
            {
                std::cout << "Failed to parse webserver config file" << std::endl;
                return 1;
            }

            if (value->GetType() != Value::TYPE_DICTIONARY)
            {
                std::cerr << "Webserver config: result of parsing json is not a dictionary " + JsonStringify(value.get()) << std::endl;
                return 2;
            }

            DictionaryValue* result_dict = static_cast<DictionaryValue*>(value.get());

            DictionaryValue::key_iterator key_iter = result_dict->begin_keys();
            for (; key_iter != result_dict->end_keys(); ++key_iter) {
                std::string value;
                result_dict->GetStringWithoutPathExpansion(*key_iter, &value);
                mg_options_.push_back(*key_iter);
                mg_options_.push_back(value.c_str());
            }

            return 0;
        }
        else
        {
            std::cerr << "can't read webserver confifg file" << std::endl;
            return 1;
        }
    }

    return 0;
}

}  // namespace webdriver



