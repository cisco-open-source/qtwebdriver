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

#include "extension_qt/vncclient.h"
#include "third_party/des/d3des.h"

#include <QtNetwork/QHostAddress>
#include <QtCore/QMap>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#define MAJOR_INDEX 6
#define MINOR_INDEX 10

using namespace webdriver;

VNCClient* VNCClient::_instance = NULL;

static QMap<quint32, quint16> initializeMap()
{
    QMap<quint32, quint16> resultMap;
    resultMap.insert(Qt::Key_Backspace, 0xff08);
    resultMap.insert(Qt::Key_Tab,       0xff09);
    resultMap.insert(Qt::Key_Return,    0xff0d);
    resultMap.insert(Qt::Key_Escape,    0xff1b);
    resultMap.insert(Qt::Key_Insert,    0xff63);
    resultMap.insert(Qt::Key_Delete,    0xffff);
    resultMap.insert(Qt::Key_Home,      0xff50);
    resultMap.insert(Qt::Key_End,       0xff57);
    resultMap.insert(Qt::Key_PageUp,    0xff55);
    resultMap.insert(Qt::Key_PageDown,  0xff56);
    resultMap.insert(Qt::Key_Left,      0xff51);
    resultMap.insert(Qt::Key_Up,        0xff52);
    resultMap.insert(Qt::Key_Right,     0xff53);
    resultMap.insert(Qt::Key_Down,      0xff54);
    resultMap.insert(Qt::Key_F1,        0xffbe);
    resultMap.insert(Qt::Key_F2,        0xffbf);
    resultMap.insert(Qt::Key_F3,        0xffc0);
    resultMap.insert(Qt::Key_F4,        0xffc1);
    resultMap.insert(Qt::Key_F5,        0xffc2);
    resultMap.insert(Qt::Key_F6,        0xffc3);
    resultMap.insert(Qt::Key_F7,        0xffc4);
    resultMap.insert(Qt::Key_F8,        0xffc5);
    resultMap.insert(Qt::Key_F9,        0xffc6);
    resultMap.insert(Qt::Key_F10,       0xffc7);
    resultMap.insert(Qt::Key_F11,       0xffc8);
    resultMap.insert(Qt::Key_F12,       0xffc9);
    resultMap.insert(Qt::Key_Shift,     0xffe1);
    resultMap.insert(Qt::Key_Control,   0xffe3);
    resultMap.insert(Qt::Key_Meta,      0xffe7);
    resultMap.insert(Qt::Key_Alt,       0xffe9);
    resultMap.insert(Qt::Key_Shift,     0xffe1);
    resultMap.insert(Qt::Key_SysReq,    0xff15);
    resultMap.insert(Qt::Key_Pause,     0xff13);
    resultMap.insert(Qt::Key_ScrollLock,0xff14);
    resultMap.insert(Qt::Key_Print,     0xff61);
    resultMap.insert(Qt::Key_NumLock,   0xff7f);

    return resultMap;
};

QMap<quint32, quint16> VNCClient::_keymap = initializeMap();

VNCClient::VNCClient()
    : _socket(NULL),
      _versionEstablished(false),
      _securityEstablished(false),
      _autenticationPassed(false),
      _handshakeFinished(false),
      _communicationError(false),
      _isReady(false),
      _establishedVersion(38),
      _establishedSecurity(Invalid),
      _serverParameters(NULL),
      _password(NULL)
{
    _logger = StdOutLog::Get();
}

VNCClient::~VNCClient()
{
    delete _serverParameters;
    delete _password;
}

VNCClient* VNCClient::getInstance()
{
    if (NULL == _instance)
    {
        _instance = new VNCClient();
    }

    return _instance;
}

bool VNCClient::Init(QString remoteHost, quint16 port)
{
    _socket = new QTcpSocket();
    QHostAddress addr;

    if (!addr.setAddress(remoteHost))
    {
        remoteHost.replace(QRegExp("http*://"), "");
        addr.setAddress(remoteHost);
    }

    // QObject::connect(_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(readSocket(qint64)));
    QObject::connect(_socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(onError(QAbstractSocket::SocketError)));
    QObject::connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    _socket->connectToHost(addr, port);
    _socket->waitForConnected();

    return _socket->isOpen();
}

