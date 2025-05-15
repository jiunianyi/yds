#ifndef UDPMGR_HXX
#define UDPMGR_HXX
#include "global.h"
extern "C" {
#include "libavcodec/packet.h"
}
#include<QString>
#include<QObject>
#include<QUdpSocket>
#include<QTimer>
class udpmgr: public QObject
{
    Q_OBJECT
public:
    static udpmgr& getInstance() {
        static udpmgr instance; // 局部静态变量
        return instance;
    }
    explicit udpmgr(QObject *parent = nullptr);
    void sendHeartbeat();
    void star();
        QUdpSocket *udpsocket;
private:
    QString backendIp;
     quint16 backendPort;
     netwheard heard;
    QTimer* m_heartbeatTimer; // 心跳定时器
    QByteArray data;
 public:
     int uid;
    int fromuid;
     int touid;

signals:
     void sig_sele_yuyin();
    void sig_sele_shipin();
    void close_sele();
     void sig_yun(int &id,int &touid);
     void end_yy(QByteArray &arr);
     void accshipin();
     void shipdata(const QByteArray &data);
     void yygd();
public slots:
    void slot_udp_connect(ServerInfo si);
    void send_yuyin_connect(QByteArray arr);
     void onready();
    void slot_ref();
     void slot_jinxth();
    void slot_acc_ship();
     void send_shisj(AVPacket *avPacket);
    void send_gd();
};

#endif // UDPMGR_HXX
