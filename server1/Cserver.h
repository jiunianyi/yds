#include<boost/asio.hpp>
#include"CSession.h"
#include<memory.h>
#include<mutex>
#include<map>
#include<iostream>
#include"AsioServerPool.h"
class Cserver
{
public:
	Cserver(boost::asio::io_context& io_context, short port);
	~Cserver();
	void clearsession(std::string);
	void Startaccept();
	void HandleAccept(std::shared_ptr<CSession>, const boost::system::error_code& error);
	boost::asio::io_context& _io_context;
	std::mutex _mutex;
	std::map<std::string, std::shared_ptr<CSession>> _sessions;
	short _port;
	boost::asio::ip::tcp::acceptor _acceptor;
};