bool VNCClient::Init(QString remoteHost, quint16 port, QString* password)
{
    _password = password;
    _socket = new QTcpSocket();
    QHostAddress addr;

    if (!addr.setAddress(remoteHost))
    {
        remoteHost.replace(QRegExp("http*://"), "");
        addr.setAddress(remoteHost);
    }

    // QObject::connect(_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(readSocket(qint64)));
    QObject::connect(_socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(onError(QAbstractSocket::SocketError)));
    QObject::connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    _socket->connectToHost(addr, port);
    _socket->waitForConnected();

    return _socket->isOpen();
}

QByteArray VNCClient::readSocket()
{
    QByteArray data = _socket->readAll();

    _logger->Log(kInfoLogLevel, base::Time::Now(), std::string("#### Read from socket: ") + std::string(data.data()));
    for (int i=0; i<data.size(); i++)
        _logger->Log(kInfoLogLevel, base::Time::Now(), QString(data.at(i)).toStdString());
    _logger->Log(kInfoLogLevel, base::Time::Now(), "\n");

    if (!_versionEstablished)
    {
        establishProtocolVersion(data);
        return data;
    }
    if (!_securityEstablished)
    {
        if (_communicationError)
        {
            handleZeroError(data);
            return data;
        }
        establishSecurity(data);
        return data;
    }

    // Go through autentication
    if (!_autenticationPassed && VNCAuthentication == _establishedSecurity)
    {
        passAutentication(data);
        return data;
    }
    if (!_handshakeFinished)
    {
        finishHandshaking(data);
        return data;
    }
    if (NULL == _serverParameters)
    {
        initServerParameters(data);
    }

    return data;
}

/* QByteArray VNCClient::readSocket(qint64 size)
{
    QByteArray data = _socket->read(size);

    std::cout << "#### Read from socket: " << data.data() << std::endl;

    if (!_versionEstablished)
    {
        establishProtocolVersion(data);
        return data;
    }
    if (!_securityEstablished)
    {
        if (_communicationError)
        {
            handleZeroError(data);
            return data;
        }
        establishSecurity(data);
        return data;
    }

    // Go through autentication
    if (!_autenticationPassed && VNCAuthentication == _establishedSecurity)
    {
        passAutentication(data);
        return data;
    }
    if (!_handshakeFinished)
    {
        finishHandshaking(data);
        return data;
    }
    if (NULL == _serverParameters)
    {
        initServerParameters(data);
    }

    return data;
}*/

qint64 VNCClient::writeToSocket(QByteArray &data)
{
    int bytesNmb = 0;

    if (QAbstractSocket::ConnectedState == _socket->state())
    {
        bytesNmb = _socket->write(data);
        _socket->flush();
    }
    else
    {
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket isn't in connected state. Couldn't write to socket");
    }

    return bytesNmb;
}

void VNCClient::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Socket state: UnconnectedState"); break;
    case QAbstractSocket::HostLookupState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Sockte state: HostLookupState"); break;
    case QAbstractSocket::ConnectingState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Socket state: ConnectingState"); break;
    case QAbstractSocket::ConnectedState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Socket state: ConnectedState"); break;
    case QAbstractSocket::BoundState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Socket state: BoundState"); break;
    case QAbstractSocket::ClosingState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Sockte state: ClosingState"); break;
    case QAbstractSocket::ListeningState:
        _logger->Log(kInfoLogLevel, base::Time::Now(), "Sockte state: ListeningState"); break;
    }
}

void VNCClient::onError(QAbstractSocket::SocketError error)
{
    switch (error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ConnectionRefusedError"); break;
    case QAbstractSocket::RemoteHostClosedError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: RemoteHostClosedError"); break;
    case QAbstractSocket::HostNotFoundError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: HostNotFoundError"); break;
    case QAbstractSocket::SocketAccessError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: SocketAccessError"); break;
    case QAbstractSocket::SocketResourceError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: SocketResourceError"); break;
    case QAbstractSocket::SocketTimeoutError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: SocketTimeoutError"); break;
    case QAbstractSocket::DatagramTooLargeError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: DatagramTooLargeError"); break;
    case QAbstractSocket::NetworkError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: NetworkError"); break;
    case QAbstractSocket::AddressInUseError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: AddressInUseError"); break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: SocketAddressNotAvailableError"); break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: UnsupportedSocketOperationError"); break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyAuthenticationRequiredError"); break;
    case QAbstractSocket::SslHandshakeFailedError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: SslHandshakeFailedError"); break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: UnfinishedSocketOperationError"); break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyConnectionRefusedError"); break;
    case QAbstractSocket::ProxyConnectionClosedError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyConnectionClosedError"); break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyConnectionTimeoutError"); break;
    case QAbstractSocket::ProxyNotFoundError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyNotFoundError"); break;
    case QAbstractSocket::ProxyProtocolError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: ProxyProtocolError"); break;
    case QAbstractSocket::UnknownSocketError:
        _logger->Log(kWarningLogLevel, base::Time::Now(), "Socket error: UnknownSocketError"); break;
    }
}

