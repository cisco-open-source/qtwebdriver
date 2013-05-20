#ifndef WEBDRIVER_ROUTE_TABLE_H_
#define WEBDRIVER_ROUTE_TABLE_H_

#include <string>
#include <vector>

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Command;

namespace internal {

class AbstractCommandCreator;

struct RouteDetails {
    RouteDetails() {
    }

    RouteDetails(const std::string &uri_regex,
                 AbstractCommandCreator* creator)
        : uri_regex_(uri_regex),
          creator_(creator) {
    }

    std::string uri_regex_;
    AbstractCommandCreator* creator_;
};

}  // namespace internal

/// @brief  base class for command creator
class AbstractCommandCreator
{
public:
    virtual Command* create(const std::vector<std::string>& path_segments,
                            const DictionaryValue* const parameters) const = 0;
};

// Template class for creating command of WebDriver REST service.
template <class C>
class CommandCreator: public AbstractCommandCreator
{
public:
    virtual Command* create(const std::vector<std::string>& path_segments,
                            const DictionaryValue* const parameters) const { return new C(path_segments, parameters); }
};

class RouteTable {
public:

    /// Creates a new RouteTable that will register all URL commands with the
    /// given context. Each command's pattern will be prefixed with the provided
    /// root.
    /// @param root root path, url path common for all commands
    explicit RouteTable(const std::string& root);
    ~RouteTable();

    /// Registers a command for a WebDriver command using the given URL pattern.
    /// @tparam <CommandType> class of command to handle this route. Must be subtype of webdriver::Command
    /// @param pattern url pattern of route to add
    template<typename CommandType>
    void Add(const std::string& pattern);

    /// Removes a command for pattern registered previously.
    /// @param pattern url pattern of route to remove 
    void Remove(const std::string& pattern);

    /// Removes all routes.
    void Clear();

    /// Checks if route is already registered
    /// @param pattern url pattern of route to check
    bool HasRoute(const std::string& pattern);

    /// Find matched route for given url and retrieve CommandCreator for this route.
    /// @param pattern url pattern of route to retrieve
    /// @return pointer to CommandCreator, NULL if route not found
    AbstractCommandCreator* GetRouteForURL(const std::string& url);

private:
    void AddRoute(const std::string& uri_pattern,
                  AbstractCommandCreator* creator);

    std::vector<webdriver::internal::RouteDetails> routes_;
    const std::string url_base_;

    DISALLOW_COPY_AND_ASSIGN(RouteTable);
};


template <typename CommandType>
void RouteTable::Add(const std::string& pattern) {
    AddRoute(url_base_ + pattern, new CommandCreator<CommandType>);
}

}  // namespace webdriver

#endif  // WEBDRIVER_ROUTE_TABLE_H_
