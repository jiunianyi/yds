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
		TokenInvalid = 1011,//token
		UidInvalid = 1012,//uid
};
enum MSG_IDS {
	MSG_CHAT_LOGIN  = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复uu
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
	ID_SELECT_YUYIN_REQ = 1020,//进行语音通话请求
	ID_ACCEPT_YUNYI_REQ = 1021,//接受语音通话请求
	ID_REFUSE_YUYIN_REQ = 1022,//拒绝语音通话请求
	ID_SELECT_SHIPIN_REQ = 1023,//选择视频通话请求
	ID_ACCEPT_SHIPIN_REQ = 1024,//接受视频通话请求
	ID_REFUSE_SHIPIN_REQ = 1025,//拒绝视频通话请求
	ID_REGUSER_UDP = 1026,//注册udp用户
	ID_HEARD = 1027//心跳
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
#define MAX_LENGTH 1024 * 1024 * 1024
//头部总长度
#define HEAD_TOTAL_LEN 6
//头部id
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 4
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 10000

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO "nameinfo_"
