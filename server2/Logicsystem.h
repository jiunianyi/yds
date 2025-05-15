#pragma once
#include <queue>
#include <thread>
#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include"CSession.h"
#include <unordered_map>
#include"data.h"

using namespace std;
class CSession;
class LogicNode;
typedef function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;
class Logicsystem :public Singleton<Logicsystem>
{
	friend class Singleton<Logicsystem>;
public:
	~Logicsystem();
	int k = 0;
	void PostMsgToQue(shared_ptr <LogicNode> msg);
private:
	Logicsystem();
	void DealMsg();
	void RegisterCallBacks();
	bool isPureDigit(const std::string str);
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	void GetUserByName(std::string uid_str, Json::Value& rtvalue);
	void LoginHandler(shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	void SearchInfo(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	void AddFriendApply(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	void AuthFriendApply(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	void DealChatTextMsg(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);
	std::thread _worker_thread;
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
};

