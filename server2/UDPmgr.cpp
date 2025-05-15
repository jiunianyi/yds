#include "UDPmgr.h"
#include<iostream>
#include"const.h"
#include"RedisMgr.h"
#include"ConfigMgr.h"
UDPmgr::UDPmgr(boost::asio::io_context& io_context, short port):_socket(io_context,
	boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
{
	std::cout << "UDP�������Ѱ󶨵��˿� " << port << std::endl;
}

void UDPmgr::do_receive()
{
	auto self = shared_from_this();
	_socket.async_receive_from(boost::asio::buffer(recv_buffer_), sender_endpoint_, [self](const boost::system::error_code& error, 
		std::size_t bytes_transferred) {
			std::cout << "�յ�udp��Ϣ" << std::endl;
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
					std::cout << "��������" << std::endl;
					self->heart_check(uid);
				}
				if (id == ID_SELECT_YUYIN_REQ)
				{
					std::cout << "�绰";
					int touid = root["touid"].asInt();
					self->send_dhtz(touid);
				}

			}
			else
			{
				std::cout << "����"<<error.message();
			}
			self ->do_receive();
		});
}
void UDPmgr::send_dhtz(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		std::cout << "��ַ��" << it->second.endpoint.address().to_string() << "�˿ڣ�" << it->second.endpoint.port();
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
		//�÷�����û�и��û�
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
			//todo
		//����ַ�˿ڷ��͸�ǰ��
		}

	}
}
void UDPmgr::regnewuser(int& uid, boost::asio::ip::udp::endpoint& endpoint)
{
	if (uid == 0)
	{
		std::cout << "ע����Ϣû�а����û�id";
		return;
	}
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{ // �û����Ѵ��ڣ�������Ϣ
		it->second.endpoint = endpoint;
		it->second.lastHeartbeat = std::chrono::steady_clock::now();
		/*std::cout << "���¿ͻ�����Ϣ: " << uid
			<< " ��ַ: " << endpoint.address().to_string()
			<< " �˿�: " << endpoint.port() << std::endl;*/

	}
	else
	{
		//ע�����û�
		ClientInfo client;
		client.uid = uid;
		client.endpoint = endpoint;
		client.lastHeartbeat = std::chrono::steady_clock::now();
		clients_.emplace(uid, client);
		std::cout << "ע���¿ͻ���: " << uid
			<< " ��ַ: " << endpoint.address().to_string()
			<< " �˿�: " << endpoint.port() << std::endl;
	}
}

void UDPmgr::heart_check(int &uid)
{
	if (uid == 0)
	{
		std::cout << "������û�и��û�" << std::endl;
		return;
	}
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{
		std::cout << "��������"<<std::endl;
		it->second.lastHeartbeat = std::chrono::steady_clock::now();
	}
	else {
		std::cerr << "�յ�δ֪�ͻ��˵�������Ϣ: " << uid << std::endl;
	}
}