bool VNCClient::establishProtocolVersion(QByteArray &data)
{
    char *version = data.data();
    int major = (int)version[MAJOR_INDEX] - 0x30;
    int minor = (int)version[MINOR_INDEX] - 0x30;

    if (3 != major && (3 != minor && 7 != minor && 8 != minor))
    {
        char version[] = {(char)(major+0x30), (char)(minor+0x30)};
        _logger->Log(kWarningLogLevel, base::Time::Now(), std::string("Bad protocol version: ") + std::string(version));
        _socket->close();
        return false;
    }

    if (major*10+minor < _establishedVersion)
    {
        _establishedVersion = major*10+minor;
    }

    // convert version to message for server
    char response[12] = {'R', 'F', 'B', 0x20, 0x30, 0x30, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x0a};
    major = _establishedVersion/10%10;
    minor = _establishedVersion%10;

    response[MAJOR_INDEX] = major + 0x30;
    response[MINOR_INDEX] = minor + 0x30;

    QByteArray respData(response, 12);
    writeToSocket(respData);

    _versionEstablished = true;

    return true;
}

bool VNCClient::establishSecurity(QByteArray& data)
{
    if (37 <= _establishedVersion)
    {
        int nmbTypes = data.at(0);

        for (int i=1; i<=nmbTypes; i++)
        {
            switch(data.at(i))
            {
                case Invalid:
                {
                    _communicationError = true;
                    _socket->close();
                    return false;
                }
                case None:
                {
                    if (NULL == _password)
                    {
                        char one = 0x01;
                        QByteArray response(1, one);
                        writeToSocket(response);
                        _securityEstablished = true;
                        _establishedSecurity = None;
                        return true;
                    }
                    break;
                }
                case VNCAuthentication:
                {
                    char one = 0x02;
                    QByteArray response(1, one);
                    writeToSocket(response);
                    _securityEstablished = true;
                    _establishedSecurity = VNCAuthentication;
                    return true;
                }
                case RA2: break;
                case RA2ne: break;
                case Tight: break;
                case Ultra: break;
                case TLS: break;
                case VeNCrypt: break;
                case GTK_VNC_SASL: break;
                case MD5_hash_athentication: break;
                case ColinDean_xvp: break;
            }
        }
    }
    if (33 == _establishedVersion)
    {
        quint32 security = data.at(0)*0x1000000 + data.at(1)*0x10000 + data.at(2)*0x100 + data.at(3);
        switch(security)
        {
            case Invalid:
            {
                _communicationError = true;
                _socket->close();
                return false;
            }
            case None:
            {
                if (NULL == _password)
                {
                    _securityEstablished = true;
                    _establishedSecurity = None;
                    return true;
                }
                break;
            }
            case VNCAuthentication:
            {
                _securityEstablished = true;
                _establishedSecurity = VNCAuthentication;
                return true;
            }
            break;
        }
    }

    _socket->close();
    return false;
}

bool VNCClient::passAutentication(QByteArray &data)
{
    if (16 != data.size())
    {
        _socket->close();
        return false;
    }

    _password->toStdString();

    QByteArray passwd = _password->toLocal8Bit();

    rfbDesKey((unsigned char*)passwd.data(), EN0);
    unsigned char response[16];
    memcpy(response, data.data(), 16);
    rfbDes((unsigned char*)data.data(), response);
    rfbDes((unsigned char*)data.data()+8, response+8);

    QByteArray encrypted((const char*)response);

    _socket->write(encrypted.left(16));
    _autenticationPassed = true;

    return true;
}

