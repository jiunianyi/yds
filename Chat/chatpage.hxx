#ifndef CHATPAGE_HXX
#define CHATPAGE_HXX

#include <QWidget>
#include "qpushbutton.h"
#include"userdata.hxx"
#include"ffmpeg.hxx"


namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AppendChatMsg(std::shared_ptr<TextChatData>msg);
    void attachFile();
    void attachImage();
    void attachFolder();
    QString getDefaultDownloadPath(const QString& fileName);
protected:
    void painEvent(QPaintEvent *event);
    void emojisinit();
    void wenjianinit();
private slots:
    void on_send_btn_clicked();
    void sel_yunyin_lb();
    void sel_shipin_lb();
    void insertEmoji();
    void showEmojiPanel();
    void sele_file();
    void send_file();

private:
    Ui::ChatPage *ui;
    QWidget *emojiPanel;
    QWidget *filew;
    QMap<QPushButton,QString> fi;

    ffmpeg *ff;
     QThread *ffThread;

public:
    std::shared_ptr<UserInfo> _user_info;
signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
};

#endif // CHATPAGE_HXX
