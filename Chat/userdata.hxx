#ifndef USERDATA_HXX
#define USERDATA_HXX
#include "qjsondocument.h"
#include<QString>
#include<vector>
#include<QJsonArray>
#include<QJsonObject>
class SearchInfo
{
public:
    SearchInfo(int uid,QString name,QString nick,QString desc,int sex,QString icon);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
    QString _icon;
};

class AddlyFriendApply{
public:
    AddlyFriendApply(int from_id,QString name,QString desc,QString icon,QString nick,int sex);
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
};

struct ApplyInfo{
    ApplyInfo(int uid,QString name,QString desc,QString icon,QString nick,int sex,int status):
        _uid(uid),_sex(sex),_status(status),_icon(icon),_nick(nick),_name(name),_desc(desc){}
    void SetIcon(QString head){
        _icon=head;
    }
    ApplyInfo(std::shared_ptr<AddlyFriendApply>addinfo):_uid(addinfo->_from_uid),_sex(addinfo->_sex),
        _status(0),_icon(addinfo->_icon),_nick(addinfo->_nick),_name(addinfo->_name),_desc(addinfo->_desc){}
    int _uid;
    int _sex;
    int _status;
    QString _icon;
    QString _nick;
    QString _name;
    QString _desc;

};
struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex){}
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
        _icon(peer_icon),_sex(peer_sex)
    {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};
struct TextChatData;
struct FriendInfo {
    FriendInfo(int uid, QString name, QString nick, QString icon,
               int sex, QString desc, QString back, QString last_msg=""):_uid(uid),
        _name(name),_nick(nick),_icon(icon),_sex(sex),_desc(desc),
        _back(back),_last_msg(last_msg){}

    FriendInfo(std::shared_ptr<AuthInfo> auth_info):_uid(auth_info->_uid),
        _nick(auth_info->_nick),_icon(auth_info->_icon),_name(auth_info->_name),
        _sex(auth_info->_sex){}

    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp):_uid(auth_rsp->_uid),
        _name(auth_rsp->_name),_nick(auth_rsp->_nick),_icon(auth_rsp->_icon),
        _sex(auth_rsp->_sex){}

    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    QString _back;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};
struct TextChatData;
struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = ""):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_last_msg(last_msg){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_last_msg(""){}

    UserInfo(int uid, QString name, QString icon):
        _uid(uid), _name(name), _nick(_name),_icon(icon),
        _sex(0),_last_msg(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_last_msg(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->_uid),_name(search_info->_name),_nick(search_info->_nick),
        _icon(search_info->_icon),_sex(search_info->_sex),_last_msg(""){

    }

    UserInfo(std::shared_ptr<FriendInfo> friend_info):
        _uid(friend_info->_uid),_name(friend_info->_name),_nick(friend_info->_nick),
        _icon(friend_info->_icon),_sex(friend_info->_sex),_last_msg(""){
        _chat_msgs = friend_info->_chat_msgs;
    }

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;

};
struct TextChatData{
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid,QString _nam="nu")
        :_msg_id(msg_id),_msg_content(msg_content),_name(_nam),_from_uid(fromuid),_to_uid(touid){

    }
    QString _msg_id;
    QString _msg_content;
    QString _name;
    int _from_uid;
    int _to_uid;
};

struct TextChatMsg{
    TextChatMsg(int fromuid, int touid, QJsonArray arrays):
        _to_uid(touid),_from_uid(fromuid){
        for(auto  msg_data : arrays){
            auto msg_obj = msg_data.toObject();
             auto content_value = msg_obj["content"];
             if (!content_value.isObject())
             {
                 //// 尝试将字符串解析为JSON对象
                 //QJsonDocument contentDoc = QJsonDocument::fromJson(content_value.toString().toUtf8());
                 //if (!contentDoc.isNull() && contentDoc.isObject()) {
                 //    // 解析成功，按对象处理
                 //    auto content_obj = contentDoc.object();
                 //    if (content_obj.contains("filename") && content_obj.contains("text")) {
                 //        auto filename = content_obj["filename"].toString();
                 //        auto text = content_obj["text"].toString();
                 //        auto msgid = msg_obj["msgid"].toString();
                 //        // 处理文件消息...
                 //        auto msg_ptr = std::make_shared<TextChatData>(msgid, text, fromuid, touid, filename);
                 //        _chat_msgs.push_back(msg_ptr);
                 //    }
                 //}
                 //else {
                     // 处理普通文本消息
                     auto content = content_value.toString();
                     auto msgid = msg_obj["msgid"].toString();
                     auto msg_ptr = std::make_shared<TextChatData>(msgid, content, fromuid, touid);
                     _chat_msgs.push_back(msg_ptr);
                // }
             }
             else
             {
                 auto content_obj = content_value.toObject(); // 获取 content 对象
                 auto filename = content_obj["filename"].toString();
                 auto text = content_obj["text"].toString();
                 auto msgid = msg_obj["msgid"].toString();
                 // 处理文件消息...
                 auto msg_ptr = std::make_shared<TextChatData>(msgid, text, fromuid, touid, filename);
                 _chat_msgs.push_back(msg_ptr);
             }
        //    if(!content_value.isObject())
        //     {
        //        QString contentStr = content_value.toString();
        //        QJsonDocument nestedDoc = QJsonDocument::fromJson(contentStr.toUtf8());
        //        if (nestedDoc.isObject()) {
        //            QJsonObject contentObj = nestedDoc.object();
        //            // 处理嵌套的 content 对象
        //            auto content_obj = content_value.toObject(); // 获取 content 对象
        //            auto filename = content_obj["filename"].toString(); // 提取 filename
        //            auto text = content_obj["text"].toString(); // 提取 text
        //            auto msgid = msg_obj["msgid"].toString();
        //            auto msg_ptr = std::make_shared<TextChatData>(msgid, text, fromuid, touid, filename);
        //        }
        //        else {
        //            // 处理 content 作为字符串的情况
        //            auto content = msg_obj["content"].toString();
        //            auto msgid = msg_obj["msgid"].toString();
        //            auto msg_ptr = std::make_shared<TextChatData>(msgid, content, fromuid, touid);
        //            _chat_msgs.push_back(msg_ptr);
        //        }
        //   
        //    }
        //    else
        //    {
        //        auto content_obj = content_value.toObject(); // 获取 content 对象
        //        auto filename = content_obj["filename"].toString(); // 提取 filename
        //        auto text = content_obj["text"].toString(); // 提取 text
        //        auto msgid = msg_obj["msgid"].toString();
        //        auto msg_ptr = std::make_shared<TextChatData>(msgid, text,fromuid, touid,filename);
        //    }
        }
    }
    int _to_uid;
    int _from_uid;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

#endif // USERDATA_HXX
