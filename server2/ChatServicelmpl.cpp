#include "ChatServicelmpl.h"
ChatServicelmpl::ChatServicelmpl()
{

}

Status ChatServicelmpl::NotifyAddFriend(ServerContext* context, const AddFriendReq* request,
	AddFriendRsp* reply) {
	//�����û��Ƿ��ڱ�������
	auto touid = request->touid();
	auto session = UserMgr::GetInstance()->GetSession(touid);

	Defer defer([request,reply]() {
		reply->set_error(ErrorCodes::success);
		reply->set_touid(request->applyuid());
		reply->set_applyuid(request->applyuid());
		});
	if (session == nullptr)
	{
		return Status::OK;
	}
	//ֱ�ӷ���֪ͨ�Է�
	Json::Value revalue;
	revalue["error"] = ErrorCodes::success; 
	revalue["icon"] = request->icon();
	revalue["nick"] = request->nick();
	revalue["sex"] = request->sex();
	revalue["desc"] = request->desc();
	revalue["applyuid"] = request->applyuid();
	revalue["name"] = request->name();
	std::cout << "grpc���ܵ�����ǰ�˷�����Ϣ" << std::endl;
	std::string return_str = revalue.toStyledString();
	session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
	
	return Status::OK;
}
Status ChatServicelmpl::NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request,
	AuthFriendRsp* response)
{
	auto touid = request->touid();
	auto fromuid = request->fromuid();
	auto session = UserMgr::GetInstance()->GetSession(touid);

	Defer defer([request,response]() {
		response->set_error(ErrorCodes::success);
		response->set_fromuid(request->fromuid());
		response->set_touid(request->touid());
		});
	//�û�������
	if (session == nullptr)
	{
		return Status::OK;
	}
	//���ڴ���ֱ�ӷ���
	Json::Value revalue;
	revalue["error"] = ErrorCodes::success;
	revalue["fromuid"] = request->fromuid();
	revalue["touid"] = request->touid();
	std::string base_key = USER_BASE_INFO + std::to_string(fromuid);
	auto user_info = std::make_shared<UserInfo>();
	bool b_info = GetBaseInfo(base_key, fromuid, user_info);
	if (b_info)
	{
		revalue["name"] = user_info->name;
		revalue["nick"] = user_info->nick;
		revalue["icon"] = user_info->icon;
		revalue["sex"] = user_info->sex;
	}
	else {
		revalue["error"] = ErrorCodes::UidInvalid;
	}
	std::string re_return = revalue.toStyledString();
	session->Send(re_return, ID_NOTIFY_AUTH_FRIEND_REQ);
	return Status::OK;
}

Status ChatServicelmpl::NotifyTextChatMsg(::grpc::ServerContext* context,
	const TextChatMsgReq* request, TextChatMsgRsp* response) {
	//�����û��Ƿ��ڱ�������
	auto touid = request->touid();
	auto session = UserMgr::GetInstance()->GetSession(touid);
	response->set_error(ErrorCodes::success);

	//�û������ڴ�����ֱ�ӷ���
	if (session == nullptr) {
		return Status::OK;
	}

	//���ڴ�����ֱ�ӷ���֪ͨ�Է�
	Json::Value  rtvalue;
	rtvalue["error"] = ErrorCodes::success;
	rtvalue["fromuid"] = request->fromuid();
	rtvalue["touid"] = request->touid();

	//������������֯Ϊ����
	Json::Value text_array;
	for (auto& msg : request->textmsgs()) {
		Json::Value element;
		Json::Reader reader;
		Json::Value contentObj;
		//auto mm = msg.msgcontent();
		reader.parse(msg.msgcontent(), contentObj);
		if (contentObj.isObject())
		{
			element["content"] = contentObj["content"]; // ֱ�ӷ����ڲ�content����
			element["msgid"] = msg.msgid();
			text_array.append(element);
		}
		else {
			Json::Value element;
			element["content"] = msg.msgcontent();
			element["msgid"] = msg.msgid();
			text_array.append(element);
		}
		
	}
	rtvalue["text_array"] = text_array;

	std::string return_str = rtvalue.toStyledString();

	session->Send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
	return Status::OK;
}

bool ChatServicelmpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
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