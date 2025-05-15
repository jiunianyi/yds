#include "Httpconnection.h"
#include"const.h"
#include"Logicsystem.h" 


Httpconnection::Httpconnection(boost::asio::io_context& ioc):_socket(ioc)
{

}
void Httpconnection::start()
{
	auto self = shared_from_this();
	boost::beast::http::async_read(_socket, _buffer, _request, [self](boost::beast::error_code ec, std::size_t bytes_transferred) {
		try {
			if (ec) {
				std::cout << "http read error :" << ec.what() << std::endl;
				return;
			}
			boost::ignore_unused(bytes_transferred);
			self->Handlereq();
			self->checkdeadline();
		}
		catch (std::exception &er)
		{
			std::cout << "error is: " << er.what() << std::endl;
		}
		});
}
unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}
unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//�ж��Ƿ�������ֺ���ĸ����
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ') //Ϊ���ַ�
			strTemp += "+";
		else
		{
			//�����ַ���Ҫ��ǰ��%���Ҹ���λ�͵���λ�ֱ�תΪ16����
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//��ԭ+Ϊ��
		if (str[i] == '+') strTemp += ' ';
		//����%������������ַ���16����תΪchar��ƴ��
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}
void Httpconnection::PreParseGetParam() {
	// ��ȡ URI  
	auto uri = _request.target();
	// ���Ҳ�ѯ�ַ����Ŀ�ʼλ�ã��� '?' ��λ�ã�  
	auto query_pos = uri.find('?');
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}
	_get_url = uri.substr(0, query_pos);
	std::string query_string = uri.substr(query_pos + 1);
	std::string key;
	std::string value;
	size_t pos = 0;
	while ((pos = query_string.find('&')) != std::string::npos) {
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); // ������ url_decode ����������URL����  
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		query_string.erase(0, pos + 1);
	}
	// �������һ�������ԣ����û�� & �ָ�����  
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}
void Httpconnection::Handlereq()
{
	//���ð汾
	_respone.version(_request.version());
	_respone.keep_alive(false);
	if (_request.method() == boost::beast::http::verb::get)
	{
		PreParseGetParam();
	//	std::cout << "666";
		bool success = Logicsystem::GetInstance()->HandleGet(_get_url, shared_from_this());
		if (!success) {
			_respone.result(boost::beast::http::status::not_found);
			_respone.set(boost::beast::http::field::content_type, "text/plain");
			boost::beast::ostream(_respone.body()) << "url found\r\n";
			Writeresponse();
			return;
		}
		_respone.result(boost::beast::http::status::ok);
		_respone.set(boost::beast::http::field::server, "Gateserver");
		Writeresponse();
		return;
	}

	if (_request.method() == boost::beast::http::verb::post)
	{
		bool success = Logicsystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
		if (!success) {
			_respone.result(boost::beast::http::status::not_found);
			_respone.set(boost::beast::http::field::content_type, "text/plain");
			boost::beast::ostream(_respone.body()) << "url found\r\n";
			Writeresponse();
			return;
		}
		
		_respone.result(boost::beast::http::status::ok);
		_respone.set(boost::beast::http::field::server, "Gateserver");
		Writeresponse();
		return;
	}
}
void Httpconnection::Writeresponse()
{
	auto self = shared_from_this();
	_respone.content_length(_respone.body().size());
	boost::beast::http::async_write(_socket, _respone, [self](boost::beast::error_code ec, 
		std::size_t bytes_transferred)
		{
			self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
			self->deadline_.cancel();
		});
}
void Httpconnection::checkdeadline()
{
	auto self = shared_from_this();
	deadline_.async_wait([self](boost::beast::error_code ec) {
		if (!ec)
		{
			self->_socket.close(ec);
		}
		});
}