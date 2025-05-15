#pragma once
#include"message.grpc.pb.h"
#include"message.pb.h"
#include"data.h"
#include<grpcpp/grpcpp.h>
#include"UserMgr.h"
#include"const.h"
#include"CSession.h"
#include"RedisMgr.h"
#include"MysqlMgr.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::AddFriendReq;
using message::AddFriendRsp;
using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatMsgData;

class ChatServicelmpl final : public ChatService::Service
{
public:

	ChatServicelmpl();
	Status NotifyAddFriend(ServerContext* context, const AddFriendReq* request,
		AddFriendRsp* reply) override;

Status NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request,
	AuthFriendRsp* response)override;

	Status NotifyTextChatMsg(::grpc::ServerContext* context,
		const TextChatMsgReq* request, TextChatMsgRsp* response) override;

	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
};

