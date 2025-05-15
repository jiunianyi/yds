#ifndef OPUS_HXX
#define OPUS_HXX
#include "qaudioformat.h"
#include "qbuffer.h"
#include "qwindowdefs.h"
#include <QObject>
#include <QAudioSink>
#include <QAudioSource>
#include <QUdpSocket>
#include <QIODevice>
#include <QTimer>
#include<opus.h>
#include<QAudioFormat>
#include <QNetworkDatagram>
#include<QBuffer>
#include<QObject>
class opus: public QObject
{
 Q_OBJECT

public:
    static opus& getInstance() {
        static opus instance; // 局部静态变量
        return instance;
    }
     opus();
    void extracted();
    QAudioFormat getAudioFormat();
    void initOpusEncoder();
private slots:
    void sendAudioData();
public slots:
    void start(int &id,int &touid);
    void onReadyRead(QByteArray &arr);
     void stop();
private:
    OpusDecoder *decoder;
    OpusEncoder *encoder = nullptr;
    /* QByteArray inputData; */      // 存储麦克风输入数据
    QBuffer inputBuffer;        // 输入缓冲区（只写）
    QByteArray outputData;      // 存储网络接收的音频数据
    QBuffer outputBuffer;       // 输出缓冲区（只读）
    int k;
    int id;
    int touid;
    QUdpSocket *uSocket ;
    QAudioFormat audioFormat;
    QAudioSource *audioSource;
    QAudioSink *audioSink;
    QIODevice *audioInputDevice;
    QIODevice *audioOutputDevice;
    QTimer *sendTimer;
    QByteArray inputData;
    const QString backendIp;
    const quint16 backendPort;
};

#endif // OPUS_HXX
