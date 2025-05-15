#include "Logicsystem.h"
#include "StatusGrpcClient.h"
#include "MysqlMgr.h"
#include "const.h"
#include "RedisMgr.h"
#include"data.h"
#include"UserMgr.h"
#include"ChatServicelmpl.h"
#include"ChatGrpcClient.h"
using namespace std;

Logicsystem::Logicsystem() :_b_stop(false) {
	RegisterCallBacks();
	_worker_thread = std::thread(&Logicsystem::DealMsg, this);
}
void Logicsystem::RegisterCallBacks() {
	_fun_callbacks[MSG_CHAT_LOGIN] = std::bind(&Logicsystem::LoginHandler, this,
		placeholders::_1, placeholders::_2, placeholders::_3);
	_fun_callbacks[ID_SEARCH_USER_REQ] = std::bind(&Logicsystem::SearchInfo, this,
		placeholders::_1, placeholders::_2, placeholders::_3); 
	_fun_callbacks[ID_ADD_FRIEND_REQ] = std::bind(&Logicsystem::AddFriendApply, this,
		placeholders::_1, placeholders::_2, placeholders::_3);
	_fun_callbacks[ID_AUTH_FRIEND_REQ] = std::bind(&Logicsystem::AuthFriendApply, this,
		placeholders::_1, placeholders::_2, placeholders::_3);
	_fun_callbacks[ID_TEXT_CHAT_MSG_REQ] = std::bind(&Logicsystem::DealChatTextMsg, this,
		placeholders::_1, placeholders::_2, placeholders::_3);
}


Logicsystem::~Logicsystem() {
	_b_stop = true;
	_consume.notify_one();
	_worker_thread.join();
	std::cout << "����"<<std::endl;
}

void Logicsystem::PostMsgToQue(shared_ptr < LogicNode> msg) {
	std::unique_lock<std::mutex> unique_lk(_mutex);
	std::cout << "ǰ" << _msg_que.size() << std::endl;
	_msg_que.push(msg);
	std::cout << "��" << _msg_que.size() << std::endl;
	//��0��Ϊ1����֪ͨ�ź�
	std::cout << "��ӡ����" << _msg_que.front() << std::endl;
	if (_msg_que.size() == 1) {
		unique_lk.unlock();
		_consume.notify_one();
	}
}

