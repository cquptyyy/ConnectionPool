#include "Connection.h"
//构造函数初始化mysql连接
std::mutex mtx;
Connection::Connection()
{
	std::lock_guard<std::mutex> lock(mtx);
	conn_ = mysql_init(nullptr);
	if (conn_ == nullptr) {
		std::cout << "conn_ == nullptr" << std::endl;
	}
}
//析构函数将mysql连接关闭
Connection::~Connection()
{
	if (conn_ != nullptr)
		mysql_close(conn_);
}
//传入MySQL Server的ip地址，开放端口，用户名，用户名密码,及访问的数据库，与MySQL Server建立连接
bool Connection::connect(std::string ip, unsigned short port, std::string user, std::string password,std::string dbname)
{
	MYSQL* p = mysql_real_connect(conn_, ip.c_str(), user.c_str(),password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
//传入的SQL语句对数据库进行更新
bool Connection::update(std::string sql)
{
	if (mysql_query(conn_, sql.c_str()))
	{
		std::cout << "conn_=" <<conn_<< std::endl;
		std::cout <<"---------------------" << mysql_error(conn_) << std::endl;
		LOG("更新失败:" + sql);
		return false;
	}
	return true;
}
//传入查询的SQL语句获取查询的数据
MYSQL_RES* Connection::query(std::string sql)
{
	if (mysql_query(conn_, sql.c_str()))
	{
		LOG("查询失败:" + sql);
		return nullptr;
	}
	return mysql_use_result(conn_);
}
//刷新连接的空闲开始时间
void Connection::refreshIdleStartTime() {
	idleStartTime_ = clock();
}
//获取连接进入空闲状态的开始时间
clock_t Connection::getIdleTime() { return clock()-idleStartTime_; }