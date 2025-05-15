#include "RedisMgr.h"
#include"ConfigMgr.h"
RedisMgr::~RedisMgr()
{
    Close();
}
RedisMgr::RedisMgr()
{
    auto& gCfgMgr = ConfigMgr::Inst();  
    auto host = gCfgMgr["Redis"]["Host"];
    auto port = gCfgMgr["Redis"]["Port"];
    auto pwd = gCfgMgr["Redis"]["Passwd"];
    _con_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}
/*bool RedisMgr::Connect(const std::string& host, int port)
{

    auto connect = _con_pool->getConnection();
    if (connect == NULL)
    {
        return false;
    }
    connect = redisConnect(host.c_str(), port);

    if (connection != NULL &&connection->err)
    {
        std::cout << "connect error " <<connection->errstr << std::endl;
        return false;
    }
    return true;
}*/
bool RedisMgr::Get(const std::string& key, std::string& value)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "GET %s", key.c_str());
    if (reply == NULL) {
        std::cout << "[ GET  " << key << " ] failed" << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    if (reply->type != REDIS_REPLY_STRING) {
        std::cout << "[ GET  " << key << " ] failed" << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    value = reply->str;
    freeReplyObject(reply);
    std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::Set(const std::string& key, const std::string& value)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    //执行redis命令行
    auto reply = (redisReply*)redisCommand(connection, "SET %s %s", key.c_str(), value.c_str());
    //如果返回NULL则说明执行失败
    if (NULL == reply)
    {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    //如果执行失败则释放连接
    if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
    {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    freeReplyObject(reply);
    std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::Auth(const std::string& password)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "AUTH %s", password.c_str());
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "认证失败" << std::endl;
        //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    else {
        //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
        freeReplyObject(reply);
        std::cout << "认证成功" << std::endl;
        _con_pool->returnConnection(connection);
        return true;
    }
}
bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "LPUSH %s %s", key.c_str(), value.c_str());
    if (NULL == reply)
    {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::LPop(const std::string& key, std::string& value) {
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
   auto  reply = (redisReply*)redisCommand(connection, "LPOP %s ", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    value = reply->str;
    std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::RPush(const std::string& key, const std::string& value) {
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "RPUSH %s %s", key.c_str(), value.c_str());
    if (NULL == reply)
    {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::RPop(const std::string& key, std::string& value) {
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "RPOP %s ", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    value = reply->str;
    std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value) {
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
   auto reply = (redisReply*)redisCommand(connection, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto reply = (redisReply*)redisCommandArgv(connection, 4, argv, argvlen);
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return "";
    }
    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
   auto  reply = (redisReply*)redisCommandArgv(connection, 3, argv, argvlen);
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
        freeReplyObject(reply);
        std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
        _con_pool->returnConnection(connection);
        return "";
    }
    std::string value = reply->str;
    freeReplyObject(reply);
    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
    _con_pool->returnConnection(connection);
    return value;
}
bool RedisMgr::Del(const std::string& key)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
   auto reply = (redisReply*)redisCommand(connection, "DEL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
bool RedisMgr::HDel(const std::string& key, const std::string& hkey)
{
    return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
    auto connection = _con_pool->getConnection();
    if (connection == nullptr)
    {
        return false;
    }
    auto reply = (redisReply*)redisCommand(connection, "exists %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        freeReplyObject(reply);
        _con_pool->returnConnection(connection);
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    freeReplyObject(reply);
    _con_pool->returnConnection(connection);
    return true;
}
