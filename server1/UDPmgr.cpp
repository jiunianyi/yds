#include "UDPmgr.h"
#include<iostream>
#include"const.h"
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
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
					int uid = 0;
					uid = root["uid"].asInt();
					self->regnewuser(uid, self->sender_endpoint_);
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
					int fromuid = root["fromuid"].asInt();
					self ->send_dhtz(touid, fromuid);
				}
				if (id == ID_REFUSE_YUYIN_REQ)
				{
					int touid = root["touid"].asInt();
					self->send_reftz(touid);
				}
				if (id == ID_YUYIN)
				{
					int touid = root["touid"].asInt();
					self->encodedData = root["context"].toStyledString();
					std::cout << self->encodedData;
					self->jing_th(touid);
				}
				if (id == ID_ACCEPT_YUNYI_REQ)
				{
					int touid = root["touid"].asInt();
					self->acc_th(touid);
				}
				if (id == ID_SELECT_SHIPIN_REQ)
				{
					int uid = 0;
					uid = root["touid"].asInt();
					int fromuid = root["fromuid"].asInt();
					self->seleship(uid,fromuid);
				}
				if (id == ID_SHIPIN)
				{
					int touid = root["touid"].asInt();
					int fromuid = root["fromuid"].asInt();
					self->shipindata = root["content"].toStyledString();
					self->size = root["size"].asInt();
					self->sendsosj(touid, fromuid);
				}
				if (id == ID_ACCEPT_SHIPIN_REQ)
				{

					int touid = root["touid"].asInt();
					int fromuid = root["fromuid"].asInt();
					self->accsend(touid, fromuid);
				}
				if (id == ID_YYGUADUAN)
				{
					int touid = root["touid"].asInt();
					self->guaduan(touid);
				}
				if (id == ID_SPGUADUAN)
				{
					int touid = root["touid"].asInt();
					self->guaduansp(touid);
				}

			}
			else
			{
				{
					std::cout << "����" << error.message();
				}
			}
			self->do_receive();
		});
}

void UDPmgr::send_dhtz(int& touid,int &fromuid)
{
	try {
		auto it = clients_.find(touid);
		if (it != clients_.end())
		{
			std::cout << "��ַ��" << it->second.endpoint.address().to_string() << "�˿ڣ�" << it->second.endpoint.port();
			//todo
			auto re_end = it->second.endpoint;
			Json::Value rsp;
			rsp["id"] = ID_SELECT_YUYIN_REQ;
			rsp["fromuid"] = fromuid;
			rsp["touid"] = touid;
			auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
			std::cout << "Sending response: " << buffer_ptr << std::endl;
			_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
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
	catch (std::exception& e)
	{
		std::cout << "ccccccccc"<<e.what();
	}
}

void UDPmgr::send_reftz(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		std::cout << "��ַ��" << it->second.endpoint.address().to_string() << "�˿ڣ�" << it->second.endpoint.port();
		//todo
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_REFUSE_YUYIN_REQ;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::jing_th(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{//
		std::cout << "��ַ��" << it->second.endpoint.address().to_string() << "�˿ڣ�" << it->second.endpoint.port() << std::endl;
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_YUYIN;
		rsp["context"] = encodedData;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString());
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}

				/*std::size_t size = strlen(encodedData);
				std::cout << size << std::endl;
				_socket.send_to(boost::asio::buffer(encodedData, size), re_end);*/
			});
	}
}

void UDPmgr::acc_th(int& uid)
{
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_ACCEPT_YUNYI_REQ;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::seleship(int& uid,int &fromuid)
{
	auto it = clients_.find(uid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_SELECT_SHIPIN_REQ;
		rsp["fromuid"] = fromuid;
		rsp["touid"] = uid;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::sendsosj(int& touid, int& fromuid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_SHIPIN;
		rsp["fromuid"] = fromuid;
		rsp["touid"] = touid;
		rsp["content"] = shipindata;
		rsp["size"] = size;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "��ַ��" << it->second.endpoint.address().to_string() << "�˿ڣ�" << it->second.endpoint.port() << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::accsend(int& touid, int& fromuid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_ACCEPT_SHIPIN_REQ;
		rsp["fromuid"] = fromuid;
		rsp["touid"] = touid;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::guaduan(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_YYGUADUAN;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
	}
}

void UDPmgr::guaduansp(int& touid)
{
	auto it = clients_.find(touid);
	if (it != clients_.end())
	{
		auto re_end = it->second.endpoint;
		Json::Value rsp;
		rsp["id"] = ID_SPGUADUAN;
		auto buffer_ptr = std::make_shared<std::string>(rsp.toStyledString()); // ����ָ�������������
		std::cout << "Sending response: " << buffer_ptr << std::endl;
		_socket.async_send_to(boost::asio::buffer(*buffer_ptr), re_end, [buffer_ptr](const boost::system::error_code& error,
			std::size_t bytes_transferred) {
				if (!error) {
					std::cout << "Response sent successfully, bytes transferred: " << bytes_transferred << std::endl;
				}
				else {
					std::cerr << "Error sending response: " << error.message() << std::endl;
				}
			});
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
		std::cout << "���¿ͻ�����Ϣ: " << uid
			<< " ��ַ: " << endpoint.address().to_string()
			<< " �˿�: " << endpoint.port() << std::endl;

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
		std::cout << "��������" << std::endl;
		it->second.lastHeartbeat = std::chrono::steady_clock::now();
	}
	else {
		std::cerr << "�յ�δ֪�ͻ��˵�������Ϣ: " << uid << std::endl;
	}
}
