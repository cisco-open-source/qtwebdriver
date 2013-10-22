/*! \mainpage Hybrid WebDriver Framework

Hybrid WebDriver(HWD) is library based on Google ChromeDriver and intended
for supporting not only web UI testing. By design it can be extended with 
extension that allow testing native UI. 

Topics:
- \subpage page_wd_server
- \subpage page_commands_routing
- \subpage page_sessions
- \subpage page_views
- \subpage page_url_post
- \subpage page_hybrid_capabilities
- \subpage page_whitelist
*/

//-----------------------------------------------------------

/*! \page page_wd_server WD Server
Entry point for HWD is webdriver::Server singleton class.
It allows to configure, set predefined command routes and start
webdriver service. It is not thread safe.

Server accepts options in form of command line arguments.
Also by default it uses webdriver::DefaultRouteTable. Or custom route table
can be passed after init.

Example:
\code
webdriver::Server* wd_server = webdriver::Server::GetInstance();
if (0 != wd_server->Configure(argc, argv)) {
    return 1;
}

wd_server->Start();
\endcode

For reconfiguring server:
- stop server - webdriver::Server::Stop()
- reset configuration - webdriver::Server::Reset()
- now can call configure and start again.

<h2>Options</h2>
All server options are represented in command line form. In example:
\code 
--verbose --port=9517 --log-path=/var/log/wd.log
\endcode

For all options please refer:
\subpage page_webdriver_switches

*/

#ifndef WEBDRIVER_SERVER_H_
#define WEBDRIVER_SERVER_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/command_line.h"
#include "base/memory/singleton.h"
#include "base/values.h"
#include "webdriver_access.h"

struct mg_context;
struct mg_connection;
struct mg_request_info;

namespace base {
class DictionaryValue;
}

namespace webdriver {

class RouteTable;
class Command;
class HttpResponse;
class Response;

class Server {
public:
    enum State {
        STATE_UNCONFIGURED = 0,
        STATE_IDLE = 1,
        STATE_RUNNING = 2
    };

    /// Configure server from command line
    /// @param options - obtained command line
    /// @return 0 - if init was success, error code otherwise.
    int Configure(const CommandLine &options);

    /// Set route table for this server. Server should be stopped.
    /// @param routeTable routeTable to set. Server keeps own copy of RouteTable.
    /// @return 0 - if success, error code otherwise.
    int SetRouteTable(RouteTable* routeTable);

    /// Start server 
    /// @return 0 - if success, error code otherwise.
    int Start();

    /// Stop server.
    /// @param force force to close opened sessions. If false and there is any open session stop will fail.
    /// @return 0 - if success, error code otherwise.
    int Stop(bool force = false);

    /// Reset server to unconfigured state. Can be applied only in idle state.
    /// @return 0 - if success, error code otherwise.
    int Reset();

    const RouteTable& GetRouteTable() const;
    const std::string& url_base() const;
    const CommandLine& GetCommandLine() const;

    static Server* GetInstance();

private:
    Server();
    ~Server();
    friend struct DefaultSingletonTraits<Server>;

    friend class XDRPCCommand;

    scoped_ptr<CommandLine> options_;
    scoped_ptr<RouteTable> routeTable_;
    std::vector<std::string> mg_options_;
    std::string url_base_;
    struct mg_context* mg_ctx_;
    State state_;
    AccessValidator accessValidor;

    void DispatchCommand(const std::string& matched_route,
                         Command* command_ptr,
                         const std::string& method,
                         Response* response);

    static void* ProcessHttpRequestCb(int event_raised,
                              struct mg_connection* connection,
                              const struct mg_request_info* request_info);

    // Converts a |Response| into a |HttpResponse| to be returned to the client.
    // This function is exposed for testing.
    void PrepareHttpResponse(const std::string& request_method,
                             const Response& command_response,
                             HttpResponse* const http_response);

    bool ProcessHttpRequest(struct mg_connection* connection,
                        const struct mg_request_info* request_info);

    bool ParseRequestInfo(const struct mg_request_info* const request_info,
                      struct mg_connection* const connection,
                      std::string* method,
                      std::vector<std::string>* path_segments,
                      base::DictionaryValue** parameters,
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

    static ListValue* ListCommandSupportedMethods(const Command& command);

    int InitMongooseOptions();

    int InitLogging();

    int ParseConfigToOptions();

    DISALLOW_COPY_AND_ASSIGN(Server);
};


}  // namespace webdriver

#endif  // WEBDRIVER_ROUTE_TABLE_H_