bool VNCClient::finishHandshaking(QByteArray &data)
{
    bool result = false;
    int response  = data.at(0)*0x1000000 + data.at(1)*0x10000 + data.at(2)*0x100 + data.at(3);

    if (0 == response)
    {
        _handshakeFinished = true;
        char one = 0x01;
        QByteArray initMsg(1, one);
        writeToSocket(initMsg);
        result = true;
    }
    else
    {
        QByteArray error("Security type not established");
        handleZeroError(error);
        result = false;
        _socket->close();
    }

    return result;
}

bool VNCClient::initServerParameters(QByteArray &data)
{
    if (NULL == _serverParameters)
        _serverParameters = new ServerParameters();

    qint16 width = data.at(0)*0x100 + data.at(1);
    _serverParameters->setWidth(width);
    qint16 height = data.at(2)*0x100 + data.at(3);
    _serverParameters->setHeight(height);

    ServerParameters::PIXEL_FORMAT *pixels = new ServerParameters::PIXEL_FORMAT();
    memcpy(pixels, data.data()+4, 16);

    int nameLength = data.at(20)*0x1000000 + data.at(21)*0x10000 + data.at(22)*0x100 + data.at(23);
    QByteArray name = data.right(nameLength);

    _serverParameters->setName(new QString(name.data()));
    _isReady = true;

    return true;
}

void VNCClient::handleZeroError(QByteArray& data)
{
    quint32 size = data.at(0)*0x1000000 + data.at(1)*0x10000 + data.at(2)*0x100 + data.at(3);

    char *msg;
    msg = (data.right(size)).data();

    _logger->Log(kWarningLogLevel, base::Time::Now(), msg);

    _socket->close();
}

void VNCClient::sendKeyEvent(QKeyEvent *key)
{
    QByteArray keyMsg;

    keyMsg.insert(0, (char)KeyEvent); // set message type

    if (QKeyEvent::KeyPress == key->type())
    {
        keyMsg.insert(1, (char)0x01);
    }
    if (QKeyEvent::KeyRelease == key->type())
    {
        keyMsg.insert(1, (char)0x00);
    }

    keyMsg.insert(2, (char)0x00);
    keyMsg.insert(3, (char)0x00);

    quint32 keyValue = convertQtKeyToX11Key(key);
    char value[4];
    value[0] = (char)((keyValue&0xff000000)>>24);
    value[1] = (char)((keyValue&0x00ff0000)>>16);
    value[2] = (char)((keyValue&0x0000ff00)>>8);
    value[3] = (char)(keyValue&0x000000ff);
    keyMsg.insert(4, value[0]);
    keyMsg.insert(5, value[1]);
    keyMsg.insert(6, value[2]);
    keyMsg.insert(7, value[3]);

    _logger->Log(kInfoLogLevel, base::Time::Now(), "Send key message: ");
    for (int i=0; i<keyMsg.size(); i++)
    {
        _logger->Log(kInfoLogLevel, base::Time::Now(), QString(keyMsg.at(i)).toStdString());
    }
    _logger->Log(kInfoLogLevel, base::Time::Now(), "\n");

    writeToSocket(keyMsg);
}

void VNCClient::sendMouseEvent(QMouseEvent *mouse)
{
    QByteArray mouseMsg;
    quint8 mouseBtn = 0x00;
    QEvent::Type type = mouse->type();

    if (QEvent::MouseButtonPress == type)
    {
        switch(mouse->button())
        {
            case Qt::NoButton: break;
            case Qt::LeftButton: mouseBtn = mouseBtn | 0x01; break;
            case Qt::RightButton: mouseBtn = mouseBtn | 0x04; break;
            case Qt::MidButton: mouseBtn = mouseBtn | 0x02; break;
            default: break;
        }
    }
    else if (QEvent::MouseButtonRelease == type)
    {
        switch(mouse->button())
        {
            case Qt::NoButton: break;
            case Qt::LeftButton: mouseBtn = mouseBtn & 0xfe; break;
            case Qt::RightButton: mouseBtn = mouseBtn & 0xfb; break;
            case Qt::MidButton: mouseBtn = mouseBtn & 0xfd; break;
            default: break;
        }
    }
    else if (QEvent::MouseButtonDblClick == type)
    {
        sendDoubleClick(mouse);
        return;
    }

    mouseMsg.insert(0, (char)PointerEvent); // set message type
    mouseMsg.insert(1, (char)mouseBtn);

    quint16 x = mouse->x();
    quint16 y = mouse->y();

    char item = (char)(x>>8);
    mouseMsg.insert(2, item);
    item = (char)(x & 0x00ff);
    mouseMsg.insert(3, item);
    item = (char)(y>>8);
    mouseMsg.insert(4, item);
    item = (char)(y & 0x00ff);
    mouseMsg.insert(5, item);

    writeToSocket(mouseMsg);
}

