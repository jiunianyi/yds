#include "MessageTextEdit.hxx"
#include <QDebug>
#include <QMessageBox>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextFrame>

MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{

    //this->setStyleSheet("border: none;");
    this->setMaximumHeight(60);

    //    connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));

}

MessageTextEdit::~MessageTextEdit()
{

}

QVector<Msginfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();
    qDebug() << "开始提取消息列表";

    QTextDocument *doc = this->document();
    QTextBlock block = doc->begin();

    while (block.isValid()) {
        QTextLayout *layout = block.layout();
        if (!layout) {
            block = block.next();
            continue;
        }

        // 使用迭代器遍历 QTextFragment
        QTextBlock::iterator it;
        for (it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (fragment.isValid()) {
                if (fragment.charFormat().isImageFormat()) {
                    QTextImageFormat imgFormat = fragment.charFormat().toImageFormat();
                    QString imgName = imgFormat.name();
                    qDebug() << "找到图片或文件:" << imgName;

                    // 查找是否已在mMsgList中存在该项
                    bool found = false;
                    for (const Msginfo &msg : mMsgList) {
                        if (msg.content == imgName) {
                            // 找到匹配项，使用原始类型和像素图
                            insertMsgList(mGetMsgList, msg.msgFlage, imgName, msg.pixmap);
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        // 如果在mMsgList中没有找到，则判断是图片还是文件
                        QFileInfo fileInfo(imgName);
                        if (fileInfo.exists() && !isImageFile(imgName)) {
                            // 这是一个文件
                            QPixmap fileIcon = getFileIconPixmap(imgName);
                            insertMsgList(mGetMsgList, "file", imgName, fileIcon);
                        } else {
                            // 这是一个图片
                            insertMsgList(mGetMsgList, "image", imgName, QPixmap(imgName));
                        }
                    }
                } else {
                    QString text = fragment.text();
                    if (!text.isEmpty()) {
                        qDebug() << "找到文本:" << text;
                        insertMsgList(mGetMsgList, "text", text, QPixmap());
                    }
                }
            }
        }

        block = block.next();
    }

    mMsgList.clear();
    this->clear();
    return mGetMsgList;
}
// 辅助函数：判断是否为图片文件
bool MessageTextEdit::isImageFile(const QString &filePath)
{
    static QStringList imageExtensions = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".webp"
    };

    QString lowerPath = filePath.toLower();
    for (const QString &ext : imageExtensions) {
        if (lowerPath.endsWith(ext)) {
            return true;
        }
    }
    return false;
}
void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this)
        event->ignore();
    else
        event->accept();
}

void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) && !(e->modifiers() & Qt::ShiftModifier))
    {
        qDebug()<<"点击回车";
        emit send();
        return; // 确保在发送后返回，不再执行 QTextEdit 的默认处理
    }
    // 调用基类的实现，处理其他按键事件（如 Shift + Enter 换行等）
    QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty())
        return;

    foreach (QString url, urls){
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

void MessageTextEdit::insertImages(const QString &url)
{

    QImage image(url);
    //按比例缩放图片
    if(image.width()>120||image.height()>80)
    {
        if(image.width()>image.height())
        {
            image =  image.scaledToWidth(120,Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80,Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    // QTextDocument *document = this->document();
    // document->addResource(QTextDocument::ImageResource, QUrl(url), QVariant(image));
    cursor.insertImage(image,url);

    insertMsgList(mMsgList,"image",url,QPixmap::fromImage(image));
}

void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this,"提示","只允许拖拽单个文件!");
        return;
    }

    if(fileInfo.size()>100*1024*1024)
    {
        QMessageBox::information(this,"提示","发送的文件大小不能大于100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(),url);
    insertMsgList(mMsgList,"file",url,pix);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    if(urls.isEmpty())
        return;

    foreach (QString url, urls)
    {
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

bool MessageTextEdit::isImage(QString url)
{
    qDebug()<<"tupian";
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

void MessageTextEdit::insertMsgList(QVector<Msginfo> &list, QString flag, QString text, QPixmap pix)
{
    Msginfo msg;
    msg.msgFlage=flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

QStringList MessageTextEdit::getUrl(QString text)
{
    QStringList urls;
    if(text.isEmpty()) return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty()){
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << endl;
}
