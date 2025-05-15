#pragma once
#include<boost/beast/http.hpp>
#include<boost/beast.hpp>
#include<boost/asio.hpp>
#include<memory>
#include<iostream>
#include"Singleton.h"
#include<functional>
#include<map>
#include<unordered_map>
#include<json/json.h>
#include<json/value.h>
#include<json/reader.h>
#include<boost/filesystem.hpp>
#include<boost/property_tree/ptree.hpp>
#include<boost/property_tree/ini_parser.hpp>
#include<atomic>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<hiredis.h>
#include<assert.h>



//81.68.86.146
enum ErrorCodes {
	success = 0,
		Error_json = 1001,//json����ʧ��
		RPCFailed = 1002,//rpc�����ҳ���
		VarifyExpired = 1003,//��֤�����
		VarifyCodeErr = 1004,//��֤�����
		UserExist = 1005,//�û��Ѿ�����
		PasswdErr = 1006,//�������
		EmailNotMatch = 1007,//���䲻ƥ��
		PasswdUpFailed = 1008,//��������ʧ��
		PasswdInvalid = 1009,//�������ʧ��
		RPCGetFailed=1010,
};

#define CODEPREFIX  "code_"
class Defer {
public:
	Defer(std::function<void()> func):func_(func){}
	~Defer()
	{
		func_();
	}
private:
	std::function<void()>func_;
};