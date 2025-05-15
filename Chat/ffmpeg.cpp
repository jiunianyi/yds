#include "ffmpeg.hxx"
#include "qdebug.h"
#include "qimage.h"
#include"video.hxx"
#include"udpmgr.hxx"
ffmpeg::ffmpeg():udpSocket(new QUdpSocket(this)),
    codec(nullptr),
    codecContext(nullptr),
    avFrame(nullptr),
    avPacket(nullptr),
    swsCtx(nullptr),
    frameWidth(640),frameHeight(480)
{
    connect(&video::getInstance(),&video::ffimage,this,&ffmpeg::processFrame);
    connect(this,&ffmpeg::sendsosj,&udpmgr::getInstance(),&udpmgr::send_shisj);
    connect(this,&ffmpeg::decimage,&video::getInstance(),&video::showdecspdata);
    connect(&udpmgr::getInstance(),&udpmgr::shipdata,this,&ffmpeg::decodePacket);
}

ffmpeg::~ffmpeg()
{
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }

    if (avFrame) {
        av_frame_free(&avFrame);
    }

    if (avPacket) {
        av_packet_free(&avPacket);
    }

    if (codecContext) {
        avcodec_free_context(&codecContext);
    }

}

void ffmpeg::decffmpeg()
{
    // 2. 初始化解码器
    decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!decoder) {
        qCritical() << "未找到 H.264 解码器";
        return ;
    }

    decoderContext = avcodec_alloc_context3(decoder);
    if (!decoderContext) {
        qCritical() << "无法分配解码器上下文";
        return ;
    }

    int ret = avcodec_open2(decoderContext, decoder, nullptr);
    if (ret < 0) {
        char errBufDec[256];
        av_strerror(ret, errBufDec, sizeof(errBufDec));
        qCritical() << "无法打开解码器:" << errBufDec;
        return ;
    }

    // 分配解码用的 AVFrame
    decoderFrame = av_frame_alloc();
    if (!decoderFrame) {
        qCritical() << "无法分配解码用的 AVFrame";
        return ;
    }

    // 分配解码用的 AVPacket
    decoderPacket = av_packet_alloc();
    if (!decoderPacket) {
        qCritical() << "无法分配解码用的 AVPacket";
        return ;
    }

    // 初始化 SwsContext 用于颜色空间转换（从 YUV420P 转为 RGB24）
    swsDecoderCtx = sws_getContext(
        frameWidth,
        frameHeight,
        AV_PIX_FMT_YUV420P, // 输入为 YUV420P
        frameWidth,
        frameHeight,
        AV_PIX_FMT_RGB24, // 输出为 RGB24
        SWS_BILINEAR,
        nullptr,
        nullptr,
        nullptr
        );
    if (!swsDecoderCtx) {
        qCritical() << "无法初始化解码器的 SwsContext";
        return ;
    }

    qDebug() << "FFmpeg 初始化成功";
    return ;
}

void ffmpeg::decodePacket( const QByteArray &data)
{
    decoderPacket = av_packet_alloc();
    if (!decoderPacket) {
        qCritical() << "无法分配 AVPacket 内存";
        return;  // 立即返回，避免后续崩溃
    }
    if (av_new_packet(decoderPacket, data.size()) < 0) {
        qCritical() << "无法创建AVPacket内存";
        return;
    }
    memcpy(decoderPacket->data, data.constData(), data.size());
    if (!decoderContext || !decoder || !decoderPacket || !decoderFrame) {
        qWarning() << "解码器未正确初始化";
        return;
    }

  

    // 发送数据包到解码器
    int ret = avcodec_send_packet(decoderContext, decoderPacket);
    if (ret < 0) {
        char errBufDecSend[256];
        av_strerror(ret, errBufDecSend, sizeof(errBufDecSend));
        qWarning() << "发送数据包到解码器时出错:" << errBufDecSend;
        av_freep(&decoderPacket->data); //释放掉自己手动分配的内存decoderPacket-data
        // 打印详细错误信息（调试用）
        qDebug() << "Error sending packet to decoder:";
        qDebug() << "  Error code:" << ret;
        qDebug() << "  PTS:" << (decoderPacket->pts == AV_NOPTS_VALUE ? "N/A" : QString::number(decoderPacket->pts));
        qDebug() << "  DTS:" << (decoderPacket->dts == AV_NOPTS_VALUE ? "N/A" : QString::number(decoderPacket->dts));
        qDebug() << "  Flags:" << decoderPacket->flags;
        qDebug() << "  Size:" << decoderPacket->size;

        return;
    }

    // 接收解码后的帧
    while (ret >= 0) {
        ret = avcodec_receive_frame(decoderContext, decoderFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            char errBufDecRecv[256];
            av_strerror(ret, errBufDecRecv, sizeof(errBufDecRecv));
            qWarning() << "解码过程中出错:" << errBufDecRecv;
            break;
        }

        // 将解码后的 YUV420P 转换为 RGB24
        QImage decodedImage(frameWidth, frameHeight, QImage::Format_RGB888);
        uint8_t *destData[1] = { decodedImage.bits() };
        int destLinesize[1] = { static_cast<int>(decodedImage.bytesPerLine()) };

        ret = sws_scale(
            swsDecoderCtx,
            decoderFrame->data,
            decoderFrame->linesize,
            0,
            frameHeight,
            destData,
            destLinesize
            );

        if (ret <= 0) {
            qWarning() << "解码后的颜色空间转换失败";
            av_frame_unref(decoderFrame);
            continue;
        }

        // 现在，decodedImage 中包含了解码后的图像，可以用于显示或其他处理
        // 例如，您可以将其发送到一个信号，以在界面上显示

        // 示例：打印一条调试信息
        emit decimage(decodedImage);
        qDebug() << "解码并转换了一帧图片";

        // 如果需要显示，可以在此处添加信号发射，例如:
       // emit decodedImageReady(decodedImage);

        // 释放解码后的帧
        av_frame_unref(decoderFrame);
    }

    // 释放数据包
    av_packet_unref(decoderPacket);
}

