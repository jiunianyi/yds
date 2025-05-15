#include "opus.hxx"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include <QMediaDevices>
#include<QBuffer>
#include"udpmgr.hxx"
#include"huat.hxx"
#include<QMessageBox>
opus::opus():
    inputBuffer(&inputData),
    outputBuffer(&outputData),
    uSocket(new QUdpSocket()),
    audioInputDevice(nullptr),
    audioOutputDevice(nullptr) ,
    sendTimer(new QTimer()),
    backendIp("47.101.36.202"),
   backendPort(8898)
{
   // uSocket->connectToHost(backendIp,backendPort);
    decoder = opus_decoder_create(16000, 1, nullptr);
    audioFormat = getAudioFormat();
    // 设置音频输出
    initOpusEncoder();

    // 初始化输入缓冲区（只写）
    inputBuffer.open(QIODevice::WriteOnly);



    // 设置音频输入设备（麦克风→inputBuffer）
    audioSource = new QAudioSource(audioFormat);


    // 设置音频输出设备
    audioSink = new QAudioSink(audioFormat);


    connect(&HuaT::getInstance(),&HuaT::guaduan,this,&opus::stop);

    //读取音频数

}
QAudioFormat  opus::getAudioFormat()
{
    QAudioDevice audioDevice = QMediaDevices::defaultAudioOutput();
    QAudioFormat format;
    format.setSampleRate(16000); // 设置采样率为 48000 Hz
    format.setChannelCount(1);   // 设置为单声道
    format.setSampleFormat(QAudioFormat::Int16); // 设置样本格式为 16 位有符号整数
    if (!audioDevice.isFormatSupported(format)) {
        qWarning() << "音频格式不受支持，尝试使用默认格式";
        format = audioDevice.preferredFormat();
    }
    return format;
}

void opus::start(int &id, int &touid)
{
    qDebug()<<touid;
    audioInputDevice = audioSource->start();
    audioOutputDevice =  audioSink->start();
   // connect(udpmgr::getInstance().udpsocket, &QUdpSocket::readyRead, this, &opus::onReadyRead);
    //connect(uSocket, &QUdpSocket::readyRead, this, &opus::onReadyRead);

    // 设置发送定时器，定期读取音频数据并发送
    connect(sendTimer, &QTimer::timeout, this, &opus::sendAudioData);
    this->id = id;
    this->touid = touid;
    if (audioSource && !sendTimer->isActive()) {
        sendTimer->start(20); // 每 30 毫秒发送一次
        qDebug() << "音频传输已启动";
    }

}

void opus::stop()
{
    if (audioSource) {
        audioSource->stop();
    }

    if (audioSink) {
        audioSink->stop();
    }

    
    if (sendTimer->isActive()) {
        sendTimer->stop();
    }

    qDebug() << "音频传输已停止";
}

void opus::initOpusEncoder()
{
    qDebug()<<"初始化opus水水水水水水水水水水";
    int error;
    encoder =opus_encoder_create(16000,1,OPUS_APPLICATION_AUDIO,&error);
    if(error<0)
    {
        qDebug()<<"失败去创建Opus encoder:"<<opus_strerror(error);
        return;
    }
    opus_encoder_ctl(encoder,OPUS_SET_BITRATE(32000));

}


void opus::onReadyRead(QByteArray &arr)
{
    qDebug() << "Received data";
    const int maxFrameSize = 320; // 对于 20ms 的帧，采样率为 48kHz 时最大帧大小为 960，你需要根据实际的采样率和帧长进行调整


        const unsigned char* encodedData = reinterpret_cast<const unsigned char*>(arr.constData());
        opus_int32 ssize = arr.size();


        // 分配足够的缓冲区来存储解码后的 PCM 数据
        QByteArray decodedData;
        decodedData.resize(maxFrameSize * sizeof(opus_int16));

        // 直接进行解码
        int sampleCount = opus_decode(decoder,encodedData,ssize,
                                      (opus_int16*)decodedData.data(), maxFrameSize, 0);
        if (sampleCount < 0) {
            qDebug() << "Opus decode failed:" << opus_strerror(sampleCount);
        }
        qDebug()<<"解码后"<<sampleCount;

        // 根据实际解码的样本数调整缓冲区大小
        decodedData.resize(sampleCount * sizeof(opus_int16));

        // 更新输出缓冲区用于音频播放
        qint64 bytesWritten = audioOutputDevice->write(decodedData);
        if (bytesWritten != decodedData.size()) {
            qDebug() << "Failed to write all decoded data to audio output";
        }

    }


// 前端发送语音数据到后端
void opus::sendAudioData()
{
    // qDebug()<<"读取音频成功"<<k++;
    const int frameSize = 320; // 20 ms * 16000 Hz, 对应于 320 个采样点

    // 从 audioInputDevice 读取所有可用的数据
    QByteArray newData = audioInputDevice->readAll();
    inputData.append(newData);

    // 当缓存的数据足够一个帧时，进行处理
    while (inputData.size() >= frameSize * sizeof(opus_int16)) {
        // 提取一个帧的数据
        QByteArray frameData = inputData.left(frameSize * sizeof(opus_int16));
        inputData.remove(0, frameSize * sizeof(opus_int16));

        // 将 QByteArray 转换为 opus_int16* 类型的 PCM 数据
        const opus_int16* pcm = reinterpret_cast<const opus_int16*>(frameData.constData());

        // Opus 编码
        unsigned char encodedData[1024];
        int nbBytes = opus_encode(encoder, pcm, frameSize, encodedData, sizeof(encodedData));
        if (nbBytes < 0) {
            qDebug() << "Opus encode failed:" << opus_strerror(nbBytes);
            return;
        }
        QFile file("audio_output.opus");
        if (file.open(QIODevice::Append)) {
            file.write(reinterpret_cast<char*>(encodedData), nbBytes);  // 将编码后的数据追加到文件中
            file.close();
        }
        // 将编码后的数据转换为 QByteArray
        QByteArray encodedBytes(reinterpret_cast<char*>(encodedData), nbBytes);
        // 进行 Base64 编码
        QString base64Encoded = encodedBytes.toBase64();
        QJsonObject obj;
        id = ID_YUYIN;
        obj["id"] = id;
        obj["touid"] = touid;
        obj["context"] = base64Encoded;
        qDebug() << "Encoded Data (Base64):" << base64Encoded;
      //  QByteArray pack(reinterpret_cast<char*>(&obj),obj.size());
        QJsonDocument doc(obj);
        QByteArray jsondata  = doc.toJson(QJsonDocument::Compact);
        // yyheard heard;
        // heard.uid=uuuud;
        // const char* uid = nullptr  ;
        // QByteArray byteArray = ud.toUtf8();
        // uid = byteArray.constData();
        //QByteArray pack(reinterpret_cast<char*>(&heard),sizeof(heard));
        //pack.append(reinterpret_cast<char*>(encodedData), nbBytes);
        // 通过 UDP 发送编码后的数据
        //  QByteArray encodedByteArray(reinterpret_cast<char*>(encodedData), nbBytes);
        udpmgr::getInstance().udpsocket->writeDatagram(jsondata, QHostAddress(backendIp), backendPort);
        //qDebug() << "Sending encoded voice data to backend, size:" << nbBytes;

    }
}
