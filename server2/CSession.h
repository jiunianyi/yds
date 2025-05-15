#pragma once
#include"Logicsystem.h"
#include<iostream>
#include <boost/uuid/uuid_generators.hpp>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include<sstream>
#include<boost/uuid/uuid.hpp>
#include<boost/uuid/uuid_io.hpp>
#include"MsgNode.h"
#include<queue>
class Cserver;
class Logicsystem;

class CSession:public std::enable_shared_from_this<CSession>
{
public:
	CSession(boost::asio::io_context& io_context, Cserver* server);
	~CSession();
	void Start();
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);
	boost::asio::ip::tcp::socket& Getsocket();
	std::string &GetSessionId();
	void SetUserId(int uid);
	int GetUserId();//获取用户id
	//读取头部数据
	void AsyncReadHead(int total_len);
	//读取完整数据
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	//读取指定长度
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,std::function<void(const boost::system::error_code&, std::size_t)> handler);
	//读取并发送到逻辑队列
	void AsyncReadBody(int length);
	void Close();
	std::shared_ptr<CSession> SharedSelf();
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	bool _b_close;
	std::queue<std::shared_ptr<SendNode> > _send_que;
	std::mutex _send_lock;
	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;
	//是不是解析了头部
	bool _b_head_parse;
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;
	boost::asio::ip::tcp::socket _socket;
	Cserver* _server;
	std::string _session_id;
	char _data[MAX_LENGTH];
	int _user_uid;
};
class LogicNode {
	friend class Logicsystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};

