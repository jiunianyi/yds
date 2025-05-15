#include "chatpage.hxx"
#include "global.h"
#include "qbuffer.h"
#include"filebubble.hxx"
#include "qthread.h"
#include "ui_chatpage.h"
#include<QStyleOption>
#include<QPainter>
#include"chatitembase.hxx"
#include"picturebubble.hxx"
#include"textbubble.hxx"
#include"messagetextedit.hxx"
#include "ui_selectbtn.h"
#include"usermgr.h"
#include"tcpmgr.hxx"
#include<QJsonDocument>
#include"selectbtn.hxx"
#include<QFileDialog>
#include<QMessageBox>
#include<QDesktopServices>
#include <QStandardPaths>
#include"video.hxx"

//打字小页面的配置
ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::ChatPage)
    , emojiPanel(new QWidget(this)),
    filew(new QWidget(this)),ffThread(new QThread(this))
{
    ui->setupUi(this);
    //设置按钮样式
    //ui->recvive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");
    emojisinit();
    wenjianinit();
    connect(ui->emo_lb, &ClickedLabel::clicked, this, &ChatPage::showEmojiPanel);
    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
    ui->shipin_lb->SetState("normal","hover","press","normal","hover","press");
    ui->yuyin_lb->SetState("normal","hover","press","normal","hover","press");
    connect(ui->yuyin_lb,&ClickedLabel::clicked,this,&ChatPage::sel_yunyin_lb);
    connect(ui->file_lb,&ClickedLabel::clicked,this,&ChatPage::sele_file);
    connect(ui->shipin_lb,&ClickedLabel::clicked,this,&ChatPage::sel_shipin_lb);
    //connect(pTextEdit,&pTextEdit::send,this,&ChatPage::on_send_btn_clicked);
    //connect(this,&ChatPage::sig_yuyin_req,udp,&udpmgr::send_yuyin_connect);
    ff = new ffmpeg();
    connect(ffThread,&QThread::started,ff,&ffmpeg::initffmpeg);
    connect(ffThread, &QThread::finished, ffThread, &QThread::deleteLater);
    ff->moveToThread(ffThread);
    ffThread->start();
}

ChatPage::~ChatPage()
{
    delete ui;
    ffThread->quit();
    ffThread->wait();
}

void ChatPage::painEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::emojisinit()
{
    emojiPanel->setWindowFlags(Qt::Popup); // 设置为弹出窗口
    QGridLayout *gridLayout = new QGridLayout(emojiPanel);
    // 表情图片路径列表
    QStringList emojiList = {
        ":/emojis/1.png",
        ":/emojis/2.png",
        ":/emojis/3.png",
        ":/emojis/4.png",
        ":/emojis/5.png",
        ":/emojis/7.png",
        ":/emojis/8.png",
        ":/emojis/9.png",
        ":/emojis/10.png",
        ":/emojis/11.png",
        ":/emojis/12.png"
    };
    int row = 0, col = 0;
    for(const QString &emojiPath : emojiList){
        QPushButton *btn = new QPushButton(emojiPanel);
        btn->setIcon(QIcon(emojiPath));
        btn->setIconSize(QSize(32, 32));
        btn->setFlat(true); // 去边框
        btn->setFixedSize(40, 40);
        gridLayout->addWidget(btn, row, col);

        // 设置按钮的自定义属性存储表情路径
        btn->setProperty("emojiPath", emojiPath);
        connect(btn, &QPushButton::clicked, this, &ChatPage::insertEmoji);

        col++;
        if(col >= 4){ // 每行4个表情
            col = 0;
            row++;
        }
    }
}

