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
