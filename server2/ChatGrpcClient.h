#pragma once
#include"const.h"
#include"Singleton.h"
#include"ConfigMgr.h"
#include"message.grpc.pb.h"
#include"message.pb.h"
#include<queue>
#include"data.h"
#include<grpcpp/grpcpp.h>
#include<atomic>
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatMsgData;

class ChatConPool {
public:
	ChatConPool(size_t poolsize, std::string host, std::string port) :host_(host), port_(port), poolsize_(poolsize)
	{
		for (int i = 0; i < poolsize; i++)
		{
			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
			connections_.push(ChatService::NewStub(channel));
			std::cout << "创建通道完成" << std::endl;
			std::cout << channel << std::endl;
		}
		
	}
	~ChatConPool()
	{
		std::lock_guard<std::mutex>lock(mutex_);
		Close();
		while (!connections_.empty())
		{
			connections_.pop();
		}
	}
	std::unique_ptr<ChatService::Stub>getconnection()
	{
		std::unique_lock<std::mutex>lock(mutex_);
		cond_.wait(lock, [this](){
			if (b_stop_)
			{
				return true;
			}
			return !connections_.empty();
		});
		//如果停止则直接返回空指针
		if (b_stop_) {
			return  nullptr;
		}
		auto context = std::move(connections_.front());
		connections_.pop();
		return context;
	}
	void returnconnection(std::unique_ptr<ChatService::Stub> context)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(std::move(context));
		cond_.notify_one();
	}

	void Close()
	{
		b_stop_ = true;
		cond_.notify_all();
	}
private:
	std::atomic<bool> b_stop_;
	std::string host_;
	std::string port_;
	size_t poolsize_;
	std::queue<std::unique_ptr<ChatService::Stub> > connections_;//用于存储往管道传信息的快递员
	std::mutex mutex_;
	std::condition_variable cond_;
};
class ChatGrpcClient: public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
public:
	~ChatGrpcClient()
	{

	}
	AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
	AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);
private:
	ChatGrpcClient();
   std::unordered_map<std::string, std::unique_ptr<ChatConPool>>_pools;
};

