
#include "webdriver_server.h"
#include "webdriver_route_table.h"









// TODO: review all headers

//#include "base/command_line.h"
//#include "base/format_macros.h"
#include "base/json/json_reader.h"
#include "base/memory/scoped_ptr.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/sys_info.h"
#include "chrome/common/chrome_version_info.h"
#include "chrome/test/webdriver/commands/command.h"
#include "chrome/test/webdriver/http_response.h"
#include "chrome/test/webdriver/webdriver_logging.h"
#include "chrome/test/webdriver/webdriver_session_manager.h"
#include "chrome/test/webdriver/webdriver_switches.h"
#include "chrome/test/webdriver/webdriver_util.h"


namespace webdriver {

void* ProcessHttpRequest(mg_event event_raised,
                         struct mg_connection* connection,
                         const struct mg_request_info* request_info);

Server::Server()
    : options_(CommandLine::NO_PROGRAM),
    routeTable_(NULL),
    mg_ctx_(NULL) {
    
    routeTable_ = new DefaultRouteTable();
}

Server::~Server() {};

int Server::Init(int argc, char *argv[]) {
    int ret_val = 0;

    // TODO: check if we already inited

    options_.InitFromArgv(argc, argv);

#if !defined(OS_WIN)    
    ret_val = ParseConfigToOptions();
    if (ret_val)
        return ret_val;
#endif

    ret_val = InitMongooseOptions();
    if (ret_val)
        return ret_val;

    ret_val = InitLogging();
    if (ret_val)
        return ret_val;

    if (options_->HasSwitch("url-base"))
        url_base_ = options_->GetSwitchValueASCII("url-base");

    std::string chromedriver_info = base::StringPrintf("ChromeDriver %s", "QtWebKit");
    FileLog::Get()->Log(kInfoLogLevel, base::Time::Now(), chromedriver_info);

    // TODO: TBD

}

void Server::SetRouteTable(RouteTable* routeTable) {
    // TODO: implement
}

int Server::Start() {
    // TODO: check state

    scoped_array<const char*> opts(new const char*[mg_options_.size() + 1]);
    for (size_t i = 0; i < mg_options_.size(); ++i) {
        opts[i] = mg_options_[i].c_str();
    }
    opts[mg_options_.size()] = NULL;

    // Initialize SHTTPD context.
    mg_ctx_ = mg_start(&ProcessHttpRequest,
                        routeTable_,
                        opts.get());
    if (mg_ctx_ == NULL) {
        std::cerr << "Port already in use. Exiting..." << std::endl;
#if defined(OS_WIN)
        return WSAEADDRINUSE;
#else
        return EADDRINUSE;
#endif
    }
}

// Maximum safe size of HTTP response message. Any larger than this,
// the message may not be transferred at all.
const size_t kMaxHttpMessageSize = 1024 * 1024 * 16;  // 16MB

// mongoose callback
void* ProcessHttpRequest(mg_event event_raised,
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
    std::string method;
    std::vector<std::string> path_segments;
    base::DictionaryValue* parameters = NULL;
    Response response;
    std::string uri(request_info->uri);

    // Overwrite mongoose's default handler for /favicon.ico to always return a
    // 204 response so we don't spam the logs with 404s.
    if (uri == "/favicon.ico") {
        SendNoContentResponse();
        return true;
    }

    // remove url_base from uri
    uri = uri.substr(url_base_.length());


    AbstractCommandCreator* cmdCreator = routeTable_->GetRouteForURL(uri);
    if (NULL == cmdCreator)
    {
        // TODO: print log
        return false;
    }

    if (ParseRequestInfo(request_info,
                            connection,
                            &method,
                            &path_segments,
                            &parameters,
                            &response)) {
        DispatchHelper(
                    cmdCreator->create(path_segments, parameters),
                    method,
                    &response);
    }
    SendResponse(connection,
                request_info->request_method,
                response);
    
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
    PrepareHttpResponse(response, &http_response);
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

void Server::PrepareHttpResponse(const Response& command_response,
                                 HttpResponse* const http_response) {
    ErrorCode status = command_response.GetStatus();
    switch (status) {
    case kSuccess:
        http_response->set_status(HttpResponse::kOk);
        break;

        // TODO(jleyba): kSeeOther, kBadRequest, kSessionNotFound,
        // and kMethodNotAllowed should be detected before creating
        // a command_response, and should thus not need conversion.
    case kSeeOther: {
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
        http_response->set_status(HttpResponse::kSeeOther);
        break;
    }

    case kBadRequest:
    case kSessionNotFound:
        http_response->set_status(status);
        break;

    case kMethodNotAllowed: {
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
        http_response->set_status(HttpResponse::kMethodNotAllowed);
        break;
    }

        // All other errors should be treated as generic 500s. The client
        // will be responsible for inspecting the message body for details.
    case kInternalServerError:
    default:
        http_response->set_status(HttpResponse::kInternalServerError);
        break;
    }

    http_response->SetMimeType("application/json; charset=utf-8");
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
    uri = uri.substr(url_base_.length());

    base::SplitString(uri, '/', path_segments);

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

void Server::DispatchHelper(Command* command_ptr,
                            const std::string& method,
                            Response* response) {
    CHECK(method == "GET" || method == "POST" || method == "DELETE");
    scoped_ptr<Command> command(command_ptr);

    if ((method == "GET" && !command->DoesGet()) ||
            (method == "POST" && !command->DoesPost()) ||
            (method == "DELETE" && !command->DoesDelete())) {
        ListValue* methods = new ListValue;
        if (command->DoesPost())
            methods->Append(Value::CreateStringValue("POST"));
        if (command->DoesGet()) {
            methods->Append(Value::CreateStringValue("GET"));
            methods->Append(Value::CreateStringValue("HEAD"));
        }
        if (command->DoesDelete())
            methods->Append(Value::CreateStringValue("DELETE"));
        response->SetStatus(kMethodNotAllowed);
        response->SetValue(methods);
        return;
    }

    DispatchCommand(command.get(), method, response);
}

void Server::DispatchCommand(Command* const command,
                             const std::string& method,
                             Response* response) {
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

int Server::InitMongooseOptions() {
    std::string port = "9517";
    std::string root;
    int http_threads = 4;
    bool enable_keep_alive = false;

    if (options_->HasSwitch("port"))
        port = options_->GetSwitchValueASCII("port");
    // The 'root' flag allows the user to specify a location to serve files from.
    // If it is not given, a callback will be registered to forbid all file
    // requests.
    if (options_->HasSwitch("root"))
        root = options_->GetSwitchValueASCII("root");
    if (options_->HasSwitch("http-threads")) {
        if (!base::StringToInt(options_->GetSwitchValueASCII("http-threads"),
                           &http_threads)) {
            std::cerr << "'http-threads' option must be an integer";
            return 1;
        }
    }
//  if (options_->HasSwitch(kEnableKeepAlive))
//      enable_keep_alive = true;

    mg_options_->push_back("listening_ports");
    mg_options_->push_back(port);
    mg_options_->push_back("enable_keep_alive");
    mg_options_->push_back(enable_keep_alive ? "yes" : "no");
    mg_options_->push_back("num_threads");
    mg_options_->push_back(base::IntToString(http_threads));
    if (!root.empty()) {
        mg_options_->push_back("document_root");
        mg_options_->push_back(root);
    }
    mg_options_->push_back("extra_mime_types");
    mg_options_->push_back(".xhtml=application/xhtml+xml");

    return 0;
}

int Server::InitLogging() {
    FilePath log_path;

    if (cmd_line->HasSwitch("log-path"))
        log_path = cmd_line->GetSwitchValuePath("log-path");

    // Init global file log.
    FileLog* log;

    if (log_path.empty()) {
        log = FileLog::CreateFileLog(FILE_PATH_LITERAL("webdriver.log"), kAllLogLevel);
    } else {
        log = new FileLog(log_path, kAllLogLevel);
    }

    FileLog::SetGlobalLog(log);

    if (cmd_line->HasSwitch("verbose")) {
        // TODO: handle
    }

    // check if silence mode
    if (cmd_line->HasSwitch("silence")) {
        // TODO: handle
    }

    if (log->IsOpen())
        return 0;

    std::cerr << "ERROR: InitLogging failed.";
    return 1;
}

#if !defined(OS_WIN)
int ParseConfigToOptions() {
    if (options_->HasSwitch("config"))
    {
        //parse json config file and set value
        std::string config_json;
        std::string configPathString = options_->GetSwitchValueASCII("config");

        FilePath configPath(configPathString);

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
                std::cerr << "Execute script returned non-dict: " + JsonStringify(value.get()) << std::endl;
                return 2;
            }

            DictionaryValue* result_dict = static_cast<DictionaryValue*>(value.get());

            int port;
            std::string root;
            std::string url_base;
            int http_threads;
            std::string log_path;
            if (result_dict->GetInteger("port", &port))
                options_->AppendSwitchASCII("port", base::IntToString(port));
            if (result_dict->GetString("root", &root))
                options_->AppendSwitchASCII("root", root);
            if (result_dict->GetString("url-base", &url_base))
                options_->AppendSwitchASCII("url-base", url_base);
            if (result_dict->GetInteger("http-threads", &http_threads))
                options_->AppendSwitchASCII("http-threads", base::IntToString(http_threads));
            if (result_dict->GetString("log-path", &log_path))
                options_->AppendSwitchASCII("log-path", log_path);

            return 0;

        }
        else
        {
            std::cerr << "can't read file" << std::endl;
            return 1;
        }
    }
}
#endif //!defined(OS_WIN)

}  // namespace webdriver



