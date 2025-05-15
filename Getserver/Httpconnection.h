#pragma once
#include"const.h"
class Httpconnection:public std::enable_shared_from_this<Httpconnection>
{
public:
	friend class Logicsystem;
	Httpconnection(boost::asio::io_context & ioc);
	void start();
	boost::asio::ip::tcp::socket& Getsocket() {
		return _socket;
	}
private:
	void checkdeadline();
	void Writeresponse();
	void Handlereq();
	void PreParseGetParam();
	boost::asio::ip::tcp::socket _socket;
	boost::beast::flat_buffer _buffer{ 8192 };
	boost::beast::http::request<boost::beast::http::dynamic_body>_request;// 用来解析请求
	boost::beast::http::response<boost::beast::http::dynamic_body>_respone;//用来回应客户端
	boost::asio::steady_timer deadline_{//超时检测
		_socket.get_executor(), std::chrono::seconds(60) };

	std::string _get_url;//处理请求的参数解析
	std::unordered_map<std::string, std::string> _get_params;
};

