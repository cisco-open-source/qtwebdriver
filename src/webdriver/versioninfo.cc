/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

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
