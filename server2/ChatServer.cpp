#include<mutex>
#include<thread>
#include"AsioServerPool.h"
#include"ConfigMgr.h"
#include"Cserver.h"
#include<csignal>
#include"RedisMgr.h"
#include"ChatServicelmpl.h"
#include"UDPmgr.h"

int main()
{
	Json::Reader reader;
	Json::Value root;
	auto& cfg = ConfigMgr::Info();
	auto server_name = cfg["SelfServer"]["Name"];
	try {
		auto pool = AsioIOServicePool::GetInstance();

		RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");

		std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
		ChatServicelmpl service;
		grpc::ServerBuilder builder;

		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		std::cout << "RPC Server listening on " << server_address << std::endl;

		std::thread  grpc_server_thread([&server]() {
			server->Wait();
			});

		boost::asio::io_context io_context, io_contextttt;
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&io_context, pool,&server](auto,auto) {
			io_context.stop();
			pool->Stop();
			server->Shutdown();
			});
		auto port_str = cfg["SelfServer"]["Port"];
		Cserver s(io_context, atoi(port_str.c_str()));
		//UDPmgr u(io_context, atoi(port_str.c_str()));
	//	std::shared_ptr<UDPmgr> u = std::make_shared<UDPmgr>(io_context,atoi(port_str.c_str()));
		//u->do_receive();
		io_context.run();
		std::cout << "退出" << std::endl;
		RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
		RedisMgr::GetInstance()->Close();
		grpc_server_thread.join();
	}
	catch (std::exception &ec)
	{
		std::cout << "exception error is : " << ec.what();
	}
}