void Logicsystem::DealMsg()  {
	for (;;) {
	
		std::unique_lock<std::mutex> unique_lk(_mutex);
		//�ж϶���Ϊ�������������������ȴ������ͷ���
		while (_msg_que.empty() && !_b_stop) {
			_consume.wait(unique_lk);
		}
		//�ж��Ƿ�Ϊ�ر�״̬���������߼�ִ��������˳�ѭ��
		if (_b_stop) {
			while (!_msg_que.empty()) {
				auto msg_node = _msg_que.front();
				cout << "recv_msg id  is " << msg_node->_recvnode->_msg_id << endl;
				auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
				if (call_back_iter == _fun_callbacks.end()) {
					std::cout << "δ�ҵ�" << std::endl;
					_msg_que.pop();
					continue;
				}
				call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
					std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
				_msg_que.pop();
			}
			break;
		}

		//���û��ͣ������˵��������������
		//std::this_thread::sleep_for(std::chrono::seconds(10));
		auto msg_node = _msg_que.front();
		if (msg_node->_recvnode->_msg_id == ID_SEARCH_USER_REQ)std::cout << "�ҵ�����ѡ��" << std::endl;
		if (msg_node->_recvnode->_msg_id == MSG_CHAT_LOGIN)std::cout << "�ҵ���¼ѡ��" << std::endl;
		cout << "recv_msg id  is " << msg_node->_recvnode->_msg_id << endl;
		auto call_back_iter = _fun_callbacks.find(msg_node->_recvnode->_msg_id);
		if (call_back_iter == _fun_callbacks.end()) {
			_msg_que.pop();
			std::cout << "msg id [" << msg_node->_recvnode->_msg_id << "] handler not found" << std::endl;
			continue;
		}
	
		call_back_iter->second(msg_node->_session, msg_node->_recvnode->_msg_id,
			std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_cur_len));
		_msg_que.pop();
		std::cout << "ִ����֮����д�С" << _msg_que.size() << std::endl;
	}
	std::cout << "�˳�forѭ��" << std::endl;

}
bool Logicsystem::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	//���Ȳ�redis�в�ѯ�û���Ϣ
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << endl;
	}
	else {
		//redis��û�����ѯmysql
		//��ѯ���ݿ�
		std::shared_ptr<UserInfo> user_info = nullptr;
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			return false;
		}

		userinfo = user_info;

		//�����ݿ�����д��redis����
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;
		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}

	return true;
}
bool Logicsystem::GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list)
{
	//��ú��������б�
	return MysqlMgr::GetInstance()->GetApplyList(to_uid, list, 0, 10);
}
void Logicsystem::AuthFriendApply(std::shared_ptr<CSession>session, const short& msg_id, const string& msg_data)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(msg_data, root);

	auto uid = root["fromuid"].asInt();
	auto touid = root["touid"].asInt();
	auto back_name = root["backname"].asString();
	std::cout << "from" << uid << "auth friend uid " << touid << std::endl;
	Json::Value revalue;
	revalue["error"] = ErrorCodes::success;
	auto user_info = std::make_shared<UserInfo>();
	std::string base_key = USER_BASE_INFO + std::to_string(touid);
	bool b_info = GetBaseInfo(base_key, touid, user_info);
	if (b_info)
	{
		revalue["name"] = user_info->name;
		revalue["nick"] = user_info->nick;
		revalue["icon"] = user_info->icon;
		revalue["sex"] = user_info->sex;
		revalue["uid"] = user_info->uid;
	}
	else{
		revalue["error"] = ErrorCodes::UidInvalid;
	}
	Defer defer([&revalue,this,session]() {
		std::string return_str = revalue.toStyledString();
		session->Send(return_str, ID_AUTH_FRIEND_RSP);
		});
	//�ȸ������ݿ�
	MysqlMgr::GetInstance()->AuthFriendApply(uid, touid);

	//�������ݿ���Ӻ���
	MysqlMgr::GetInstance()->AddFriend(uid, touid, back_name);

	//��ѯreadis����touid��Ӧ��sever֪ͨ�Է�������֤ͨ��
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ipserver = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ipserver)
	{
		return;
	}
	auto& cfg = ConfigMgr::Info();
	auto self_name = cfg["SelfServer"]["Name"];
	if (self_name == to_ip_value)
	{
		auto session = UserMgr::GetInstance()->GetSession(touid);
		if (session)
		{
			Json::Value notify;
			notify["error"] = ErrorCodes::success;
			notify["fromuid"] = uid;
			notify["touid"] = touid;
			std::string base_key = USER_BASE_INFO + uid;
			auto  user_info = std::make_shared<UserInfo>();
			bool b_bol = GetBaseInfo(base_key, uid,user_info);
			if (b_bol)
			{
				notify["name"] = user_info->name;
				notify["nick"] = user_info->nick;
				notify["icon"] = user_info->icon;
				notify["sex"] = user_info->sex;
			}
			else
			{
				notify["error"] = ErrorCodes::success;
			}
			std::string re_turn = notify.toStyledString();
			session->Send(re_turn, ID_NOTIFY_AUTH_FRIEND_REQ);
		}
		return;
	}
	AuthFriendReq auth_req;
	auth_req.set_fromuid(uid);
	auth_req.set_touid(touid);

	//����֪ͨ
	ChatGrpcClient::GetInstance()->NotifyAuthFriend(to_ip_value, auth_req);

}
void Logicsystem::DealChatTextMsg(std::shared_ptr<CSession>session, const short& msg_id, const string& msg_data)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(msg_data, root);
	auto uid = root["fromuid"].asInt();
	auto touid = root["touid"].asInt();
	const Json::Value arrays = root["text_array"];
	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::success;
	rtvalue["text_array"] = arrays;
	rtvalue["fromuid"] = uid;
	rtvalue["touid"] = touid;
	Defer defer([this,&rtvalue,session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, ID_TEXT_CHAT_MSG_RSP);
		});
	//��ѯredis����touid��Ӧ��server
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ip)
	{
		return;
	}
	auto& cfg = ConfigMgr::Info();
	auto self_name = cfg["SelfServer"]["Name"];
	if (self_name == to_ip_value)
	{
		auto session = UserMgr::GetInstance()->GetSession(touid);
		if (session)
		{
			std::string return_str = rtvalue.toStyledString();
			session->Send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
		}
		return;
	}
	TextChatMsgReq text_msg_req;
	text_msg_req.set_fromuid(uid);
	text_msg_req.set_touid(touid);
	for (const auto& txt_obj : arrays)
	{
		auto content_value = txt_obj["content"];
		auto msgid = txt_obj["msgid"].asString();

		if (!content_value.isObject()) {
			auto content = txt_obj["content"].asString();
			std::cout << "content is " << content << std::endl;
			std::cout << "msgid is " << msgid << std::endl;

			Json::Value v;
			v["content"] = content;
			auto* text_msg = text_msg_req.add_textmsgs();
			text_msg->set_msgid(msgid);
			text_msg->set_msgcontent(Json::FastWriter().write(v));
		}
		else {
			auto content_obj = content_value; // ��ȡ content ����
			auto filename = content_obj["filename"].asString(); // ��ȡ filename
			auto text = content_obj["text"].asString(); // ��ȡ text

			Json::Value context_obj;
			context_obj["filename"] = filename;
			context_obj["text"] = text;

			// �� context_obj ���� final_obj �� content �ֶ���
			Json::Value final_obj;
			final_obj["content"] = context_obj;
			auto* text_msg = text_msg_req.add_textmsgs();
			text_msg->set_msgid(msgid);
			text_msg->set_msgcontent(Json::FastWriter().write(final_obj));
		}
		
	}


	//����֪ͨ todo...
	ChatGrpcClient::GetInstance()->NotifyTextChatMsg(to_ip_value, text_msg_req, rtvalue);
}
void Logicsystem::LoginHandler	(shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	std::cout << "���ܵ�ǰ�˵ĵ�¼����" << std::endl;
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid = root["uid"].asInt();
	auto token = root["token"].asString();

	std::cout << root["error"].asInt();
	root["email"].asString();
	std::cout << "user login uid is  " << uid << " user token  is "
		<< token << endl;


	Json::Value  rtvalue;
	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, MSG_CHAT_LOGIN_RSP);
		});

	//��redis��ȡ�û�token�Ƿ���ȷ
	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	if (!success) {
		root["error"] = ErrorCodes::UidInvalid;
		return;
	}

	if (token_value != token) {
		root["error"] = ErrorCodes::TokenInvalid;
		return;
	}

	root["error"] = ErrorCodes::success;

	std::string base_key = USER_BASE_INFO + uid_str;
	auto user_info = std::make_shared<UserInfo>();
	bool b_base = GetBaseInfo(base_key, uid, user_info);
	if (!b_base) {
		root["error"] = ErrorCodes::UidInvalid;
		return;
	}
	rtvalue["uid"] = uid;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;

	//�����ݿ��ȡ�����б�
	std::vector<std::shared_ptr<ApplyInfo>> apply_list;
	auto b_apply = GetFriendApplyInfo(uid, apply_list);
	if (b_apply)
	{
		for (auto& apply : apply_list) {
			Json::Value obj;
			obj["name"] = apply->_name;
			obj["uid"] = apply->_uid;
			obj["icon"] = apply->_icon;
			obj["desc"] = apply->_desc;
			obj["sex"] = apply->_sex;
			obj["status"] = apply->_status;
			rtvalue["apply_list"].append(obj);
		}
	}
		//��ȡ�����б�
	std::vector<std::shared_ptr<UserInfo>> friend_list;
	bool b_friend_list = GetFriendList(uid, friend_list);
	for (auto& friend_l : friend_list)
	{
		Json::Value obj;
		obj["name"] = friend_l->name;
		obj["uid"] = friend_l->uid;
		obj["icon"] = friend_l->icon;
		obj["nick"] = friend_l->nick;
		obj["sex"] = friend_l->sex;
		obj["desc"] = friend_l->desc;
		obj["back"] = friend_l->back;
		rtvalue["friend_list"].append(obj);

	}
	auto server_name = ConfigMgr::Info().GetValue("SelfServer", "Name");
	//����¼��������
	auto rd_res = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server_name);
	int count = 0;
	if (!rd_res.empty()) {
		count = std::stoi(rd_res);
	}

	count++;

	auto count_str = std::to_string(count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, count_str);

	//session���û�uid
	session->SetUserId(uid);

	//Ϊ�û����õ�¼ip server������
	std::string  ipkey = USERIPPREFIX + uid_str;
	RedisMgr::GetInstance()->Set(ipkey, server_name);

	//uid��session�󶨹���,�����Ժ����˲���
	UserMgr::GetInstance()->SetUserSession(uid, session);

	return;
}

