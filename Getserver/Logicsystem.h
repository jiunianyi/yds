#pragma once
#include"const.h";
class Httpconnection;
typedef std::function<void(std::shared_ptr<Httpconnection>)>Httphandler;
class Logicsystem :public Singleton<Logicsystem>
{
	friend class Singleton<Logicsystem>;
public:
	~Logicsystem();
	bool HandleGet( std::string, std::shared_ptr<Httpconnection>);
	bool HandlePost(std::string, std::shared_ptr<Httpconnection>);
	void RegGet(std::string, Httphandler handler);
	void RegPost(std::string, Httphandler handler);
private:
	Logicsystem();
	    std::map<std::string, Httphandler> _post_handlers;
	std::map<std::string, Httphandler> _get_handlers;
};

