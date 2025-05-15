#include "Cserver.h"
#include"Httpconnection.h" 
#include"AsioServerPool.h"
Cserver::Cserver(boost::asio::io_context& ioc, unsigned short& port):_ioc(ioc),
_acceptor(ioc,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port),port)
{

}
void Cserver::start()
{
	auto self = shared_from_this();
	//
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<Httpconnection> new_con = std::make_shared<Httpconnection>(io_context);
	_acceptor.async_accept(new_con->Getsocket(), [self,new_con](boost::beast::error_code ec) {
		try {
			//����������ӣ����¼���
			if (ec)
			{
				self->start();
				return;
			}
			//����������
			new_con->start();
			//��������
			self->start();
		}
		catch (std::exception& ex)
		{

		}
		});
}

