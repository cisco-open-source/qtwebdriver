#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <string>

namespace webdriver {

class VersionInfo {
public:
    VersionInfo();
    ~VersionInfo();

    // E.g. "WebDriver" or "Google Chrome".
    std::string Name() const;

    // Version number, e.g. "0.1.0".
    std::string Version() const;

    // The git revision of this release.  E.g. "".
    std::string LastChange() const;

    // Returns a version string.
    std::string CreateVersionString() const;

    // Returns a date and time of last build.
    std::string BuildDateTime() const;
};

}

#endif // VERSIONINFO_H