void Logicsystem::SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	std::cout << "���ܵ�ǰ�˵���������" << std::endl;
	Json::Reader reader;
	Json::Value		root;
	reader.parse(msg_data, root);
	auto uid_str = root["uid"].asString();
	std::cout << "user searchinfo uid is " << uid_str << endl;
	Json::Value  rtvalue;

	Defer defer([this, &rtvalue, session]() {
		std::string return_str = rtvalue.toStyledString();
		session->Send(return_str, ID_SEARCH_USER_RSP);
		});
	
	
	/*bool b_digit = isPureDigit(uid_str);
	if (b_digit) {
		GetUserByUid(uid_str, rtvalue);
	}
	else {
		GetUserByName(uid_str, rtvalue);
	}*/
	GetUserByName(uid_str, rtvalue);
	
}
void Logicsystem::AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data)
{
	Json::Reader reader;
	Json::Value root;
	reader.parse(msg_data, root);
	auto uid = root["fromuid"].asInt();
	auto applyname = root["applyname"].asString();
	auto backname = root["backname"].asString();
	auto touid = root["touid"].asInt();
	std::cout << "������Ӻ��ѵ�uid" << touid << std::endl;
	Json::Value revalue;
	Defer defer([this, &revalue, session]() {
		std::string return_str = revalue.toStyledString();
		session->Send(return_str, ID_ADD_FRIEND_RSP);
		});
	//�ȸ������ݿ�
	MysqlMgr::GetInstance()->AddFriendApply(uid, touid);
	//��ѯredis����touid��Ӧ��server_ip;
	auto to_str = std::to_string(touid);
	auto to_ip_key = USERIPPREFIX + to_str;
	std::string to_ip_value = "";
	bool b_ip = RedisMgr::GetInstance()->Get(to_ip_key, to_ip_value);
	if (!b_ip)
	{
		return;
	}
	std::string base_key = USER_BASE_INFO + std::to_string(uid);
	auto apply_info = std::make_shared<UserInfo>();
	bool b_info = GetBaseInfo(base_key, uid, apply_info);

	auto& cfg = ConfigMgr::Info();
	auto self_name = cfg["SelfServer"]["Name"];
	//���������һ��������ֱ�ӷ���������Ϣ
	if (self_name == to_ip_value)
	{
		auto session = UserMgr::GetInstance()->GetSession(touid);
		if (session)
		{
			Json::Value notify;
			notify["error"] = ErrorCodes::success;
			notify["applyuid"] = uid;
			notify["name"] = applyname;
			notify["desc"] = "";
			notify["icon"] = apply_info->icon;
			notify["sex"] = apply_info->sex;
			notify["nick"] = apply_info->nick;
			std::string return_str = notify.toStyledString();
			session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
		}
		return;
	}


	AddFriendReq add_req;
	add_req.set_applyuid(uid);
	add_req.set_touid(touid);
	add_req.set_name(applyname);
	add_req.set_desc("");
	if (b_info)
	{
		add_req.set_icon(apply_info->icon);
		add_req.set_nick(apply_info->nick);
		add_req.set_sex(apply_info->sex);
	}
	//����֪ͨ
	ChatGrpcClient::GetInstance()->NotifyAddFriend(to_ip_value, add_req);
}
bool Logicsystem::isPureDigit(const std::string str)
{
	for (char c : str)
	{
		if (!std::isdigit(c))
		{
			return false;
		}
	}
	return true;
}