void ChatPage::wenjianinit()
{
    filew->setWindowFlags(Qt::Popup);
    QGridLayout *gridLayout = new QGridLayout(filew);
    QStringList filelist = {
        ":/res/wj.jpg",
        ":/res/wjj.jpg",
        ":/res/tup.jpg"
    };
    int row = 0, col = 0;
    for(const QString &filepath : filelist){
        QPushButton *btn = new QPushButton(filew);
        btn->setIcon(QIcon(filepath));
        btn->setIconSize(QSize(50, 50));
        btn->setFlat(true); // 去边框
        btn->setFixedSize(60, 60);
        gridLayout->addWidget(btn, row, col);

        // 设置按钮的自定义属性存储表情路径
        if(filepath== ":/res/wj.jpg")
        {
            btn->setProperty("filepath", "文件");
        }else if(filepath== ":/res/wjj.jpg")
        {
            btn->setProperty("filepath", "文件夹");
        }else{
            btn->setProperty("filepath", "图片");
        }

        connect(btn, &QPushButton::clicked, this, &ChatPage::send_file);

        col++;
        if(col >= 3){ // 每行4个表情
            col = 0;
            row++;
        }
    }
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    //设置ui界面
    ui->title_lb->setText(_user_info->_name);
    ui->chat_data_list->removAlItem();
    for(auto &msg:user_info->_chat_msgs)
    {
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    qDebug()<<"设置消息";
    auto self_info = UserMgr::getinstance()->GetUserInfo();
    ChatRole role;
    //todo
    if (msg->_from_uid == self_info->_uid) {
        if(msg->_msg_id=="file")
        {

        }
        if(msg->_msg_id=="wj")
        {
            role = ChatRole::self;
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
            // 桌面环境允许用户选择保存位置
            QString defaultPath = getDefaultDownloadPath(msg->_name);
         
            if (!defaultPath.isEmpty()) {
                QFile file(defaultPath);
                if (file.open(QIODevice::WriteOnly)) {
                    QByteArray data = QByteArray::fromBase64(msg->_msg_content.toUtf8());
                    qint64 bytesWritten = file.write(data);

                    if (bytesWritten == -1) {
                        qWarning() << "写入文件失败：" << file.errorString();
                    }
                    else {
                        qDebug() << "成功写入" << bytesWritten << "字节到" << defaultPath;
                    }
                }
                else {
                    qWarning() << "无法打开文件进行写入：" << file.errorString();
                }
                //获取文件图标
                QFileIconProvider iconProvider;
                QFileInfo fileInfo(defaultPath);
                QIcon icon = iconProvider.icon(fileInfo);
                QPixmap pixmap = icon.pixmap(24, 24);
                // 在聊天记录显示图片
                ChatItemBase *pChatItem = new ChatItemBase(role);
                pChatItem->setUserName(self_info->_name);
                pChatItem->setUserIcon(QPixmap(self_info->_icon));
                QWidget *pBubble = nullptr;
                //点击查看内容
                QPushButton *fileButton = new QPushButton(pixmap,QFileInfo(defaultPath).fileName(), this);
                pBubble = new fileBubble(pixmap, fileButton,role);

                pChatItem->setWidget(pBubble);
                ui->chat_data_list->appendChatItem(pChatItem);

                // 连接按钮的点击信号到查看文件的槽
                connect(fileButton, &QPushButton::clicked, this, [defaultPath]() {
                    QFileInfo fileInfo(defaultPath);
                    QString suffix = fileInfo.suffix().toLower();

                    // 判断文件类型是否为文本文件
                    if (suffix == "txt" || suffix == "cpp" || suffix == "h" || suffix == "py" || suffix == "md") {
                        QFile file(defaultPath);
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            QString content = in.readAll();
                            file.close();

                            // 创建一个自定义的对话框来显示文本内容
                            QDialog *dialog = new QDialog();
                            dialog->setWindowTitle("查看文件 - " + fileInfo.fileName());
                            QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);

                            QLabel *label = new QLabel("<b>文件内容:</b>", dialog);
                            dialogLayout->addWidget(label);

                            QTextEdit *textEdit = new QTextEdit(dialog);
                            textEdit->setReadOnly(true);
                            textEdit->setText(content);
                            textEdit->setMinimumSize(400, 300);
                            dialogLayout->addWidget(textEdit);

                            QPushButton *closeButton = new QPushButton("关闭", dialog);
                            dialogLayout->addWidget(closeButton);
                            connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

                            dialog->setLayout(dialogLayout);
                            dialog->exec();
                        } else {
                            QMessageBox::warning(nullptr, "错误", "无法打开文件: " + defaultPath);
                        }
                    }
                    else {
                        // 对于非文本文件，使用默认应用程序打开
                        QDesktopServices::openUrl(QUrl::fromLocalFile(defaultPath));
                    }
                });

            }
            return;
        }
        if (msg->_msg_id == "tu")
        {
            qDebug() << "weqewewewewewewe";
            //todo解码图片
            role = ChatRole::self;
            ChatItemBase* pChatItem = new ChatItemBase(role);
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
            pChatItem->setUserName(self_info->_name);
            pChatItem->setUserIcon(QPixmap(self_info->_icon));
            // QByteArray byteArray = QByteArray::fromBase64(msg->_msg_content.toUtf8());

            QPixmap pixmap(msg->_msg_content);
            QWidget* pBubble = nullptr;
            pBubble = new PictureBubble(pixmap,role);
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
            if (pixmap.isNull()) {
                qDebug() << "Failed to load image from Base64 data.";
            } else {
                qDebug() << "Image loaded successfully from Base64 data.";
                return;
                // 这里可以使用 pixmap，例如显示在界面上
            }
            return;
        }
        else if(msg->_msg_id == "wtu")
        {
            role = ChatRole::self;
            ChatItemBase* pChatItem = new ChatItemBase(role);
            pChatItem->setUserName(self_info->_name);
            pChatItem->setUserIcon(QPixmap(self_info->_icon));
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
             QByteArray byteArray = QByteArray::fromBase64(msg->_msg_content.toUtf8());
             QPixmap pixmap;
             QWidget* pBubble = nullptr;
             if (pixmap.loadFromData(byteArray)) {
                 pBubble = new PictureBubble(pixmap, role);
                 // 显示 pBubble ...
             }
             else {
                 qDebug() << "无法从数据加载 QPixmap";
             }
             pChatItem->setWidget(pBubble);
             ui->chat_data_list->appendChatItem(pChatItem);
             return;
        }
        role = ChatRole::self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->setUserName(self_info->_name);
        pChatItem->setUserIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
    else {
        if(msg->_msg_id=="file")
        {

        }
        if(msg->_msg_id=="wj")
        {
            role = ChatRole::other;
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
            // 桌面环境允许用户选择保存位置
            QString defaultPath = getDefaultDownloadPath(msg->_name);

            if (!defaultPath.isEmpty()) {
                QFile file(defaultPath);
                if (file.open(QIODevice::WriteOnly)) {
                    QByteArray data = QByteArray::fromBase64(msg->_msg_content.toUtf8());
                    qint64 bytesWritten = file.write(data);

                    if (bytesWritten == -1) {
                        qWarning() << "写入文件失败：" << file.errorString();
                    }
                    else {
                        qDebug() << "成功写入" << bytesWritten << "字节到" << defaultPath;
                    }
                }
                else {
                    qWarning() << "无法打开文件进行写入：" << file.errorString();
                }
                //获取文件图标
                QFileIconProvider iconProvider;
                QFileInfo fileInfo(defaultPath);
                QIcon icon = iconProvider.icon(fileInfo);
                QPixmap pixmap = icon.pixmap(24, 24);
                // 在聊天记录显示图片
                ChatItemBase *pChatItem = new ChatItemBase(role);
                pChatItem->setUserName(friend_info->_name);
                pChatItem->setUserIcon(QPixmap(friend_info->_icon));
                QWidget *pBubble = nullptr;
                //点击查看内容
                QPushButton *fileButton = new QPushButton(pixmap,QFileInfo(defaultPath).fileName(), this);
                pBubble = new fileBubble(pixmap, fileButton,role);

                pChatItem->setWidget(pBubble);
                ui->chat_data_list->appendChatItem(pChatItem);

                // 连接按钮的点击信号到查看文件的槽
                connect(fileButton, &QPushButton::clicked, this, [defaultPath]() {
                    QFileInfo fileInfo(defaultPath);
                    QString suffix = fileInfo.suffix().toLower();

                    // 判断文件类型是否为文本文件
                    if (suffix == "txt" || suffix == "cpp" || suffix == "h" || suffix == "py" || suffix == "md") {
                        QFile file(defaultPath);
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            QString content = in.readAll();
                            file.close();

                            // 创建一个自定义的对话框来显示文本内容
                            QDialog *dialog = new QDialog();
                            dialog->setWindowTitle("查看文件 - " + fileInfo.fileName());
                            QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);

                            QLabel *label = new QLabel("<b>文件内容:</b>", dialog);
                            dialogLayout->addWidget(label);

                            QTextEdit *textEdit = new QTextEdit(dialog);
                            textEdit->setReadOnly(true);
                            textEdit->setText(content);
                            textEdit->setMinimumSize(400, 300);
                            dialogLayout->addWidget(textEdit);

                            QPushButton *closeButton = new QPushButton("关闭", dialog);
                            dialogLayout->addWidget(closeButton);
                            connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

                            dialog->setLayout(dialogLayout);
                            dialog->exec();
                        } else {
                            QMessageBox::warning(nullptr, "错误", "无法打开文件: " + defaultPath);
                        }
                    }
                    else {
                        // 对于非文本文件，使用默认应用程序打开
                        QDesktopServices::openUrl(QUrl::fromLocalFile(defaultPath));
                    }
                });

            }
            return;
        }
        if (msg->_msg_id == "tu")
        {
            qDebug() << "weqewewewewewewe";
            //todo解码图片
            role = ChatRole::other;
            ChatItemBase* pChatItem = new ChatItemBase(role);
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
            pChatItem->setUserName(friend_info->_name);
            pChatItem->setUserIcon(QPixmap(friend_info->_icon));
            // QByteArray byteArray = QByteArray::fromBase64(msg->_msg_content.toUtf8());

            QPixmap pixmap(msg->_msg_content);
            QWidget* pBubble = nullptr;
            pBubble = new PictureBubble(pixmap,role);
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
            return;
        }
        if(msg->_msg_id == "wtu")
        {
            role = ChatRole::other;
            ChatItemBase* pChatItem = new ChatItemBase(role);
            auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
            if (friend_info == nullptr) {
                return;
            }
            pChatItem->setUserName(friend_info->_name);
            pChatItem->setUserIcon(QPixmap(friend_info->_icon));
            QByteArray byteArray = QByteArray::fromBase64(msg->_msg_content.toUtf8());
            QPixmap pixmap;
            QWidget* pBubble = nullptr;
            if (pixmap.loadFromData(byteArray)) {
             
                pBubble = new PictureBubble(pixmap, role);
                // 显示 pBubble ...
       
            }
            else {
                qDebug() << "无法从数据加载 QPixmap";
            }
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
            return;
        }
        role = ChatRole::other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->setUserName(friend_info->_name);
        pChatItem->setUserIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}

