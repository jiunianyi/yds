#include "udpmgr.hxx"
#include "global.h"
#include<QNetworkDatagram>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include"accref.hxx"
#include"selectbtn.hxx"
#include"opus.hxx"
#include"huat.hxx"
#include"video.hxx"
#include"spsele.hxx"
udpmgr::udpmgr(QObject *parent):udpsocket(new QUdpSocket(this)),m_heartbeatTimer(new QTimer(this))
{
    qDebug()<<"sssss";
    connect(this,&udpmgr::sig_sele_yuyin,&accref::getInstance(),&accref::swad);
    connect(&accref::getInstance(),&accref::ref_yubyin,this,&udpmgr::slot_ref);
    //语音
    connect(&selectbtn::getInstance(),&selectbtn::sig_yuyin_req,this,&udpmgr::send_yuyin_connect);
    connect(this,&udpmgr::close_sele,&selectbtn::getInstance(),&selectbtn::slot_close_sele);
    connect(&accref::getInstance(),&accref::accp_yuyin,this,&udpmgr::slot_jinxth);
    connect(this,&udpmgr::sig_yun,&opus::getInstance(),&opus::start);
    connect(this,&udpmgr::end_yy,&opus::getInstance(),&opus::onReadyRead);
    connect(&HuaT::getInstance(),&HuaT::sendgd,this,&udpmgr::send_gd);
    connect(this,&udpmgr::yygd,&opus::getInstance(),&opus::stop);
    //视频
    connect(&video::getInstance(),&video::sig_shipin_req,this,&udpmgr::send_yuyin_connect);
    connect(this,&udpmgr::sig_sele_shipin,&spsele::getInstance(),&spsele::swsps);
    connect(&spsele::getInstance(),&spsele::accship,this,&udpmgr::slot_acc_ship);
    connect(this,&udpmgr::accshipin,&video::getInstance(),&video::accsendsp);
}

void udpmgr::sendHeartbeat()
{
    QJsonObject obj;
    obj["id"] = ID_HEARD;
    obj["uid"] = uid;
    QJsonDocument doc(obj);
    QByteArray jsondata = doc.toJson(QJsonDocument::Compact);
    udpsocket->writeDatagram(jsondata,QHostAddress(backendIp),backendPort);
}

void udpmgr::star()
{
    m_heartbeatTimer->start(5000);
}

void udpmgr::slot_udp_connect(ServerInfo si)
{
    backendIp = si.Host;
   // backendPort = static_cast<quint16>(si.Port.toUInt());
    backendPort = 8898;
    qDebug()<<backendIp<<backendPort;
    // if (!udpsocket->bind(QHostAddress("47.101.36.202"), 6667)) {
    //     qDebug() << "Failed to bind socket:" << udpsocket->errorString();
    //     return;
    // }
    udpsocket->connectToHost(QHostAddress("47.101.36.202"), backendPort);
    //注册
    uid = si.uid;
    QJsonObject obj;
    obj["uid"] = si.uid;
    obj["id"] = ID_REGUSER_UDP;
    QJsonDocument doc(obj);
    QByteArray jsondata  = doc.toJson(QJsonDocument::Compact);
    //   qDebug()<<uid;
    // QByteArray packet(reinterpret_cast<char*>(&heard), sizeof(heard));
    qint64 bytesSent = udpsocket->writeDatagram(jsondata, QHostAddress("47.101.36.202"), backendPort);
     if (bytesSent == -1) {
         qDebug() << "Failed to send datagram:" << udpsocket->errorString();
     } else {
         qDebug() << "Sent " << bytesSent << " bytes.";
     }
   // 连接定时器信号到槽
    connect(m_heartbeatTimer, &QTimer::timeout, this, &udpmgr::sendHeartbeat);
    star();
    connect(udpsocket,&QUdpSocket::readyRead,this,&udpmgr::onready);
}

void udpmgr::send_yuyin_connect(QByteArray arr)
{
    qDebug()<<"正在打电话给对方"<<arr;
    qDebug()<<"端口:"<<backendPort;
    qint64 bytesSent =udpsocket->writeDatagram(arr,QHostAddress("47.101.36.202"),backendPort);
    if (bytesSent == -1) {
        qDebug() << "Failed to send datagram:" << udpsocket->errorString();
    } else {
        qDebug() << "Sent " << bytesSent << " bytes.";
    }

}