void VNCClient::sendDoubleClick(QMouseEvent *event)
{
    QByteArray mouseMsg;
    quint8 mouseBtn = 0x00;

    switch(event->button())
    {
        case Qt::NoButton: break;
        case Qt::LeftButton: mouseBtn = mouseBtn | 0x01; break;
        case Qt::RightButton: mouseBtn = mouseBtn | 0x04; break;
        case Qt::MidButton: mouseBtn = mouseBtn | 0x02; break;
        default: break;
    }

    mouseMsg.insert(0, (char)PointerEvent); // set message type
    mouseMsg.insert(1, (char)mouseBtn);

    quint16 x = event->globalX();
    quint16 y = event->globalY();

    char item = (char)(x>>8);
    mouseMsg.insert(2, item);
    item = (char)(x & 0x00ff);
    mouseMsg.insert(3, item);
    item = (char)(y>>8);
    mouseMsg.insert(4, item);
    item = (char)(y & 0x00ff);
    mouseMsg.insert(5, item);

    writeToSocket(mouseMsg);

    switch(event->button())
    {
        case Qt::NoButton: break;
        case Qt::LeftButton: mouseBtn = mouseBtn & 0xfe; break;
        case Qt::RightButton: mouseBtn = mouseBtn & 0xfb; break;
        case Qt::MidButton: mouseBtn = mouseBtn & 0xfd; break;
        default: break;
    }

    mouseMsg.insert(1, mouseBtn);
    writeToSocket(mouseMsg);

    switch(event->button())
    {
        case Qt::NoButton: break;
        case Qt::LeftButton: mouseBtn = mouseBtn | 0x01; break;
        case Qt::RightButton: mouseBtn = mouseBtn | 0x04; break;
        case Qt::MidButton: mouseBtn = mouseBtn | 0x02; break;
        default: break;
    }

    mouseMsg.insert(1, mouseBtn);
    writeToSocket(mouseMsg);

    switch(event->button())
    {
        case Qt::NoButton: break;
        case Qt::LeftButton: mouseBtn = mouseBtn & 0xfe; break;
        case Qt::RightButton: mouseBtn = mouseBtn & 0xfb; break;
        case Qt::MidButton: mouseBtn = mouseBtn & 0xfd; break;
        default: break;
    }

    mouseMsg.insert(1, mouseBtn);
    writeToSocket(mouseMsg);
}

bool VNCClient::isReady()
{
    return _isReady;
}

quint16 VNCClient::convertQtKeyToX11Key(QKeyEvent *key)
{
    quint32 keysym = 0xffffffff;

    if (_keymap.contains(key->key()))
    {
        keysym = _keymap.value(key->key());
    }
    else
    {
        QString keyText = key->text();
        QChar symbol = keyText.at(0);
        keysym = symbol.unicode();
    }

    return keysym;
}

void VNCClient::SplitVncLoginParameters(QString &loginInfo, QString *login, QString *passwd, QString *ip, QString *port)
{
    //
    //  Format: - login:password@ip:port
    //

    QStringList parts = loginInfo.split("@");

    if (2 == parts.count())
    {
        QString logInfo = parts[0];
        QString vncAddress = parts[1];

        // split login and password
        if (!logInfo.isEmpty())
        {
            parts = logInfo.split(":");

            if (2 == parts.count())
            {
                *login = parts[0];
                *passwd = parts[1];
            }
            else
            {
                *passwd = parts[0];
            }
        }

        // split ip and port
        if (!vncAddress.isEmpty())
        {
            parts = vncAddress.split(":");

            if (2 == parts.count())
            {
                *ip = parts[0];
                *port = parts[1];
            }
            else
            {
                *port = parts[0];
            }
        }
    }
    else
    {
        loginInfo = parts[0];

        if (!loginInfo.isEmpty())
        {
            parts = loginInfo.split(":");

            *ip = parts[0];
            *port = parts[1];
        }
    }

}