void Logicsystem::GetUserByUid(std::string uid_str, Json::Value& rtvalue)
{
	rtvalue["error"] = ErrorCodes::success;
	std::string base_key = USER_BASE_INFO + uid_str;
	//����redis�в��û���Ϣ
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email << " icon is " << icon << endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
		return;
	}
	auto uid = std::stoi(uid_str);
	//redis��û�����ѯmysql
	//��ѯ���ݿ�
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(uid);
	if (user_info == nullptr) {
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	//�����ݿ�����д��redis����
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	redis_root["icon"] = user_info->icon;

	//���������ø�ǰ��
	rtvalue["uid"] = user_info->uid;;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());

}

void Logicsystem::GetUserByName(std::string name, Json::Value& rtvalue)
{
	rtvalue["error"] = ErrorCodes::success;
	std::string base_key = NAME_INFO + name;
	//����redis�в��û���Ϣ
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key,  info_str);
	/*if (b_base)
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		auto uid = root["uid"].asInt();
		auto name = root["name"].asString();
		auto pwd = root["pwd"].asString();
		auto email = root["email"].asString();
		auto nick = root["nick"].asString();
		auto desc = root["desc"].asString();
		auto sex = root["sex"].asInt();
		auto icon = root["icon"].asString();
		std::cout << "user  uid is  " << uid << " name  is "
			<< name << " pwd is " << pwd << " email is " << email  << endl;

		rtvalue["uid"] = uid;
		rtvalue["pwd"] = pwd;
		rtvalue["name"] = name;
		rtvalue["email"] = email;
		rtvalue["nick"] = nick;
		rtvalue["desc"] = desc;
		rtvalue["sex"] = sex;
		rtvalue["icon"] = icon;
	
		return;
	}*/
	//redis��û�����ѯmysql
	//��ѯ���ݿ� 
	std::shared_ptr<UserInfo> user_info = nullptr;
	user_info = MysqlMgr::GetInstance()->GetUser(name);
	if (user_info == nullptr) {
		rtvalue["error"] = ErrorCodes::UidInvalid;
		return;
	}

	//�����ݿ�����д��redis����
	Json::Value redis_root;
	redis_root["uid"] = user_info->uid;
	redis_root["pwd"] = user_info->pwd;
	redis_root["name"] = user_info->name;
	redis_root["email"] = user_info->email;
	redis_root["nick"] = user_info->nick;
	redis_root["desc"] = user_info->desc;
	redis_root["sex"] = user_info->sex;
	redis_root["icon"] = user_info->icon;
	//���������ø�ǰ��
	rtvalue["uid"] = user_info->uid;;
	rtvalue["pwd"] = user_info->pwd;
	rtvalue["name"] = user_info->name;
	rtvalue["email"] = user_info->email;
	rtvalue["nick"] = user_info->nick;
	rtvalue["desc"] = user_info->desc;
	rtvalue["sex"] = user_info->sex;
	rtvalue["icon"] = user_info->icon;
	RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
}
bool Logicsystem::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list)
{
	//��mysql�л�ȡ�����б�
	return MysqlMgr::GetInstance()->GetFriendList(self_id, user_list);
}

