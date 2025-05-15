#pragma once
#include <string>
#include "const.h"
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;
class Logicsystem;
class MsgNode
{
public:
	MsgNode(uint32_t max_len) :_total_len(max_len), _cur_len(0) {
		_data = new char[_total_len + 1]();
		_data[_total_len] = '\0';
	}

	~MsgNode() {
		std::cout << "destruct MsgNode" << endl;
		delete[] _data;
	}

	void Clear() {
		::memset(_data, 0, _total_len);
		_cur_len = 0;
	}

	uint32_t _cur_len;
	uint32_t _total_len;
	char* _data;
};

class RecvNode :public MsgNode {
	friend class Logicsystem;
public:
	RecvNode(uint32_t max_len, short msg_id);
private:
	short _msg_id;
};

class SendNode :public MsgNode {
	friend class Logicsystem;
public:
	SendNode(const char* msg, uint32_t max_len, short msg_id);
private:
	short _msg_id;
};