void ChatPage::attachFile()
{
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    ChatRole role = ChatRole::self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件");
   if (!filePath.isEmpty()) {
        QFile file(filePath);
       if (file.open(QIODevice::ReadOnly)) {
           QByteArray fileData = file.readAll();
           file.close();
           //获取文件图标
           QFileIconProvider iconProvider;
           QFileInfo fileInfo(filePath);
           QIcon icon = iconProvider.icon(fileInfo);
           QPixmap pixmap = icon.pixmap(24, 24);
           // 在聊天记录显示图片
           ChatItemBase *pChatItem = new ChatItemBase(role);
           pChatItem->setUserName(userName);
           pChatItem->setUserIcon(QPixmap(userIcon));
           QWidget *pBubble = nullptr;
           //点击查看内容
           QPushButton *fileButton = new QPushButton(pixmap,QFileInfo(filePath).fileName(), this);
           pBubble = new fileBubble(pixmap, fileButton,role);

           pChatItem->setWidget(pBubble);
           ui->chat_data_list->appendChatItem(pChatItem);

           // 连接按钮的点击信号到查看文件的槽
           connect(fileButton, &QPushButton::clicked, this, [filePath]() {
               QFileInfo fileInfo(filePath);
               QString suffix = fileInfo.suffix().toLower();

               // 判断文件类型是否为文本文件
               if (suffix == "txt" || suffix == "cpp" || suffix == "h" || suffix == "py" || suffix == "md") {
                   QFile file(filePath);
                   if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                       QTextStream in(&file);
                       QString content = in.readAll();
                       file.close();

                       // 创建一个自定义的对话框来显示文本内容
                       QDialog *dialog = new QDialog();
                       dialog->setWindowTitle("查看文件 - " + fileInfo.fileName());
                       QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);

                       QLabel *label = new QLabel("<b>文件内容:</b>", dialog);
                       dialogLayout->addWidget(label);

                       QTextEdit *textEdit = new QTextEdit(dialog);
                       textEdit->setReadOnly(true);
                       textEdit->setText(content);
                       textEdit->setMinimumSize(400, 300);
                       dialogLayout->addWidget(textEdit);

                       QPushButton *closeButton = new QPushButton("关闭", dialog);
                       dialogLayout->addWidget(closeButton);
                       connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

                       dialog->setLayout(dialogLayout);
                       dialog->exec();
                   } else {
                       QMessageBox::warning(nullptr, "错误", "无法打开文件: " + filePath);
                   }
               }
               else {
                   // 对于非文本文件，使用默认应用程序打开
                   QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
               }
           });
           // 创建图片消息的数据结构并发送信号
           auto img_msg = std::make_shared<TextChatData>("wj",fileData.toBase64().constData(), user_info->_uid, _user_info->_uid);
           emit sig_append_send_chat_msg(img_msg);

           //
           QString base64Image = fileData.toBase64();
           QJsonObject obj;
         k myStruct(filePath, fileData);
           obj["content"] = QJsonObject{
             {"filename", myStruct.filename},
             {"text", myStruct.text}
         };
           obj["msgid"] = "wj";
           obj["fromuid"] = user_info->_uid;
           obj["touid"] = _user_info->_uid;
           QJsonArray textArray;
           textArray.append(obj);
           QJsonObject textObj;
           textObj["text_array"] = textArray;
           textObj["fromuid"] = user_info->_uid;
           textObj["touid"] = _user_info->_uid;
             QJsonDocument doc(textObj);
           QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
           //发送并清空之前累计的文本列表
           textArray = QJsonArray();
           textObj = QJsonObject();
           //发送tcp请求给chat server
           emit TcpMgr::getinstance()->sig_send_data(RedId::ID_TEXT_CHAT_MSG_REQ, jsonData);
       }

    }

}

