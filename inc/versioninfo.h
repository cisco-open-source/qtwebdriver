#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <string>

namespace webdriver {

/// @brief This class contains information about version webdriver
class VersionInfo {
public:
    /// @return Name of webdriver, e.g. "WebDriver" or "Google Chrome".
    static std::string Name();

    /// @return Version number as a string, e.g. "0.1.0".
    static std::string Version();

    /// @return The git revision of this release.
    static std::string LastChange();

    /// @return The version string, which contains the name of webdriver, version number and git revision
    static std::string CreateVersionString();

    /// @return The date and time of last build.
    static std::string BuildDateTime();

private:
    VersionInfo();
    ~VersionInfo();
};

}
#endif // VERSIONINFO_H
