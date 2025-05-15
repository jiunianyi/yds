#include "Logicsystem.h"
#include"Httpconnection.h"
#include"VerifyGrpcClient.h"
#include"RedisMgr.h"
#include"MysqlMgr.h"
#include"StatusGrpcClient.h"
Logicsystem::~Logicsystem()
{

}
void Logicsystem::RegGet(std::string url,Httphandler handler)
{
	_get_handlers.insert(std::make_pair(url, handler));
}

void Logicsystem::RegPost(std::string url, Httphandler handler)
{
	_post_handlers.insert(std::make_pair(url, handler));
}

Logicsystem::Logicsystem()
{
	RegGet("/get_test", [](std::shared_ptr<Httpconnection> connection) {
		boost::beast::ostream(connection->_respone.body()) << "recvive get_test req";
		int i = 0;
		for (auto& elem : connection->_get_params) {
			i++;
			boost::beast::ostream(connection->_respone.body()) << "param" << i << " key is " << elem.first;
			boost::beast::ostream(connection->_respone.body()) << ", " << " value is " << elem.second << std::endl;
		}
		});
	
	RegPost("/get_varifycode", [](std::shared_ptr<Httpconnection>connection) {
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "recvive body is " << body_str << std::endl;
		connection->_respone.set(boost::beast::http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success)
		{
			std::cout << "failed to parse json data!" << std::endl;
			root["error"] = ErrorCodes::Error_json;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		if (!src_root.isMember("email"))
		{
			std::cout << "failed to parse json data!" << std::endl;
			root["error"] = ErrorCodes::Error_json;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		auto email = src_root["email"].asString();
		GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
		std::cout << "email is :" << email << std::endl;
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		std::string jsonstr = root.toStyledString();
		boost::beast::ostream(connection->_respone.body()) << jsonstr;
		return true;  
		});

	RegPost("/user_register", [](std::shared_ptr<Httpconnection> connection) {
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->_respone.set(boost::beast::http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_json;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		auto icon = src_root["icon"].asString();
		auto confirm = src_root["confirm"].asString();
		auto name = src_root["user"].asCString();
		auto email = src_root["email"].asCString();
		auto pwd = src_root["passwd"].asCString();

		if (pwd != confirm)
		{
			std::cout << "passwd is err" << std::endl;
			root["error"] = ErrorCodes::PasswdErr;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		//先查找redis中email对应的验证码是否合理
		std::string  varify_code;
		bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX+src_root["email"].asString(), varify_code);
		if (!b_get_varify) {
			std::cout << " get varify code expired" << std::endl;
			root["error"] = ErrorCodes::VarifyExpired;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		if (varify_code != src_root["varifycode"].asString()) {
			std::cout << " varify code error" << std::endl;
			root["error"] = ErrorCodes::VarifyCodeErr;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		
		
		//查找数据库判断用户是否存在
		int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd, icon);
		if (uid == 0 || uid == -1)
		{
			std::cout << "user or email exist" << std::endl;
			root["error"] = ErrorCodes::UserExist;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}

		root["error"] = 0;
		root["email"] = email;
		root["uuid"] = uid;
		root["user"] = name;
		root["passwd"] = pwd;
		root["icon"] = icon;
		root["confirm"] = src_root["confirm"].asString();
		root["varifycode"] = src_root["varifycode"].asString();
		std::string jsonstr = root.toStyledString();
		boost::beast::ostream(connection->_respone.body()) << jsonstr;
		return true;
		});

	RegPost("/reset_pwd", [](std::shared_ptr<Httpconnection> connection) {
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->_respone.set(boost::beast::http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_json;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		auto email = src_root["email"].asString();
		auto name = src_root["user"].asString();
		auto pwd = src_root["passwd"].asString();
		//先查找redis中email对应的验证码是否合理
		std::string  varify_code;
		bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), varify_code);
		if (!b_get_varify) {
			std::cout << " get varify code expired" << std::endl;
			root["error"] = ErrorCodes::VarifyExpired;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		if (varify_code != src_root["varifycode"].asString()) {
			std::cout << " varify code error" << std::endl;
			root["error"] = ErrorCodes::VarifyCodeErr;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		//查询数据库判断用户名和邮箱是否匹配
		bool email_valid = MysqlMgr::GetInstance()->CheckEmail(name, email);
		if (!email_valid) {
			std::cout << " user email not match" << std::endl;
			root["error"] = ErrorCodes::EmailNotMatch;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		//更新密码为最新密码
		bool b_up = MysqlMgr::GetInstance()->UpdatePwd(name, pwd);
		if (!b_up) {
			std::cout << " update pwd failed" << std::endl;
			root["error"] = ErrorCodes::PasswdUpFailed;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}
		std::cout << "succeed to update password" << pwd << std::endl;
		root["error"] = 0;
		root["email"] = email;
		root["user"] = name;
		root["passwd"] = pwd;
		root["varifycode"] = src_root["varifycode"].asString();
		std::string jsonstr = root.toStyledString();
		boost::beast::ostream(connection->_respone.body()) << jsonstr;
		return true;
		});

	RegPost("/user_login", [](std::shared_ptr<Httpconnection> connection) {
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is:" << body_str << std::endl;
		connection->_respone.set(boost::beast::http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success)
		{
			std::cout << "failed to parse json data!" << std::endl;
			root["error"] = ErrorCodes::Error_json;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
		}
		auto name = src_root["user"].asString();
		auto pwd = src_root["passwd"].asString();
		UserInfo userInfo;
		//查询数据库判断用户名和密码是否匹配
		bool pwd_valid = MysqlMgr::GetInstance()->CheckPwd(name, pwd, userInfo);
		if (!pwd_valid) {
			std::cout << " user pwd not match" << std::endl;
			root["error"] = ErrorCodes::PasswdInvalid;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}

		//查询StatusServer找到合适的连接
		auto reply = StatusGrpcClient::GetInstance()->GetChatServer(userInfo.uid);
		if (reply.error()) {
			std::cout << " grpc get chat server failed, error is " << reply.error() << std::endl;
			root["error"] = ErrorCodes::RPCGetFailed;
			std::string jsonstr = root.toStyledString();
			boost::beast::ostream(connection->_respone.body()) << jsonstr;
			return true;
		}

		std::cout << "succeed to load userinfo uid is " << userInfo.uid << std::endl;
		root["error"] = 0;
		root["user"] = name;
		root["uid"] = userInfo.uid;
		root["token"] = reply.token();
		root["host"] = reply.host();
		root["port"] = reply.port();
		std::cout << root["error"];
	//	std::cout << reply.port()<<std::endl;
		//std::cout<< reply.host();
		//std::cout << reply.token();
		root["email"] = src_root["user"].asString();

		std::string jsonstr = root.toStyledString();
		boost::beast::ostream(connection->_respone.body()) << jsonstr;
		return true;
		});

 }

bool Logicsystem::HandleGet(std::string path, std::shared_ptr<Httpconnection> con)
{
	if (_get_handlers.find(path) == _get_handlers.end())
	{
		return false;
	}
	_get_handlers[path](con);
	return true;
}

bool Logicsystem::HandlePost(std::string path, std::shared_ptr<Httpconnection> con)
{
	if (_post_handlers.find(path) == _post_handlers.end())
	{
		return false;
	}
	_post_handlers[path](con);
	return true;
}

