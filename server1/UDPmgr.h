#pragma once
#include<boost/asio.hpp>
#include <unordered_map>
#include<mutex>

// 客户端信息结构体
struct ClientInfo {
	int uid;
	boost::asio::ip::udp::endpoint endpoint;
	std::chrono::steady_clock::time_point lastHeartbeat;
};
class UDPmgr:public std::enable_shared_from_this<UDPmgr>
{
public:
	UDPmgr(boost::asio::io_context&io_context,short port);
	void regnewuser(int &uid, boost::asio::ip::udp::endpoint& endpoint);//注册udp新用户
	void heart_check(int &uid);//心跳检测
	void do_receive();
	void send_dhtz(int &touid,int& fromuid);
	void send_reftz(int& touid);
	void jing_th(int& touid);
	void acc_th(int& uid);
	void seleship(int& uid,int &fromuid);
	void sendsosj(int& touid, int& fromuid);
	void accsend(int& touid, int& fromuid);
	void guaduan(int& touid);
	void guaduansp(int& touid);
private:
	std::string encodedData;
	std::string shipindata;
	int size;
	boost::asio::ip::udp::socket _socket;
	boost::asio::ip::udp::endpoint sender_endpoint_;
	std::array<char, 1024> recv_buffer_;
	std::unordered_map<int, ClientInfo> clients_;
};

