#include "UDPmgr.h"
#include<iostream>
#include"const.h"
#include"RedisMgr.h"
#include"ConfigMgr.h"
UDPmgr::UDPmgr(boost::asio::io_context& io_context, short port):_socket(io_context,
	boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
{
	std::cout << "UDP服务器已绑定到端口 " << port << std::endl;
}

void UDPmgr::do_receive()
{
	auto self = shared_from_this();
	_socket.async_receive_from(boost::asio::buffer(recv_buffer_), sender_endpoint_, [self](const boost::system::error_code& error, 
		std::size_t bytes_transferred) {
			std::cout << "收到udp消息" << std::endl;
			std::string received_data(self->recv_buffer_.data(), bytes_transferred);
			if (!error)
			{
				Json::Value root;
				Json::Reader reader;
				int id;
				reader.parse(received_data, root);
				id = root["id"].asInt();
				if (id == ID_REGUSER_UDP)
				{
					int uid=0;
					uid = root["uid"].asInt();
					self->regnewuser( uid,self-> sender_endpoint_);
				}
				if (id == ID_HEARD)
				{
					int uid = 0;
					uid = root["uid"].asInt();
					std::cout << "心跳触发" << std::endl;
					self->heart_check(uid);
				}
				if (id == ID_SELECT_YUYIN_REQ)
				{
					std::cout << "电话";
					int touid = root["touid"].asInt();
					self->send_dhtz(touid);
				}

			}
			else
			{
				std::cout << "错误："<<error.message();
			}
			self ->do_receive();
		});
}
void UDPmgr::send_dhtz(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		std::cout << "地址：" << it->second.endpoint.address().to_string() << "端口：" << it->second.endpoint.port();
		//todo
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_SELECT_YUYIN_REQ;
		std::string return_rsp = rsp.toStyledString();
		_socket.async_send_to(boost::asio::buffer(return_rsp), re_end, [](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
	else {
		//该服务器没有该用户
		//查询redis查找touid对应的server
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
			//todo
		//将地址端口发送给前端
		}

	}
}
void UDPmgr::regnewuser(int& uid, boost::asio::ip::udp::endpoint& endpoint)
{
	if (uid == 0)
	{
		std::cout << "注册信息没有包含用户id";
		return;
	}
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{ // 用户名已存在，更新信息
		it->second.endpoint = endpoint;
		it->second.lastHeartbeat = std::chrono::steady_clock::now();
		/*std::cout << "更新客户端信息: " << uid
			<< " 地址: " << endpoint.address().to_string()
			<< " 端口: " << endpoint.port() << std::endl;*/

	}
	else
	{
		//注册新用户
		ClientInfo client;
		client.uid = uid;
		client.endpoint = endpoint;
		client.lastHeartbeat = std::chrono::steady_clock::now();
		clients_.emplace(uid, client);
		std::cout << "注册新客户端: " << uid
			<< " 地址: " << endpoint.address().to_string()
			<< " 端口: " << endpoint.port() << std::endl;
	}
}

void UDPmgr::heart_check(int &uid)
{
	if (uid == 0)
	{
		std::cout << "心跳中没有该用户" << std::endl;
		return;
	}
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{
		std::cout << "更新心跳"<<std::endl;
		it->second.lastHeartbeat = std::chrono::steady_clock::now();
	}
	else {
		std::cerr << "收到未知客户端的心跳消息: " << uid << std::endl;
	}
}
