#ifndef CONNECTION_H
#define CONNECTION_H

//雄关漫道真如铁，而今迈步从头越
#include <iostream>
#include <mysql.h>
#include <string>
#include <mutex>
#include "public.h"
class Connection {
public:
	//初始化连接
	Connection();
	//关闭连接
	~Connection();
	//传入MySQL Server的ip地址，端口，用户名，密码，数据库名称，建立访问数据库的连接
	bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
	//传入sql更新数据库
	bool update(std::string sql);
	//传入sql查询数据库
	MYSQL_RES* query(std::string sql);
	//更新连接进入空闲状态的开始时间
	void refreshIdleStartTime();
	//获取连接进入空闲状态的开始时间
	clock_t getIdleTime();
private:
	MYSQL* conn_;//标识一个连接
	clock_t idleStartTime_;//连接空闲开始时间
 };

#endif