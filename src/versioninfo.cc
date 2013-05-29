#include "versioninfo.h"
#include "wdversion.h"



namespace webdriver {

VersionInfo::VersionInfo()
{
}

VersionInfo::~VersionInfo()
{
}

std::string VersionInfo::Name() const
{
    return kProductName;
}

std::string VersionInfo::Version() const
{
    return kVersionNumber;
}

std::string VersionInfo::LastChange() const
{
    return kLastChanges;
}

std::string VersionInfo::BuildDateTime() const
{
    std::string buildTime = kBuildTime;
    std::string buildDate = kBuildDate;
    return buildDate + " " + buildTime;
}

std::string VersionInfo::CreateVersionString() const
{
    std::string current_version;
    current_version = Name() + "-" + Version();
    return current_version;
}

}
