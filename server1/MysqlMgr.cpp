#include "MysqlMgr.h"

MysqlMgr::~MysqlMgr() {
}
int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    return _dao.RegUser(name, email, pwd);
}
MysqlMgr::MysqlMgr() {
}
bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
    return _dao.CheckEmail(name,email);
}
bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
    return _dao.UpdatePwd(name,pwd);
}
bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo)
{
    return _dao.CheckPwd(email, pwd, userinfo);
}
bool MysqlMgr::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applylist, int begin, int limit)
{
    return _dao.GetApplyList(touid, applylist, begin, limit);
}
bool MysqlMgr::AddFriendApply(const int& from, const int& to)
{
    return _dao.AddFriendApply(from,to);
}
bool MysqlMgr::AuthFriendApply(const int& from, const int& to)
{
    return _dao.AuthFriendApply(from,to);
}
bool MysqlMgr::AddFriend(const int& from, const int& to, std::string back_name)
{
    return _dao.AddFriend(from, to, back_name);
}
bool MysqlMgr::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list)
{
    return _dao.GetFriendList(self_id, user_list);
}
std::shared_ptr<UserInfo> MysqlMgr::GetUser(int uid)
{
    return _dao.GetUser(uid);
}
std::shared_ptr<UserInfo> MysqlMgr::GetUser(std::string name)
{
    return _dao.GetUser(name);
}


