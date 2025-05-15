#ifndef TCPMGR_HXX
#define TCPMGR_HXX
#include<QTcpSocket>
#include"singleton.h"
#include"global.h"
#include<functional>
#include<QObject>
#include"userdata.hxx"
#include<memory>
class TcpMgr:public QObject,public singleton<TcpMgr>
    ,std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    int t=0;
    ~TcpMgr();
    TcpMgr();
    void inithander();
    void handleMsg(RedId id,int len,QByteArray data);
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint32 _message_len;
    QMap<RedId,std::function<void(RedId id,int len,QByteArray data)>>_handlers;
public slots:
    void slot_tcp_connect(ServerInfo si);
    void slot_send_data(RedId reqId, QByteArray data);
signals:
    void sig_con_success(bool bsuccess);
    void sig_con_failed(int);
    void sig_send_data(RedId reqId, QByteArray data);
    void sig_switch_chatdlg();
    void sig_login_failed(int);
    void sig_user_search(std::shared_ptr<SearchInfo>);//searchuser
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_friend_apply(std::shared_ptr<AddlyFriendApply>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg>msg);
};

#endif // TCPMGR_HXX
