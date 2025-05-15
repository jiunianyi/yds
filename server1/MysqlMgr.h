#pragma once
#include"const.h"
#include"MySQLdao.h"
class MysqlMgr : public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& pwd);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
    bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applylist, int begin, int limit = 10);
    bool AddFriendApply(const int& from, const int& to);
    bool AuthFriendApply(const int& from, const int& to);
    bool AddFriend(const int& from, const int& to, std::string back_name);
    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);
    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);
private:
    MysqlMgr();
    MysqlDao  _dao;
};

