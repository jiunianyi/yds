#ifndef VIDEO_HXX
#define VIDEO_HXX
#include<QImage>
#include<QCamera>
#include<QMediaCaptureSession>
#include<QVideoSink>
#include <QDialog>
#include<QUdpSocket>

namespace Ui {
class video;
}

class video : public QDialog
{
    Q_OBJECT

public:
    static video& getInstance() {
        static video instance; // 局部静态变量
        return instance;
    }
    explicit video(QWidget *parent = nullptr);
    ~video();
        void initvideo();
    void sendship();
    int touid;
private:
    void sendselemsg();
private slots:
    void showlocimage(const QVideoFrame &frame);
    void showsendlocimage(const QVideoFrame &frame);
    void getimage(const QVideoFrame &frame);
public slots:
    void showdecspdata(const QImage&image);
    void accsendsp();
signals:
    void sig_shipin_req(QByteArray jsondata);
    void ffimage(const QImage &image);
private:
    //摄像头
    QMediaCaptureSession *captureSession;
    QCamera *camera;
    QVideoSink *videoSink;
    //udp
    QUdpSocket *udpsocket;
    QString backendIp;
    quint16 backendPort;

    Ui::video *ui;
};

#endif // VIDEO_HXX
