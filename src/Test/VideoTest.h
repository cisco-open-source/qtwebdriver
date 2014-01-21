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