void udpmgr::onready()
{
    while(udpsocket->hasPendingDatagrams())
    {
        qDebug()<<"读到数据";
        QNetworkDatagram datagram = udpsocket->receiveDatagram();
        QByteArray Data = datagram.data();

        QJsonDocument jsondoc = QJsonDocument::fromJson(Data);
        QJsonObject obj = jsondoc.object();
        qDebug() << obj["id"];
        int id = obj["id"].toInt();
        if(id==ID_SELECT_SHIPIN_REQ)
        {
            fromuid = obj["fromuid"].toInt();
            touid = obj["touid"].toInt();
      
            emit sig_sele_shipin();
        }
        if(id==ID_SELECT_YUYIN_REQ)
        {
            fromuid = obj["fromuid"].toInt();
            touid = fromuid;
            qDebug()<<fromuid;
            qDebug()<<touid;
            qDebug() << "1111111111";
            emit sig_sele_yuyin();
        }
        if(id == ID_REFUSE_YUYIN_REQ)
        {
            qDebug() << "2222222";
            emit close_sele();
        }
        if(id==ID_ACCEPT_YUNYI_REQ)
        {
            HuaT::getInstance().show();
            selectbtn::getInstance().hide();
            int iid=ID_YUYIN;
            int uid = touid;
            touid = selectbtn::getInstance().touid;
            qDebug()<<selectbtn::getInstance().touid<<"123456789";
            emit sig_yun(iid,selectbtn::getInstance().touid);
        }
        if(id == ID_YUYIN)
        {
            if (!obj.contains("context") || !obj["context"].isString()) {
                qDebug() << "JSON does not contain valid 'context' field";
                return;
            }
            QString base64Encoded = obj["context"].toString();
            QByteArray encodedBytes = QByteArray::fromBase64(base64Encoded.toUtf8());
            emit end_yy(encodedBytes);
        }
        if(id==ID_ACCEPT_SHIPIN_REQ)
        {
            touid = obj["fromuid"].toInt();
            fromuid = obj["touid"].toInt();
            qDebug() << "对方已经同意fromuid" << fromuid << "touid" << touid;
            emit accshipin();
        }
        if(id==ID_SHIPIN)
        {
            if (!obj.contains("content") || !obj["content"].isString()) {
                qDebug() << "JSON does not contain valid 'content' field";
                return;
            }
            QString shipindata = obj["content"].toString();
            if (shipindata.isEmpty()) {
                qCritical() << "接收到的content字段为空";
                return;
            }
            QByteArray encodedBytes = QByteArray::fromBase64(shipindata.toLatin1(), QByteArray::Base64Encoding);
            if (encodedBytes.isEmpty()) {
                qWarning() << "Base64解码失败或数据为空";
                return;
            }
            int size = obj["size"].toInt();
            data += encodedBytes;
            if (size == 0)
            {
                qDebug() << "2w2w2w2w2w2w";
                emit shipdata(data);
                data.clear(); // 清空数据
            }
        }
        if(id==ID_YYGUADUAN)
        {
            HuaT::getInstance().hide();
            emit yygd();
        }
    }
}

void udpmgr::slot_ref()
{
    QJsonObject obj;
    obj["id"] =ID_REFUSE_YUYIN_REQ;
    obj["touid"] = fromuid;
    QJsonDocument doc(obj);
    QByteArray arr = doc.toJson(QJsonDocument::Compact);
   qint64 bytesSent =udpsocket->writeDatagram(arr,QHostAddress("47.101.36.202"),backendPort);
    if (bytesSent == -1) {
        qDebug() << "Failed to send datagram:" << udpsocket->errorString();
    } else {
        qDebug() << "Sent " << bytesSent << " bytes.";
    }
}

void udpmgr::slot_jinxth()
{
    int id =ID_ACCEPT_YUNYI_REQ;
    QJsonObject obj;
    obj["id"] = id;
    obj["touid"] = fromuid;
    QJsonDocument doc(obj);
    QByteArray arr = doc.toJson(QJsonDocument::Compact);
    qint64 bytesSent =udpsocket->writeDatagram(arr,QHostAddress("47.101.36.202"),backendPort);
    if (bytesSent == -1) {
        qDebug() << "Failed to send datagram:" << udpsocket->errorString();
    } else {
        qDebug() << "Sent " << bytesSent << " bytes.";
    }

}

void udpmgr::slot_acc_ship()
{
    QJsonObject obj;
    obj["id"] = ID_ACCEPT_SHIPIN_REQ;
    obj["fromuid"] = touid;
    obj["touid"] = fromuid;
    qDebug()<<"已经接受touid"<<touid<<"fromuid"<<fromuid;
    QJsonDocument doc(obj);
    QByteArray arr = doc.toJson(QJsonDocument::Compact);
    qint64 bytesSent =udpsocket->writeDatagram(arr,QHostAddress("47.101.36.202"),backendPort);
    if (bytesSent == -1) {
        qDebug() << "Failed to send datagram:" << udpsocket->errorString();
    } else {
        qDebug() << "Sent " << bytesSent << " bytes.";
    }

}

void udpmgr::send_shisj(AVPacket *avPacket)
{
    qDebug() << "sss" << fromuid << "sss" << touid;
    QJsonObject obj;
    obj["id"] = ID_SHIPIN;
    obj["touid"] = touid;
    obj["fromuid"] = fromuid;
    int dataSize = avPacket->size;
    uint8_t *dataPtr = avPacket->data;
    QByteArray datagram(reinterpret_cast<char*>(dataPtr), dataSize);
    qDebug() << "大小" << dataSize;
    const int maxPacketSize = 500; // 避免 IP 分片
    int currentSize = 0;
     while (dataSize > 0) {
         if (dataSize < 500) obj["size"] = 0;
         else obj["size"] = dataSize;
             currentSize = qMin(dataSize, maxPacketSize);
            QByteArray datagram(reinterpret_cast<char*>(dataPtr), currentSize);
            obj["content"] = QString::fromLatin1(datagram.toBase64());
            QJsonDocument doc(obj);
            QByteArray arr = doc.toJson(QJsonDocument::Compact);
            udpsocket->writeDatagram(arr, QHostAddress("47.101.36.202"), backendPort);
             dataPtr += currentSize;
             dataSize -= currentSize;
           
         }
    av_packet_unref(avPacket);
    av_packet_free(&avPacket);
}

void udpmgr::send_gd()
{
    qDebug() << "touidssss" << touid;
    QJsonObject obj;
    obj["id"] = ID_YYGUADUAN;
    obj["touid"] = touid;
    QJsonDocument doc(obj);
    QByteArray arr = doc.toJson(QJsonDocument::Compact);
    udpsocket->writeDatagram(arr, QHostAddress("47.101.36.202"), backendPort);
}



