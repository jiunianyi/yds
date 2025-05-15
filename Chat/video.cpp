#include "video.hxx"
#include "ui_video.h"
#include<QMediaDevices>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include"usermgr.h"
#include"global.h"
video::video(
    QWidget *parent)
    : QDialog(parent)
    , captureSession(new QMediaCaptureSession(this))
    ,camera(new QCamera(QMediaDevices::defaultVideoInput(), this)),
    videoSink(new QVideoSink(this)),udpsocket(new QUdpSocket(this)),

    ui(new Ui::video)
{
    ui->setupUi(this);
}

video::~video()
{
    delete ui;
    camera->stop();
}

void video::initvideo()
{
    sendselemsg();
    camera->start();
    // 配置媒体捕获会话
    captureSession->setCamera(camera);
    captureSession->setVideoSink(videoSink);
    connect(videoSink,&QVideoSink::videoFrameChanged,this,&video::showlocimage);
}

void video::sendship()
{
    qDebug() << "不能出现";
    camera->start();
    // 配置媒体捕获会话
    captureSession->setCamera(camera);
    captureSession->setVideoSink(videoSink);
    connect(videoSink,&QVideoSink::videoFrameChanged,this,&video::showsendlocimage);
}

void video::sendselemsg()
{
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    QJsonObject obj;
    obj["touid"] = touid;
    obj["fromuid"] = user_info->_uid;
    obj["id"] = ID_SELECT_SHIPIN_REQ;
    QJsonDocument doc(obj);
    QByteArray jsondata = doc.toJson(QJsonDocument::Compact);
    emit sig_shipin_req(jsondata);
}

void video::showlocimage(const QVideoFrame &frame)
{
    if (!frame.isValid())
    {
        qDebug()<<"1111";
        return;
    }
    // 将 QVideoFrame 转换为 QImage
    QImage image = frame.toImage();
    if (image.isNull())
    {
        qDebug()<<"222";
        return;
    }
    // 缩放图像以适应 QLabel
    QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->self_lb->size(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
    ui->self_lb->setPixmap(pixmap);
}

void video::showsendlocimage(const QVideoFrame &frame)
{
    if (!frame.isValid())
    {
        qDebug()<<"1111";
        return;
    }
    // 将 QVideoFrame 转换为 QImage
    QImage image = frame.toImage();
    if (image.isNull())
    {
        qDebug()<<"222";
        return;
    }
    // 缩放图像以适应 QLabel
    QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->self_lb->size(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
    ui->self_lb->setPixmap(pixmap);
    emit ffimage(image);

}

void video::getimage(const QVideoFrame &frame)
{
    if (!frame.isValid())
    {
        qDebug()<<"1111";
        return;
    }
    // 将 QVideoFrame 转换为 QImage
    QImage image = frame.toImage();
    if (image.isNull())
    {
        qDebug()<<"222";
        return;
    }
    emit ffimage(image);
}

void video::showdecspdata(const QImage &image)
{
    QPixmap pixmap = QPixmap::fromImage(image).scaled(ui->other_lb->size(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);
    ui->other_lb->setPixmap(pixmap);
}

void video::accsendsp()
{
    qDebug() << "也不能出现";
    connect(videoSink,&QVideoSink::videoFrameChanged,this,&video::getimage);
}
