#include "widget_view_visualizer.h"

#include <QtCore/QBuffer>
#include <QtCore/QProcess>
#include "extension_qt/widget_view_executor.h"
#include "webdriver_session.h"

namespace webdriver {

static const QStringList SUPPORTED_CLASSES = QStringList() << "QCheckBox" << "QLabel" << "QLineEdit" << "QPushButton" << "QScrollArea" << "QToolButton" << "QPlainTextEdit";

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
    *source = transform(*source, "src/webdriver/extension_qt/widget_view_visualizer.xsl");
}

std::string QWidgetViewVisualizerSourceCommand::transform(const std::string& source, const std::string& stylesheet) const {
    QProcess process;
    QStringList arguments;
    arguments << "-jar" << "src/third_party/saxon/saxon9he.jar";
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
