#ifndef USERMGR_H
#define USERMGR_H
#include <QObject>
#include <memory>
#include "singleton.h"
#include"userdata.hxx"
#include<vector>
#include<QJsonArray>
class UserMgr:public QObject,public singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
      friend class singleton<UserMgr>;
    ~ UserMgr();

    void SetToken(QString token);
    int GetUid();
    QString GetName();
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool Alreadyapply(int uid);
    void Addapplylist(std::shared_ptr<ApplyInfo>app);
    void SetUserInfo(std::shared_ptr<UserInfo>user_info);
    void AppendApplyList(QJsonArray arry);
    void AppendFriendlist(QJsonArray arry);
    bool CheckFriendById(int uid);
    void Addfriend(std::shared_ptr<AuthRsp>auth_rsp);
    void Addfriend(std::shared_ptr<AuthInfo>auth_info);
    std::shared_ptr<FriendInfo>GetfriendByid(int uid);
     void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    std::shared_ptr<UserInfo>GetUserInfo();
private:
    UserMgr();
    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>>_apply_list;
    std::shared_ptr<UserInfo>_user_info;
      QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
      int _chat_loaded;
    int _contact_loaded;
};

#endif // USERMGR_H
