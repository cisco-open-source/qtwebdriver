#ifndef WEBDRIVER_ROUTE_TABLE_H_
#define WEBDRIVER_ROUTE_TABLE_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Command;

/// @brief  base class for command creator
class AbstractCommandCreator : public base::RefCounted<AbstractCommandCreator>
{
public:
    virtual Command* create(const std::vector<std::string>& path_segments,
                            const base::DictionaryValue* const parameters) const = 0;
};

typedef scoped_refptr<AbstractCommandCreator> CommandCreatorPtr;

// Template class for creating command of WebDriver REST service.
template <class C>
class CommandCreator: public AbstractCommandCreator
{
public:
    virtual Command* create(const std::vector<std::string>& path_segments,
                            const base::DictionaryValue* const parameters) const { return new C(path_segments, parameters); }
};

namespace internal {

struct RouteDetails {
    RouteDetails() {
    }

    RouteDetails(const std::string &uri_regex,
                 const webdriver::CommandCreatorPtr& creator)
        : uri_regex_(uri_regex),
          creator_(creator) {
    }

    std::string uri_regex_;
    webdriver::CommandCreatorPtr creator_;
};

}  // namespace internal

/// Container for routes. Each route is a pair - url pattern and command creator.
/// url pattern is string. Some path segments in this url pattern can be wildcarded with single '*'.
/// In example - "/session/*/log"
class RouteTable {
public:

    /// Creates a new RouteTable that will register all URL commands with the
    /// given context.
    explicit RouteTable();

    /// Copy constructor
    RouteTable(const RouteTable &obj);
 
    /// assignment operator
    RouteTable& operator= (const RouteTable &obj);

    virtual ~RouteTable();

    /// Registers a command for a WebDriver command using the given URL pattern.
    /// @tparam <CommandType> class of command to handle this route. Must be subtype of webdriver::Command
    /// @param pattern url pattern of route to add
    /// @return true, if added successfully, false - otherwise.
    template<typename CommandType>
    bool Add(const std::string& pattern);

    /// Removes a command for pattern registered previously.
    /// @param pattern url pattern of route to remove 
    void Remove(const std::string& pattern);

    /// Removes all routes.
    void Clear();

    /// Checks if route is already registered
    /// @param pattern url pattern of route to check
    bool HasRoute(const std::string& pattern);

    /// Find matched route for given url and retrieve CommandCreator for this route.
    /// @param url url to handle
    /// @return pointer to CommandCreator, NULL if route not found
    CommandCreatorPtr GetRouteForURL(const std::string& url);

    /// Returns list of registered routes
    /// @return vector of registered patterns
    std::vector<std::string> GetRoutes();

private:
    // return true if added successfully, false - otherwise.
    bool AddRoute(const std::string& uri_pattern,
                  const CommandCreatorPtr& creator);

    // return true if pattern1 is bestmatch then pattern2
    bool CompareBestMatch(const std::string& uri_pattern1, const std::string& uri_pattern2);

    bool MatchPattern(const std::string& url, const std::string& pattern);

    std::vector<webdriver::internal::RouteDetails> routes_;
};

template <typename CommandType>
bool RouteTable::Add(const std::string& pattern) {
    return AddRoute(pattern, make_scoped_refptr(new CommandCreator<CommandType>));
}

/// Subclass of RouteTable with default WD commands set.
class DefaultRouteTable : public RouteTable {
public:

    /// Creates a new DefaultRouteTable with default WD commands set.
    explicit DefaultRouteTable();
    virtual ~DefaultRouteTable();
};

}  // namespace webdriver

#endif  // WEBDRIVER_ROUTE_TABLE_H_
