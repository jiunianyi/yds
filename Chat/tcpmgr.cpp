#include "tcpmgr.hxx"
#include"QAbstractSocket"
#include"qjsonobject.h"
#include<QJsonDocument>
#include"usermgr.h"

TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr():_host(""),_port(0),_message_id(0),_message_len(0),_b_recv_pending(false)
{

    QObject::connect(&_socket,&QTcpSocket::connected,[&]()
    {
        qDebug()<<"45";
        //连接建立后发送消息
        qDebug()<<"connect to server!";
        emit sig_con_success(true);
    });
    QObject::connect(&_socket,&QTcpSocket::readyRead,[&](){
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer,QIODevice::ReadOnly);
        //设置版本
        stream.setVersion(QDataStream::Qt_6_0);
        for(;;){
        if(!_b_recv_pending)
        {
            // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
            if(_buffer.size()<static_cast<int>(sizeof(quint16) +sizeof(quint32)))
            {
                //数据不够继续读取
                return;
            }
            // 预读取消息ID和消息长度，但不从缓冲区中移除
            stream >> _message_id >> _message_len;
            //将buffer的头部去掉
            _buffer = _buffer.mid(6);
            //输出头部id和数据大小
            qDebug()<<"Message id: "<<_message_id<<"Message len: "<<_message_len;
        }

        //buffer剩余长读是否满足消息体长度，不满足则退出继续等待接受
        if(_buffer.size()<_message_len)
        {
            _b_recv_pending=true;
            return;
        }
        _b_recv_pending=false;
        //都数据信息
        QByteArray mesasgeBody = _buffer.mid(0,_message_len);
        //清楚数据
        _buffer= _buffer.mid(_message_len);
        ++t;
        if(t==1)qDebug()<<"接受登录请求回报";
        if(t==2)qDebug()<<"接受到搜索请求回报";
        handleMsg(RedId(_message_id),_message_len,mesasgeBody);
        }
    });
    //5.15 之后版本
    //连接错误
   QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
         Q_UNUSED(socketError)
        qDebug()<<"sss";
       qDebug() << "Error:" << _socket.errorString();
   });
           //处理断开连接
     QObject::connect(&_socket,&QTcpSocket::disconnected,[&](){
       qDebug()<<"Disconnected from server";

              });
    //连接发送信号用来发送数据
     QObject::connect(this,&TcpMgr::sig_send_data,this,&TcpMgr::slot_send_data);
     //注册信息
     inithander();
}

