#include "versioninfo.h"
#include "webdriver_version.h"

namespace webdriver {

VersionInfo::VersionInfo() {
}

VersionInfo::~VersionInfo() {
}

std::string VersionInfo::Name() {
    return kProductName;
}

std::string VersionInfo::Version() {
    return kVersionNumber;
}

std::string VersionInfo::LastChange() {
    return kLastChanges;
}

std::string VersionInfo::BuildDateTime() {
    std::string buildTime = kBuildTime;
    std::string buildDate = kBuildDate;
    return buildDate + " " + buildTime;
}

std::string VersionInfo::CreateVersionString() {
    std::string current_version;
    current_version = "Version:    "+ Name() + "-" + Version() +
                    "\nBuild Time: "+ BuildDateTime() +
                    "\nRevision:   " + LastChange();
    return current_version;
}

}
