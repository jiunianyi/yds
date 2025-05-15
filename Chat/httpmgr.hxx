#ifndef HTTPMGR_HXX
#define HTTPMGR_HXX
#include<QUrl>
#include<QString>
#include"singleton.h"
#include<QObject>
#include<QNetworkAccessManager>
#include<QJsonObject>
#include<QJsonDocument>

class HttpMgr:public QObject,public singleton<HttpMgr>,public std::enable_shared_from_this<HttpMgr>
{
    //friend class registerdialog;
    Q_OBJECT
public:
    ~HttpMgr();
     void posthttpreq(QUrl url,QJsonObject json,RedId req_id,Modules mod);//发送
private:
    friend class singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager _manager;
    //void posthttpreq(QUrl url,QJsonObject json,RedId req_id,Modules mod);//发送
private slots:
    void slot_http_finish(RedId req_id, QString res,ErrorCodes err,Modules mod);
signals:
    void sig_http_finish(RedId req_id, QString res,ErrorCodes err,Modules mod);//发送信号通知其他模块
    void sig_reg_mod_finish(RedId id,QString res,ErrorCodes err);//连接成功 界面响应
    void sig_reset_mod_finish(RedId id,QString res,ErrorCodes err);
    void sig_login_mod_finish(RedId id,QString res,ErrorCodes err);
};

#endif // HTTPMGR_HXX