void TcpMgr::inithander()
{
    qDebug()<<"oooooo";
    _handlers.insert(ID_CHAT_LOGIN_RSP,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"登陆成功后返回的结果如下：";
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
            {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(jsonobj.contains("error"))
            {
            //int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"login is failed,err is json parse err";
            //emit sig_log
            //return;
        }
        qDebug()<<"666asdfghyjuio";
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
            {
            qDebug()<<"login is err"<<err;
            emit sig_login_failed(err);
            return;
        }
        //UserMgr::getinstance()->SetUid(jsonobj["uid"].toInt());
        //UserMgr::getinstance()->SetName(jsonobj["name"].toString());
        //UserMgr::getinstance()->SetToken(jsonobj["token"].toString());
        auto uid = jsonobj["uid"].toInt();
        auto name = jsonobj["name"].toString();
        auto nick = jsonobj["nick"].toString();
        auto icon = jsonobj["icon"].toString();
        auto sex = jsonobj["sex"].toInt();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

        UserMgr::getinstance()->SetUserInfo(user_info);
        UserMgr::getinstance()->SetToken(jsonobj["token"].toString());
        if(jsonobj.contains("apply_list")){
            qDebug()<<"服务器传送过来apply_lis";
            UserMgr::getinstance()->AppendApplyList(jsonobj["apply_list"].toArray());
        }


        if (jsonobj.contains("friend_list")) {
            UserMgr::getinstance()->AppendFriendlist(jsonobj["friend_list"].toArray());
        }
        emit sig_switch_chatdlg();
    });
    _handlers.insert(ID_SESRCH_USER_RSP,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        qDebug()<<"收到收缩请求回报";
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
        {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(!jsonobj.contains("error"))
        {
            int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"ID_SESRCH_USER_RSP is failed,err is json parse err";

            //return;
        }
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"ID_SESRCH_USER_RSP is err"<<err;
            emit sig_user_search(nullptr);
            return;
        }
        qDebug()<<jsonobj["uid"].toInt();
        auto search_info = std::make_shared<SearchInfo>(jsonobj["uid"].toInt(),jsonobj["name"].toString(),
            jsonobj["nick"].toString(),jsonobj["desc"].toString()
                                ,jsonobj["sex"].toInt(),jsonobj["icon"].toString());
        emit sig_user_search(search_info);
    });
    _handlers.insert(ID_ADD_FRIEND_RSP,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        qDebug()<<"收到添加好友回报";
       /* QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
        {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(!jsonobj.contains("error"))
        {
            int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"ID_ADD_FRIEND_RSP is failed,err is json parse err"<<err;

            //return;
        }
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"ID_ADD_FRIEND_RSP is err"<<err;

            return;
        }*/
        qDebug()<<"添加好友成功";
    });

    _handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        qDebug()<<"通知用户添加好友申请回报";
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
        {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(!jsonobj.contains("error"))
        {
            int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ is failed,err is json parse err"<<err;

        }
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ is err"<<err;

            return;
        }
        int from_uid=jsonobj["applyuid"].toInt();
        QString name = jsonobj["name"].toString();
        QString desc = jsonobj["desc"].toString();
        QString icon = jsonobj["icon"].toString();
        QString nick =jsonobj["nick"].toString();
        int sex = jsonobj["sex"].toInt();
        auto apply_info = std::make_shared<AddlyFriendApply>(from_uid,name,desc,icon,nick,sex);
        emit sig_friend_apply(apply_info);

        qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ friend success";
    });
    _handlers.insert(ID_NOTIF_AUTH_FRIEND_REQ,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        qDebug()<<"通知用户认证好友申请";
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
        {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(!jsonobj.contains("error"))
        {
            int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ is failed,err is json parse err"<<err;

        }
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ is err"<<err;

            return;
        }
        int from_uid=jsonobj["applyuid"].toInt();
        QString name = jsonobj["name"].toString();
        QString icon = jsonobj["icon"].toString();
        QString nick =jsonobj["nick"].toString();
        int sex = jsonobj["sex"].toInt();
        auto apply_info = std::make_shared<AuthInfo>(from_uid,name,nick,icon,sex);
        emit sig_add_auth_friend(apply_info);

    });
    _handlers.insert(ID_AUTH_FRIEND_RSP,[this](RedId id,int len,QByteArray data){
        Q_UNUSED(len);//防警告
        qDebug()<<"handle id is:"<<id<<"handle data is:"<<data;
        //将QByteArray转换成ajsondocument
        qDebug()<<"通知用户认证好友回报";
        QJsonDocument jsondoc = QJsonDocument::fromJson(data);
        //检查是否转换成功
        if(jsondoc.isNull())
        {
            qDebug()<<"jsondocyment is failed";
            return;
        }
        QJsonObject jsonobj = jsondoc.object();
        if(!jsonobj.contains("error"))
        {
            int err  =ErrorCodes::ERR_JSON;
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ is failed,err is json parse err"<<err;

        }
        int err = jsonobj["error"].toInt();
        qDebug()<<"err:"<<err;
        if(err!=ErrorCodes::SUCCESS)
        {
            qDebug()<<"auth friend false err is"<<err;

            return;
        }
        auto name = jsonobj["name"].toString();
        auto nick = jsonobj["nick"].toString();
        auto icon = jsonobj["icon"].toString();
        auto sex = jsonobj["sex"].toInt();
        auto uid = jsonobj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid,name,nick,icon ,sex);
        emit sig_auth_rsp(rsp);
        qDebug()<<"Auth Frined success";
    });
    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](RedId id, int len, QByteArray data) {
        Q_UNUSED(len);
      //  qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });
    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](RedId id, int len, QByteArray data) {
        Q_UNUSED(len);
       // qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });

}

void TcpMgr::handleMsg(RedId id, int len, QByteArray data)
{
    int n=0;
    if(id==ID_CHAT_LOGIN_RSP)
    {
        qDebug()<<"登陆聊天服务器回报";
        qDebug()<<n++;
    }
    if(id==ID_LOGIN_USER)
        {
        qDebug()<<"登陆回报"<<n++;
        }
    if(id==ID_SESRCH_USER_RSP)
        {
            qDebug()<<"搜索请求求回回包"<<n++;
        }
    auto find_iter = _handlers.find(id);
    if(find_iter == _handlers.end())
    {
        qDebug()<<"not found id";
        return;
    }
    find_iter.value()(id,len,data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug()<< "receive tcp connect signal";
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";
    _host = si.Host;
    qDebug() <<_host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(_host, _port);
}

void TcpMgr::slot_send_data(RedId reqId, QByteArray dataBytes)
{
    uint16_t id = reqId;
    if(reqId==ID_CHAT_LOGIN)qDebug()<<"正在向服务器发送登录请求";
    if(reqId==ID_SEARCH_USER_REQ)qDebug()<<"正在向服务器发送搜索请求";
    qDebug()<<"正在发送";
    // 计算长度（使用网络字节序转换）
    quint32 len = static_cast<quint32>(dataBytes.size());
    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);
    // 写入ID和长度
    out << id << len;
    // 添加字符串数据
    block.append(dataBytes);
    // 发送数据
    _socket.write(block);
    //emit sig_switch_chatdlg();
}



