void ffmpeg::initffmpeg()
{
    qDebug()<<"sasasasasasa";
    //avcodec_register_all();
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug()<<"libx264 encoder not found";
    }
    if (!codec) {
        qCritical() << "H.264 编码器未找到";
        return ;
    }

    // 检查找到的是否是 libx264
    if (std::string(codec->name) != "libx264") {
        qWarning() << "找到的 H.264 编码器不是 libx264，而是:" << codec->name;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qDebug()<<"Could not allocate video codec context";
    }
    // 设置编码参数
    codecContext->bit_rate = 400000;
    codecContext->width = frameWidth;
    codecContext->height = frameHeight;
    codecContext->time_base = AVRational{1, 30}; // 30 fps
    codecContext->framerate = AVRational{30, 1};
    codecContext->gop_size = 10;
    codecContext->max_b_frames = 0;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    // 设置编码器选项（如 preset 和 crf）
    AVDictionary *codecOptions = nullptr;
    av_dict_set(&codecOptions, "preset", "veryfast", 0);
    av_dict_set(&codecOptions, "crf", "23", 0);
    // 打开编码器
    if (avcodec_open2(codecContext, codec, &codecOptions) < 0) {
        qDebug()<<"Could not open codec";
    }
    // 分配 AVFrame
    avFrame = av_frame_alloc();
    if (!avFrame) {
        qDebug()<<"Could not allocate video frame";
    }
    avFrame->format = codecContext->pix_fmt;
    avFrame->width  = codecContext->width;
    avFrame->height = codecContext->height;
    if (av_frame_get_buffer(avFrame, 32) < 0) {
        qFatal("Could not allocate the video frame data");
    }
    // 分配 AVPacket
    avPacket = av_packet_alloc();
    if (!avPacket) {
        qFatal("Could not allocate AVPacket");
    }
    // 初始化 SwsContext 用于颜色空间转换（从 RGB24 转为 YUV420P）
    swsCtx = sws_getContext(frameWidth, frameHeight, AV_PIX_FMT_RGB24,
                            frameWidth, frameHeight, AV_PIX_FMT_YUV420P,
                            SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsCtx) {
        qFatal("Could not initialize the conversion context");
    }
    decffmpeg();

}

void ffmpeg::processFrame(const QImage &image)
{
    if (image.isNull())
        return;
    // 确保图像格式为 RGB24
    QImage rgbImage = image.convertToFormat(QImage::Format_RGB888);
    const uint8_t *srcData[1] = { rgbImage.bits() };
    int srcLinesize[1] = { static_cast<int>(rgbImage.bytesPerLine()) };
    // 颜色空间转换
    sws_scale(swsCtx, srcData, srcLinesize, 0, frameHeight,
              avFrame->data, avFrame->linesize);
    // 设置 PTS
    avFrame->pts = frameIndex++;

    // 编码帧
    int ret = avcodec_send_frame(codecContext, avFrame);
    if (ret < 0) {
        char errBuf[256];
        av_strerror(ret, errBuf, sizeof(errBuf));
        qDebug() << "Error sending a frame for encoding:" << errBuf;
        return;
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(codecContext, avPacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            qWarning("Error during encoding");
            break;
        }
        // 发送编码后的数据通过 UDP
        const int maxPacketSize = 1200; // 避免 IP 分片
        int dataSize = avPacket->size;
        qDebug()<<"编码大小"<<dataSize;
        uint8_t *dataPtr = avPacket->data;
        AVPacket* packetCopy = av_packet_alloc();
        av_packet_ref(packetCopy, avPacket); // 深拷贝
        emit sendsosj(packetCopy);
        // while (dataSize > 0) {
        //     int currentSize = qMin(dataSize, maxPacketSize);
        //     QByteArray datagram(reinterpret_cast<char*>(dataPtr), currentSize);
        //     udpSocket->writeDatagram(datagram, udpHost, udpPort);
        //     dataPtr += currentSize;
        //     dataSize -= currentSize;
        // }
        // 传递给解码函数
        //decodePacket(avPacket);
        av_packet_unref(avPacket);
    }
}
