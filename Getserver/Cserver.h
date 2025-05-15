#pragma once
#include<boost/beast/http.hpp>
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<memory>

class Cserver:public std::enable_shared_from_this<Cserver>
{
public:
	Cserver(boost::asio::io_context& ioc, unsigned short& port);
	void start();
private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& _ioc;
	

};

