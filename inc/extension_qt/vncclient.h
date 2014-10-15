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

/*! \file vncclient.h
    \page vncclient
  vncclient - WebDriver module, which allows connect to VNC server.
  It works via RFB protocol (http://www.realvnc.com/docs/rfbproto.pdf)
  and allow to do coonection, authentication and
  send key and mouse events. It is allowed to create only
  one client instance.
*/

#ifndef VNCCLIENT_H
#define VNCCLIENT_H

#include <QtNetwork/QTcpSocket>
#include <QtGui/QKeyEvent>
#include <QtCore/QMap>

#include "vncserverparameters.h"
#include "webdriver_logging.h"

/**
  VNCClient - class provides main functionality of VNC client.
*/
class VNCClient : public QObject
{
    Q_OBJECT

public:

    /**
      Destructor
    */
    ~VNCClient();

    /**
      Returns VNCClient instance. Will create new one, if it not exist
      @return pointer to @sa VNCClient
    */
    static VNCClient* getInstance();

    /**
      Splits vnc command line parameter to separate variables
      @param [in] loginInfo - string with all login info
      @param [out] login - string with login
      @param [out] passwd - string with password
      @param [out] ip - string with ip address of VNC server
      @param [out] port - string with port number of VNC server
    */
    static void SplitVncLoginParameters(QString &loginInfo, QString *login, QString *passwd, QString *ip, QString *port);

    /**
      Initialize VNC client connection without authentication
      @param remoteHost - address of VNC server
      @param port - port number of VNC server
      @return true if client initialized, else false
    */
    bool Init(QString remoteHost, quint16 port);

    /**
      Initialize VNC client connection with authentication
      @param remoteHost - address of VNC server
      @param port - port number of VNC server
      @param password - authentication password
      @return true if client initialized, else false
    */
    bool Init(QString remoteHost, quint16 port, QString* password);

    /**
      Sends Qt key event to VNC server
      @param key - pointer to QKeyEvent instance
    */
    void sendKeyEvent(QKeyEvent *key);

    /**
      Sends Qt mouse event to VNC server
      @param mouse - pointer to QMouseEvent instance
    */
    void sendMouseEvent(QMouseEvent *mouse);

    /**
      Indicates whether VNC client is initialized
      @return true, if client initialized, else false
    */
    bool isReady();

public slots:

    /**
      reads data from socket
      @return array with data
    */
    QByteArray readSocket();
    // QByteArray readSocket(qint64 size);

    /**
      writes data to socket
      @param data - byte array with data for writing
      @return number of actuall written bytes
    */
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
    /**
      enumeration with encodings
     */
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

    /**
      enumeration with message types
     */
    enum ClientMsg {
        SetPixelFormat = 0,
        FixColourMapEntries = 1,
        SetEncodings = 2,
        FramebufferUpdateRequest = 3,
        KeyEvent = 4,
        PointerEvent = 5,
        ClientCutText = 6
    };

    /**
      Map with keycodes correspondences between Qt and RFB protocol
     */
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
