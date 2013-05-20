// TODO: put this on init
// Overwrite mongoose's default handler for /favicon.ico to always return a
// 204 response so we don't spam the logs with 404s.
AddCallback("/favicon.ico", &SendNoContentResponse, NULL);











//#include "base/command_line.h"
//#include "base/format_macros.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
//#include "base/message_loop_proxy.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/sys_info.h"
//#include "base/threading/platform_thread.h"
//#include "base/threading/thread.h"
#include "chrome/common/chrome_version_info.h"
#include "chrome/test/webdriver/commands/command.h"
#include "chrome/test/webdriver/http_response.h"
#include "chrome/test/webdriver/webdriver_logging.h"
#include "chrome/test/webdriver/webdriver_session_manager.h"
#include "chrome/test/webdriver/webdriver_switches.h"
#include "chrome/test/webdriver/webdriver_util.h"

namespace webdriver {

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

    AbstractCommandCreator* cmdCreator = routeTable->GetRouteForURL(uri);
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

    // TODO: bad solution to have url_base in session_manager. Get it from RouteTable?
    // fix this
    std::string uri(request_info->uri);
    SessionManager* manager = SessionManager::GetInstance();
    uri = uri.substr(manager->url_base().length());

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

}  // namespace webdriver