void ChatPage::attachImage()
{
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    ChatRole role = ChatRole::self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray imageData = file.readAll();
            file.close();
            // 在聊天记录显示图片
            ChatItemBase *pChatItem = new ChatItemBase(role);
            pChatItem->setUserName(userName);
            pChatItem->setUserIcon(QPixmap(userIcon));
            QWidget *pBubble = nullptr;
            QPixmap pixmap;
            pixmap.loadFromData(imageData);
             pBubble = new PictureBubble(pixmap, role);
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
            // 创建图片消息的数据结构并发送信号
            auto img_msg = std::make_shared<TextChatData>("wtu",imageData.toBase64(), user_info->_uid, _user_info->_uid);
            emit sig_append_send_chat_msg(img_msg);
//
            QImage image(filePath);
            QByteArray im;
            QBuffer buffer(&im);
            buffer.open(QIODevice::WriteOnly);
            if (!image.save(&buffer, "PNG")) {  // 注意这里要严格匹配格式字符串
                qDebug() << "Failed to save image to PNG format";
                return;
            }
            QString base64Image = im.toBase64();
            QJsonObject obj;
            obj["content"] = base64Image;
            obj["fromuid"] = user_info->_uid;
            obj["touid"] = _user_info->_uid;
            obj["msgid"] = "wtu";
            QJsonArray textArray;
            textArray.append(obj);
            QJsonObject textObj;
            textObj["text_array"] = textArray;
            textObj["fromuid"] = user_info->_uid;
            textObj["touid"] = _user_info->_uid;
            QJsonDocument doc(textObj);
            QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
            //发送并清空之前累计的文本列表
            textArray = QJsonArray();
            textObj = QJsonObject();
            //发送tcp请求给chat server
            emit TcpMgr::getinstance()->sig_send_data(RedId::ID_TEXT_CHAT_MSG_REQ, jsonData);

        }

    }
}

