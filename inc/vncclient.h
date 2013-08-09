#ifndef VNCCLIENT_H
#define VNCCLIENT_H

#include <QtNetwork/QTcpSocket>
#include <QtGui/QKeyEvent>
#include <QtCore/QMap>

#include "vncserverparameters.h"
#include "webdriver_logging.h"

class VNCClient : public QObject
{
    Q_OBJECT

public:
    ~VNCClient();

    static VNCClient* getInstance();
    static void SplitVncLoginParameters(QString &loginInfo, QString *login, QString *passwd, QString *ip, QString *port);

    bool Init(QString remoteHost, quint16 port);
    bool Init(QString remoteHost, quint16 port, QString* password);
    void sendKeyEvent(QKeyEvent *key);
    void sendMouseEvent(QMouseEvent *mouse);
    bool isReady();

public slots:
    QByteArray readSocket();
    // QByteArray readSocket(qint64 size);
    qint64 writeToSocket(QByteArray& data);

private slots:
    void onStateChanged(QAbstractSocket::SocketState state);
    void onError(QAbstractSocket::SocketError error);

private:
    VNCClient();

    bool establishProtocolVersion(QByteArray& data);
    bool establishSecurity(QByteArray& data);
    bool passAutentication(QByteArray& data);
    bool finishHandshaking(QByteArray& data);
    bool initServerParameters(QByteArray& data);
    void sendDoubleClick(QMouseEvent *event);

    void handleZeroError(QByteArray& data);
    quint16 convertQtKeyToX11Key(QKeyEvent *key);

public:
    enum Encodings {
        Invalid = 0,
        None = 1,
        VNCAuthentication = 2,
        RA2 = 5,
        RA2ne = 6,
        Tight = 16,
        Ultra = 17,
        TLS = 18,
        VeNCrypt = 19,
        GTK_VNC_SASL = 20,
        MD5_hash_athentication = 21,
        ColinDean_xvp = 22
    };

    enum ClientMsg {
        SetPixelFormat = 0,
        FixColourMapEntries = 1,
        SetEncodings = 2,
        FramebufferUpdateRequest = 3,
        KeyEvent = 4,
        PointerEvent = 5,
        ClientCutText = 6
    };

    static QMap<quint32, quint16> _keymap;

private:
    static VNCClient *_instance;
    QTcpSocket *_socket;
    bool _versionEstablished;
    bool _securityEstablished;
    bool _autenticationPassed;
    bool _handshakeFinished;
    bool _communicationError;
    bool _isReady;
    int _establishedVersion;
    Encodings _establishedSecurity;
    ServerParameters* _serverParameters;
    webdriver::StdOutLog *_logger;

    QString *_password;
};

#endif // VNCCLIENT_H
