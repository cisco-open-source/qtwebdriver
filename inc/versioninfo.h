#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <string>

namespace webdriver {

/// @brief This class contains information about version webdriver
class VersionInfo {
public:
    VersionInfo();
    ~VersionInfo();

    /// @return Name of webdriver, e.g. "WebDriver" or "Google Chrome".
    std::string Name() const;

    /// @return Version number as a string, e.g. "0.1.0".
    std::string Version() const;

    /// @return The git revision of this release.
    std::string LastChange() const;

    /// @return The version string, which contains the name of webdriver, version number and git revision
    std::string CreateVersionString() const;

    /// @return The date and time of last build.
    std::string BuildDateTime() const;
};

}
#endif // VERSIONINFO_H
