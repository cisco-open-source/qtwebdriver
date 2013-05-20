#ifndef WEBDRIVER_SERVER_H_
#define WEBDRIVER_SERVER_H_

#include <string>
#include <vector>

#include "base/basictypes.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Command;
class HttpResponse;

class Server {
public:

    // Creates a new Server
    Server();
    ~Server();

    // init server, parse arguments as options
    int init(int argc, char *argv[]);

    // start server
    int start();

private:

    // Converts a |Response| into a |HttpResponse| to be returned to the client.
    // This function is exposed for testing.
    void PrepareHttpResponse(const Response& command_response,
                             HttpResponse* const http_response);

    void DispatchCommand(Command* const command,
                         const std::string& method,
                         Response* response);

    // Allows the bulk of the implementation of |Dispatch| to be moved out of this
    // header file. Takes ownership of |command|.
    void DispatchHelper(Command* command_ptr,
                        const std::string& method,
                        Response* response);

    bool ProcessHttpRequest(struct mg_connection* connection,
                        const struct mg_request_info* request_info);

    bool ParseRequestInfo(const struct mg_request_info* const request_info,
                      struct mg_connection* const connection,
                      std::string* method,
                      std::vector<std::string>* path_segments,
                      DictionaryValue** parameters,
                      Response* const response);

    void ReadRequestBody(const struct mg_request_info* const request_info,
                     struct mg_connection* const connection,
                     std::string* request_body);

    void SendNoContentResponse(struct mg_connection* connection,
                           const struct mg_request_info* request_info);
 
    void SendResponse(struct mg_connection* const connection,
                  const std::string& request_method,
                  const Response& response);

    void WriteHttpResponse(struct mg_connection* connection,
                       const HttpResponse& response);


    DISALLOW_COPY_AND_ASSIGN(Server);
};


}  // namespace webdriver

#endif  // WEBDRIVER_ROUTE_TABLE_H_
