#include "usermgr.h"
#include "global.h"
#include "qjsonarray.h"

UserMgr::~UserMgr()
{

}
// QPushButton {
//     background-color: #4CAF50;
// color: white;
// border: none;
// padding: 8px 16px;
//     border-radius: 4px;
//     font-size: 14px;
//     min-width: 80px;
// }

void UserMgr::SetToken(QString token)
{
    _token = token;
}

int UserMgr::GetUid()
{
    return _user_info->_uid;
}

QString UserMgr::GetName()
{
    return _user_info->_name;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    return _apply_list;
}

bool UserMgr::Alreadyapply(int uid)
{
    for(auto &apply:_apply_list)
    {
        if(apply->_uid==uid)
        {
            return true;
        }
    }
    return false;
}

void UserMgr::Addapplylist(std::shared_ptr<ApplyInfo> app)
{
    _apply_list.push_back(app);
}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info=user_info;
}

void UserMgr::AppendApplyList(QJsonArray arry)
{
    //将好友申请放入列表
    for (const QJsonValue &value : arry) {
        qDebug()<<"好友申请已添加";
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                                                desc, icon, nick, sex, status);
        _apply_list.push_back(info);
    }
}

void UserMgr::AppendFriendlist(QJsonArray arry)
{
    for(const QJsonValue& value:arry)
    {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex =  value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();

        auto info = std::make_shared<FriendInfo>(uid,name,nick,icon,sex,desc,back);
        _friend_list.push_back(info);
        _friend_map.insert(uid,info);
    }
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter = _friend_map.find(uid);
    if(iter==_friend_map.end())
    {
        return false;
    }
    return true;
}


void UserMgr::Addfriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    _friend_map[friend_info->_uid]=friend_info;
}

void UserMgr::Addfriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    _friend_map[friend_info->_uid]=friend_info;
}

std::shared_ptr<FriendInfo> UserMgr::GetfriendByid(int uid)
{
    auto find_it = _friend_map.find(uid);
    if(find_it==_friend_map.end())
    {
        return nullptr;
    }
    return *find_it;
}

void UserMgr::AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData> >msgs)
{
    auto find_iter = _friend_map.find(friend_id);
    if(find_iter == _friend_map.end()){
        qDebug()<<"append friend uid  " << friend_id << " not found";
        return;
    }

    find_iter.value()->AppendChatMsgs(msgs);
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetChatListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = _chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return friend_list;
    }

    if (end > _friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.begin()+ end);
    return friend_list;
}

bool UserMgr::IsLoadChatFin()
{
    if (_chat_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

void UserMgr::UpdateChatLoadedCount()
{
    int begin = _chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return ;
    }

    if (end > _friend_list.size()) {
        _chat_loaded = _friend_list.size();
        return ;
    }

    _chat_loaded = end;
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetConListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = _contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return friend_list;
    }

    if (end > _friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.begin() + end);
    return friend_list;
}

void UserMgr::UpdateContactLoadedCount()
{
    int begin = _contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return;
    }

    if (end > _friend_list.size()) {
        _contact_loaded = _friend_list.size();
        return;
    }

    _contact_loaded = end;
}

bool UserMgr::IsLoadConFin()
{
    if (_contact_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    return _user_info;
}

UserMgr::UserMgr():_user_info(nullptr),_chat_loaded(0),_contact_loaded(0)
{

}
