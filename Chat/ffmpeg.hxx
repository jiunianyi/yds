#ifndef FFMPEG_HXX
#define FFMPEG_HXX
#include "qudpsocket.h"
#include<QObject>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
class ffmpeg:public QObject
{
    Q_OBJECT
public:
    ffmpeg();
    ~ffmpeg();
private:
    void decffmpeg();

private slots:
    void processFrame(const QImage &image);
        void decodePacket(const QByteArray &data);
public slots:
    void initffmpeg();
signals:
    void decimage(const QImage &image );
    void sendsosj(AVPacket *avPacket);
private:

    // UDP 传输相关
    QUdpSocket *udpSocket;
    QHostAddress udpHost;
    quint16 udpPort;
    // FFmpeg 编码相关
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *avFrame;
    AVPacket *avPacket;
    struct SwsContext *swsCtx;
    //解码
    const AVCodec *decoder;
    AVCodecContext *decoderContext;
    AVFrame *decoderFrame;
    AVPacket *decoderPacket;
    SwsContext *swsDecoderCtx;

    // 帧大小
    int frameWidth;
    int frameHeight;
    int64_t frameIndex = 0;
    // 缓存
    QByteArray encodedData;
};

#endif // FFMPEG_HXX
