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
		TokenInvalid = 1011,//token
		UidInvalid = 1012,//uid
};
enum MSG_IDS {
	MSG_CHAT_LOGIN  = 1005, //�û���½
	MSG_CHAT_LOGIN_RSP = 1006, //�û���½�ذ�
	ID_SEARCH_USER_REQ = 1007, //�û���������
	ID_SEARCH_USER_RSP = 1008, //�����û��ذ�
	ID_ADD_FRIEND_REQ = 1009, //������Ӻ�������
	ID_ADD_FRIEND_RSP = 1010, //������Ӻ��ѻظ�uu
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //֪ͨ�û���Ӻ�������
	ID_AUTH_FRIEND_REQ = 1013,  //��֤��������
	ID_AUTH_FRIEND_RSP = 1014,  //��֤���ѻظ�
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //֪ͨ�û���֤��������
	ID_TEXT_CHAT_MSG_REQ = 1017, //�ı�������Ϣ����
	ID_TEXT_CHAT_MSG_RSP = 1018, //�ı�������Ϣ�ظ�
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
	ID_SELECT_YUYIN_REQ = 1020,//��������ͨ������
	ID_ACCEPT_YUNYI_REQ = 1021,//��������ͨ������
	ID_REFUSE_YUYIN_REQ = 1022,//�ܾ�����ͨ������
	ID_SELECT_SHIPIN_REQ = 1023,//ѡ����Ƶͨ������
	ID_ACCEPT_SHIPIN_REQ = 1024,//������Ƶͨ������
	ID_REFUSE_SHIPIN_REQ = 1025,//�ܾ���Ƶͨ������
	ID_REGUSER_UDP = 1026,//ע��udp�û�
	ID_HEARD = 1027,//����
	ID_YUYIN = 1028,//����
	ID_CHANG_HEAD = 1029,//����ͷ��
	ID_SHIPIN = 1030,//��Ƶ
	ID_YYGUADUAN = 1031,//�Ҷ�����
	ID_SPGUADUAN = 1032//��Ƶ�Ҷ�
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
//ͷ���ܳ���
#define HEAD_TOTAL_LEN 6
//ͷ��id
#define HEAD_ID_LEN 2
//ͷ�����ݳ���
#define HEAD_DATA_LEN 4
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 10000

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO "nameinfo_"