void ChatPage::attachFolder()
{

}



void ChatPage::on_send_btn_clicked()
{
    if (_user_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }

    auto user_info = UserMgr::getinstance()->GetUserInfo();
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;

    const QVector<Msginfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        qDebug()<<msgList.size();
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlage;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if(type == "text")
        {
            qDebug()<<"文字";
            //生成唯一id
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            if(txt_size + msgList[i].content.length()> 1024){
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpMgr::getinstance()->sig_send_data(RedId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
                                                          user_info->_uid, _user_info->_uid);
            emit sig_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            qDebug()<<"图片";
            // pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
            // 生成唯一 ID
            QUuid uuid = QUuid::createUuid();
           // QString uuidString = uuid.toString();暂时不用
            QString uuidString = "tu";


            QPixmap pixmap(msgList[i].content);
            pBubble = new PictureBubble(pixmap, role);

            QJsonObject obj;
            obj["type"] = "image";
            obj["content"] = msgList[i].content;
            obj["msgid"] = uuidString;
            obj["fromuid"] = user_info->_uid;
            obj["touid"] = _user_info->_uid;

            // 将图片消息添加到 textArray 中（虽然名称是 textArray，但可以存放多种类型消息）
            textArray.append(obj);
            txt_size += msgList[i].content.length();

            // 检查累计的大小是否超过限制
            if(txt_size > 1024){
                // 构建最终发送的 JSON 对象
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;

                // 转换为 JSON 文档和字节数组
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

                // 发送 TCP 请求给聊天服务器
                emit TcpMgr::getinstance()->sig_send_data(RedId::ID_TEXT_CHAT_MSG_REQ, jsonData);

                // 清空累计的数据
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
            }

            // 创建图片消息的数据结构并发送信号
           auto img_msg = std::make_shared<TextChatData>(uuidString, msgList[i].content, user_info->_uid, _user_info->_uid);
           emit sig_append_send_chat_msg(img_msg);
        }
        else if(type == "file")
        {

            qDebug()<<"文件";

        }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }

    }

    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->_uid;
    textObj["touid"] = _user_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpMgr::getinstance()->sig_send_data(RedId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

void ChatPage::sel_yunyin_lb()
{
   selectbtn &btn = selectbtn::getInstance();
    btn.ui->label->setText("是否打给对方");
    btn.QDialog::show();
   btn.touid = _user_info->_uid;
    qDebug()<<UserMgr::getinstance()->GetUserInfo()->_uid<<"wwwwwwwww";
    btn.fromuid = UserMgr::getinstance()->GetUserInfo()->_uid;
}

void ChatPage::sel_shipin_lb()
{
    video& vi = video::getInstance();
    vi.touid = _user_info->_uid;
    qDebug() << vi.touid << "ssssssssss";
    video::getInstance().show();
    video::getInstance().initvideo();

}
void ChatPage::send_file()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(btn)
    {
        QString context = btn->property("filepath").toString();
        if (!context.isEmpty()) {
            if (context == "文件") {
                attachFile();
            } else if (context == "图片") {
                attachImage();
            } else if (context == "文件夹") {
                attachFolder();
            }
        }
    }
}
void ChatPage::insertEmoji()
{
    // 获取发送信号的按钮
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(btn){
        // 从按钮的属性中获取表情路径
        QString emojiPath = btn->property("emojiPath").toString();
        if (emojiPath.isEmpty()) {
            qDebug() << "表情路径为空!";
            return;
        }

        // 加载表情图片
        QPixmap pixmap(emojiPath);
        if(pixmap.isNull()){
            qDebug() << "加载表情图片失败:" << emojiPath;
            return;
        }

        // 准备插入图片
        QTextCursor cursor = ui->chatEdit->textCursor();
        QTextImageFormat imageFormat;
        imageFormat.setName(emojiPath);

        // 可选：设置图片显示大小
        imageFormat.setWidth(32);  // 根据需要调整
        imageFormat.setHeight(32); // 根据需要调整

        // 插入图片
        cursor.insertImage(imageFormat);

        // 更新光标位置
        ui->chatEdit->setTextCursor(cursor);
    }
    //  emojiPanel->hide(); // 插入后隐藏表情包选择框
}

