#pragma once
#include"const.h"
class Httpconnection;
typedef std::function<void(std::shared_ptr<Httpconnection>)>HttpHandler;
class Logicsysytem :public Singleton<Logicsysytem>
{
	friend class Singleton<Logicsysytem>;
public:
	~Logicsysytem();
	bool HandleGet(std::string, std::shared_ptr<Httpconnection>);//����get����
	bool HandlePost(std::string, std::shared_ptr<Httpconnection>);//����post����

	void RegGet(std::string, HttpHandler handler);//ע��get����
	void Regpost(std::string url, HttpHandler handler);//ע��post����

private:
	Logicsysytem();
	std::map<std::string, HttpHandler >_post_handler;
	std::map<std::string, HttpHandler>_get_handler;
};

