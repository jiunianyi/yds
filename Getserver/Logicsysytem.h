#pragma once
#include"const.h"
class Httpconnection;
typedef std::function<void(std::shared_ptr<Httpconnection>)>HttpHandler;
class Logicsysytem :public Singleton<Logicsysytem>
{
	friend class Singleton<Logicsysytem>;
public:
	~Logicsysytem();
	bool HandleGet(std::string, std::shared_ptr<Httpconnection>);//处理get请求
	bool HandlePost(std::string, std::shared_ptr<Httpconnection>);//处理post请求

	void RegGet(std::string, HttpHandler handler);//注册get请求
	void Regpost(std::string url, HttpHandler handler);//注册post请求

private:
	Logicsysytem();
	std::map<std::string, HttpHandler >_post_handler;
	std::map<std::string, HttpHandler>_get_handler;
};

