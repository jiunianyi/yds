#include "VerifyGrpcClient.h"
#include"ConfigMgr.h"
#include"const.h"
VerifyGrpcClient::VerifyGrpcClient()
{
	auto gCfgMgr = ConfigMgr::Info();
	std::string host=gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	pool_.reset(new RPComPool(5, host, port)); 
}
RPComPool::RPComPool(std::size_t poolsize, std::string host, std::string port) :poolsize_(poolsize),
host_(host), port_(port), b_stop_(false)
{
	for (size_t i = 0; i < poolsize_; i++)
	{
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host+":"+port,
			grpc::InsecureChannelCredentials());
		connections_ .push(varifyService::NewStub(channel));
	}
}
RPComPool::~RPComPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	close();
	while (!connections_.empty())
	{
		connections_.pop();
	}
}
 void RPComPool::close()
{
	 b_stop_ = true;
	 cond_.notify_all();
 }
 std::unique_ptr<varifyService::Stub> RPComPool::getconnection()
 {
	 std::unique_lock<std::mutex> lock(mutex_);
	 cond_.wait(lock, [this] {
		 if (b_stop_) {
			 return true;  
		 }
		 return !connections_.empty();
		 });
	 if (b_stop_)
	 {
		 return nullptr;
	 }
	 auto context = std::move(connections_.front());
	 connections_.pop();
	 return context;
 }
 void RPComPool::returnconnection(std::unique_ptr<varifyService::Stub> context)
 {
	 std::lock_guard<std::mutex>lock(mutex_);
	 if (b_stop_)
	 {
		 return;
	 }
	 connections_.push(std::move(context));
	 cond_.notify_one();
 }

