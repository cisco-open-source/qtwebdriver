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

#include "widget_view_visualizer.h"

#if !defined(OS_WIN)
#include <dlfcn.h>
#endif
#include <QtCore/QBuffer>
#if defined(OS_LINUX)
#include <QtCore/QProcess>
#endif
#include "base/file_util.h"
#include "webdriver_session.h"
#include "widget_view_util.h"

namespace webdriver {

static const QStringList SUPPORTED_CLASSES = QStringList() << "QCheckBox" << "QLabel" << "QLineEdit" << "QPushButton" << "QScrollArea" << "QToolButton" << "QPlainTextEdit";

FilePath CurrentDynamicObjectPath() {
#if defined(OS_WIN)
    return FilePath();
#else
    Dl_info dl_info;
    dladdr((void *)CurrentDynamicObjectPath, &dl_info);
    return FilePath(dl_info.dli_fname);
#endif
}

FilePath LookupRecursively(const FilePath& folder, const FilePath& path) {
    if (file_util::PathExists(folder.Append(path)))
        return folder.Append(path);

    for (FilePath currentFolder = folder; currentFolder.DirName() != currentFolder; currentFolder = currentFolder.DirName()) {
        if (file_util::PathExists(currentFolder.Append(path)))
            return currentFolder.Append(path);
    }

    return path;
}

static const FilePath STYLESHEET_PATH = LookupRecursively(CurrentDynamicObjectPath().DirName(), FilePath::FromUTF8Unsafe("src/webdriver/extension_qt/widget_view_visualizer.xsl"));
static const QString STYLESHEET_WEB_PATH = "http://localhost:9517/widget_view_visualizer.xsl";
static const FilePath PROCESSOR_PATH = LookupRecursively(CurrentDynamicObjectPath().DirName(), FilePath::FromUTF8Unsafe("src/third_party/saxon/saxon9he.jar"));
static bool XSLT_PROCESS = false;
static bool XSLT_INJECT_STYLESHEET = false;

QWidgetViewVisualizerSourceCommand::QWidgetViewVisualizerSourceCommand(Session* session, ViewId viewId, QWidget* view)
    : session_(session), viewId_(viewId), view_(view)
{}

void QWidgetViewVisualizerSourceCommand::Execute(std::string* source, Error** error) {
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    QWidgetXmlSerializer serializer(&buff);
    serializer.setSession(session_);
    serializer.setViewId(viewId_);
    serializer.setDumpAll(true);
    serializer.setSupportedClasses(SUPPORTED_CLASSES);
    if (XSLT_INJECT_STYLESHEET) {
        serializer.setStylesheet(STYLESHEET_WEB_PATH);
    }
    serializer.createXml(view_);
    *source = byteArray.data();

    session_->logger().Log(kInfoLogLevel, "[VisualizerSource] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);

    if (XSLT_PROCESS) {
#if defined(OS_LINUX)
        *source = transform(*source, STYLESHEET_PATH.value());
#endif
    }
}

#if defined(OS_LINUX)
std::string QWidgetViewVisualizerSourceCommand::transform(const std::string& source, const std::string& stylesheet) const {
    return transform(source, std::wstring(stylesheet.begin(), stylesheet.end()));
}

std::string QWidgetViewVisualizerSourceCommand::transform(const std::string& source, const std::wstring& stylesheet) const {
    QProcess process;
    QStringList arguments;
    arguments << "-jar" << QString::fromStdString(PROCESSOR_PATH.value());
    arguments << QString::fromStdWString(L"-xsl:" + stylesheet);
    arguments << "-";
    process.start("java", arguments);
    if (!process.waitForStarted(-1)) {
        session_->logger().Log(kSevereLogLevel, "Can not start process!");
        return "";
    }
    qint64 written = process.write(source.data(), source.length());
    if (written != source.length()) {
        session_->logger().Log(kSevereLogLevel, "Writting xml to xsl processor failure!");
        return "";
    }
    process.waitForBytesWritten(-1);
    process.closeWriteChannel();

    process.waitForFinished(-1);

    QByteArray stderr = process.readAllStandardError();
    if (stderr.length() > 0) {
        session_->logger().Log(kSevereLogLevel, QString::fromUtf8(stderr.data(), stderr.length()).toStdString());
    }

    QByteArray stdout = process.readAllStandardOutput();
    return QString::fromUtf8(stdout.data(), stdout.length()).toStdString();
}
#endif

} //namespace webdriver
