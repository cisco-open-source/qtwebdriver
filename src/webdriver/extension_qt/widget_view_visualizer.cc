#include "widget_view_visualizer.h"

#include <dlfcn.h>
#include <QtCore/QBuffer>
#include <QtCore/QProcess>
#include "base/file_util.h"
#include "extension_qt/widget_view_executor.h"
#include "webdriver_session.h"

namespace webdriver {

static const QStringList SUPPORTED_CLASSES = QStringList() << "QCheckBox" << "QLabel" << "QLineEdit" << "QPushButton" << "QScrollArea" << "QToolButton" << "QPlainTextEdit";

FilePath CurrentDynamicObjectPath() {
    Dl_info dl_info;
    dladdr((void *)CurrentDynamicObjectPath, &dl_info);
    return FilePath(dl_info.dli_fname);
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

static const FilePath STYLESHEET_PATH = LookupRecursively(CurrentDynamicObjectPath().DirName(), FilePath("src/webdriver/extension_qt/widget_view_visualizer.xsl"));
static const FilePath PROCESSOR_PATH = LookupRecursively(CurrentDynamicObjectPath().DirName(), FilePath("src/third_party/saxon/saxon9he.jar"));

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
    serializer.createXml(view_);
    *source = byteArray.data();

    session_->logger().Log(kInfoLogLevel, "[VisualizerSource] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);
    *source = transform(*source, STYLESHEET_PATH.value());
}

std::string QWidgetViewVisualizerSourceCommand::transform(const std::string& source, const std::string& stylesheet) const {
    QProcess process;
    QStringList arguments;
    arguments << "-jar" << QString::fromStdString(PROCESSOR_PATH.value());
    arguments << QString::fromStdString("-xsl:" + stylesheet);
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

        session_->logger().Log(kSevereLogLevel, QString::fromLatin1(stderr.data(), stderr.length()).toStdString());
    }

    QByteArray stdout = process.readAllStandardOutput();
    return QString::fromLatin1(stdout.data(), stdout.length()).toStdString();
}

} //namespace webdriver
