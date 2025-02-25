#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "Connection.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <atomic>
//将连接池设计成单例模式
class ConnectionPool {
private:
	//初始化线程池
	ConnectionPool();
	//加载配置文件初始化连接池
	bool loadConfigFile();
public:
	//释放连接池资源
	~ConnectionPool();
	//给外部提供获取线程池的接口
	static ConnectionPool* getConnectionPool();

	//为外部消费线程提供消费连接函数接口
	std::shared_ptr<Connection> consumeFunc();
	//为生产线程提供生产连接函数
	void produceFunc();
	//为销毁线程提供的销毁连接函数
	void destroyFunc();

	ConnectionPool(const Connection&)= delete;
	ConnectionPool& operator=(const Connection&)= delete;
private:
	std::string ip_;//MySQL Server的ip地址
	unsigned short port_;//MySQL Server开放的端口号
	std::string userName_;//用户名   用户名密码数据库用于连接认证
	std::string passWord_;//用户对应的密码
	std::string dbName_;//访问的数据库

	int initConnSize_;//初始连接量
	int maxConnSize_;//最大连接量
	int maxIdleTime_;//连接的最大空闲时间
	int connTimeOut_;//申请连接的超时时间
	std::atomic_int curConnSize_;//当前连接的数量

	std::condition_variable empty_;//等待连接数量不等于最大连接量时使用的条件变量
	std::condition_variable notEmpty_;//等待空闲连接队列不为空时使用的条件变量
	std::mutex mtx_;//保证线程互斥使用的互斥锁
	std::queue<Connection*> idleConnectionQue_;//空闲连接队列
};

#endif