void ChatPage::showEmojiPanel()
{
    // 将表情包选择框显示在表情按钮下方
    QPoint pos = ui->emo_lb->mapToGlobal(QPoint(0, ui->emo_lb->height() - 200)); //偏移
    emojiPanel->move(pos);
    emojiPanel->show();
}

void ChatPage::sele_file()
{
    QPoint pos = ui->file_lb->mapToGlobal(QPoint(ui->file_lb->width()-80, ui->file_lb->height() - 120)); //偏移
    filew->move(pos);
    filew->show();
    // QString filepath = QFileDialog::getOpenFileName(
    //     this,
    //     tr("选择文件"),
    //     QDir::homePath(),
    //     tr("所有文件(*.*)")
    //     );
    // if(!filepath.isEmpty())
    // {
    //     qDebug()<<"成功打开";
    //     ui->chatEdit->insertTextFile(filepath);
    // }
}

QString ChatPage::getDefaultDownloadPath(const QString &fileName)
{
    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadPath.isEmpty()) {
        // 如果 Download 目录不可用，使用文档目录作为备选
        downloadPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }

    QDir dir(downloadPath);
    if (!dir.exists()) {
        dir.mkpath(downloadPath); // 创建目录（如果不存在）
    }

    return dir.filePath(fileName);
}






