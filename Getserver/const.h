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
		Error_json = 1001,//json解析失败
		RPCFailed = 1002,//rpc请求我出错
		VarifyExpired = 1003,//验证码过期
		VarifyCodeErr = 1004,//验证码错误
		UserExist = 1005,//用户已经催在
		PasswdErr = 1006,//密码错误
		EmailNotMatch = 1007,//邮箱不匹配
		PasswdUpFailed = 1008,//更新密码失败
		PasswdInvalid = 1009,//密码更新失败
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