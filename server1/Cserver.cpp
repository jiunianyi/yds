#include"Cserver.h"
#include"UserMgr.h"

Cserver::Cserver(boost::asio::io_context& io_context, short port) :_io_context(io_context), _port(port),
_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	std::cout << "server start success,listen on port: " << _port<<std::endl;
	Startaccept();
}
void Cserver::Startaccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->Getsocket(), std::bind(&Cserver::HandleAccept, this, new_session, std::placeholders::_1));
}
void Cserver::HandleAccept(std::shared_ptr<CSession>new_session, const boost::system::error_code& error)
{
	if (!error)
	{
		new_session->Start();
		std::lock_guard<std::mutex>lock(_mutex);
		_sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
	}
}
void Cserver::clearsession(std::string session_id)
{

	if (_sessions.find(session_id) != _sessions.end())
	{
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}
	std::lock_guard<std::mutex>lock(_mutex);
	_sessions.erase(session_id);
}
Cserver::~Cserver()
{
	std::cout << "server destruct listen on port: " << _port << std::endl;
}