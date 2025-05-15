#ifndef CHATUSERLIST_HXX
#define CHATUSERLIST_HXX
#include<QListWidget>
#include<QWheelEvent>
#include<QEvent>
#include<QScrollBar>
#include<QDebug>

class ChatUserList:public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);
    bool _load_pending;
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_HXX
