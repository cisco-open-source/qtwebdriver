#include "extension_qt/vncclient.h"

#include <iostream>
#include <QtNetwork/QHostAddress>
#include <QtCore/QMap>
#include <QtCore/QRegExp>

#define MAJOR_INDEX 6
#define MINOR_INDEX 10

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

    std::cout << "#### Read from socket: " << data.data() << std::endl;
    for (int i=0; i<data.size(); i++)
        std::cout << data.at(i) << " ";
    std::cout << std::endl;

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
        // todo
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

QByteArray VNCClient::readSocket(qint64 size)
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
        // todo
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
        std::cout << "#### Socket isn't in connected state. Couldn't write to socket" << std::endl;
    }

    return bytesNmb;
}

void VNCClient::onStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        std::cout << "#### Socket state: UnconnectedState" << std::endl; break;
    case QAbstractSocket::HostLookupState:
        std::cout << "#### Sockte state: HostLookupState" << std::endl; break;
    case QAbstractSocket::ConnectingState:
        std::cout << "#### Socket state: ConnectingState" << std::endl; break;
    case QAbstractSocket::ConnectedState:
        std::cout << "#### Socket state: ConnectedState" << std::endl; break;
    case QAbstractSocket::BoundState:
        std::cout << "#### Socket state: BoundState" << std::endl; break;
    case QAbstractSocket::ClosingState:
        std::cout << "#### Socket state: ClosingState" << std::endl; break;
    case QAbstractSocket::ListeningState:
        std::cout << "#### Socket state: ListeningState" << std::endl; break;
    }
}

void VNCClient::onError(QAbstractSocket::SocketError error)
{
    switch (error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        std::cout << "#### Socket error: ConnectionRefusedError" << std::endl; break;
    case QAbstractSocket::RemoteHostClosedError:
        std::cout << "#### Socket error: RemoteHostClosedError" << std::endl; break;
    case QAbstractSocket::HostNotFoundError:
        std::cout << "#### Socket error: HostNotFoundError" << std::endl; break;
    case QAbstractSocket::SocketAccessError:
        std::cout << "#### Socket error: SocketAccessError" << std::endl; break;
    case QAbstractSocket::SocketResourceError:
        std::cout << "#### Socket error: SocketResourceError" << std::endl; break;
    case QAbstractSocket::SocketTimeoutError:
        std::cout << "#### Socket error: SocketTimeoutError" << std::endl; break;
    case QAbstractSocket::DatagramTooLargeError:
        std::cout << "#### Socket error: DatagramTooLargeError" << std::endl; break;
    case QAbstractSocket::NetworkError:
        std::cout << "#### Socket error: NetworkError" << std::endl; break;
    case QAbstractSocket::AddressInUseError:
        std::cout << "#### Socket error: AddressInUseError" << std::endl; break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        std::cout << "#### Socket error: SocketAddressNotAvailableError" << std::endl; break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        std::cout << "#### Sockte error: UnsupportedSocketOperationError" << std::endl; break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        std::cout << "#### Socket error: ProxyAuthenticationRequiredError" << std::endl; break;
    case QAbstractSocket::SslHandshakeFailedError:
        std::cout << "#### Socket error: SslHandshakeFailedError" << std::endl; break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        std::cout << "#### Socket error: UnfinishedSocketOperationError" << std::endl; break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        std::cout << "#### Socket error: ProxyConnectionRefusedError" << std::endl; break;
    case QAbstractSocket::ProxyConnectionClosedError:
        std::cout << "#### Socket error: ProxyConnectionClosedError" << std::endl; break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        std::cout << "#### Socket error: ProxyConnectionTimeoutError" << std::endl; break;
    case QAbstractSocket::ProxyNotFoundError:
        std::cout << "#### Socket error: ProxyNotFoundError" << std::endl; break;
    case QAbstractSocket::ProxyProtocolError:
        std::cout << "#### Socket error: ProxyProtocolError" << std::endl; break;
    case QAbstractSocket::UnknownSocketError:
        std::cout << "#### Socket error: UnknownSocketError" << std::endl; break;
    }
}

bool VNCClient::establishProtocolVersion(QByteArray &data)
{
    char *version = data.data();
    int major = (int)version[MAJOR_INDEX] - 0x30;
    int minor = (int)version[MINOR_INDEX] - 0x30;

    if (3 != major && (3 != minor || 7 != minor || 8 != minor))
    {
        std::cout << "#### Return false, version: " << major << "." << minor << std::endl;
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
                    return false;
                    break;
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
                    _establishedSecurity = None;
                    return true;
                    break;
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
                return false;
                break;
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

    return false;
}

bool VNCClient::passAutentication(QByteArray &data)
{
    return false;
}

bool VNCClient::finishHandshaking(QByteArray &data)
{
    bool result = false;
    int response  = data.at(0)*0x1000000 + data.at(1)*0x10000 + data.at(2)*0x100 + data.at(3);

    if (0 == response)
    {
        _handshakeFinished = true;
        char zero = 0x00;
        QByteArray initMsg(1, zero);
        writeToSocket(initMsg);
        result = true;
    }
    else
    {
        QByteArray error("Security type not established");
        handleZeroError(error);
        result = false;
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

    std::cout << "#### Error: " << msg << std::endl;

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

    std::cout << "#### Send key message: " << std::endl;
    for (int i=0; i<keyMsg.size(); i++)
    {
        std::cout << (int)keyMsg.at(i) << " ";
    }
    std::cout << std::endl;

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
