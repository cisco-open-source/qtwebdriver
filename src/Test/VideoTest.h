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

#ifndef VIDEOTEST_H
#define VIDEOTEST_H

#include <QtWidgets/QtWidgets>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtNetwork/QNetworkRequest>

class VideoTestWidget;
class QAbstractButton;
class QMediaPlayer;
class QVideoWidget;

class VideoTestWidget : public QMainWindow
{
    Q_OBJECT
private:
    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
    QAbstractButton* playButton;
    QSlider *positionSlider;
    QSlider *volumeSlider;
public slots:
    void play();
private slots:
    void setVolume(int);
    void setPlayingPosition(int);
    void durationChanged(qint64);
    void positionChanged(qint64);
    void volumeChanged(int);
public:
    explicit VideoTestWidget(QWidget *parent = 0);
    ~VideoTestWidget();
    
};

#endif // VIDEOTEST_H
