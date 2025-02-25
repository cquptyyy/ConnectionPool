#include "ConnectionPool.h"
//加载配置文件
bool ConnectionPool::loadConfigFile() {
	
	FILE* fp = fopen("mysql.ini", "r");
	if (fp==nullptr) {
		LOG("mysql.init open fail");
		return false;
	}
	while (!feof(fp)) {
		char buffer[1024];
		fgets(buffer, sizeof(buffer), fp);
		std::string s = buffer;
		int startIndex = s.find("=");
		if (startIndex == std::string::npos)continue;
		int endIndex = s.find("\n");
		if (endIndex == std::string::npos)continue;
		std::string key = s.substr(0, startIndex);
		std::string value = s.substr(startIndex + 1, endIndex - 1 - startIndex);
		if (key == "ip")ip_ = value;
		else if (key == "port")port_ = std::stoi(value);
		else if (key == "userName")userName_ = value;
		else if (key == "passWord")passWord_ = value;
		else if (key == "dbName")dbName_ = value;
		else if (key == "initConnSize")initConnSize_ = stoi(value);
		else if (key == "maxConnSize")maxConnSize_ = stoi(value);
		else if (key == "maxIdleTime")maxIdleTime_ = stoi(value);
		else if (key == "connTimeOut")connTimeOut_ = stoi(value);
	}

	return true;
}

//提供外部获取连接池的接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;
	return &pool;
}

//初始化连接池
ConnectionPool::ConnectionPool()
	:curConnSize_(0) {
	//加载配置文件，初始连接池的属性
	if (loadConfigFile()==false) {
		LOG("loadConfigFile fail");
		exit(1);
	}
	for (int i = 0; i < initConnSize_; ++i) {
		Connection* p = new Connection();
		p->connect(ip_, port_, userName_, passWord_, dbName_);
		p->refreshIdleStartTime();
		idleConnectionQue_.push(p);
		curConnSize_++;
	}
	std::thread produceThread(std::bind(&ConnectionPool::produceFunc,this));
	std::thread destroyThread(std::bind(&ConnectionPool::destroyFunc, this));
	produceThread.detach();
	destroyThread.detach();
}

//生产连接函数
void ConnectionPool::produceFunc() {
	while (true) {
		std::unique_lock<std::mutex> lock(mtx_);
		//当空闲连接队列中没有连接且连接的数量小于最大连接数量，才去生产连接
		while (!idleConnectionQue_.empty()||curConnSize_ == maxConnSize_) {
			empty_.wait(lock);
		}
		Connection* p = new Connection;
		p->connect(ip_, port_, userName_, passWord_, dbName_);
		p->refreshIdleStartTime();
		idleConnectionQue_.push(p);
		curConnSize_++;
		notEmpty_.notify_all();
	}
}

//为外部提供消费连接函数接口
std::shared_ptr<Connection> ConnectionPool::consumeFunc() {
	std::unique_lock<std::mutex> lock(mtx_);
	while (idleConnectionQue_.empty()) {
		if (std::cv_status::timeout == notEmpty_.wait_for(lock, std::chrono::milliseconds(connTimeOut_))) {
			if (idleConnectionQue_.empty()) {
				LOG("get a connection time out!!!");
				return nullptr;
			}
		}
	}
	Connection* pconn=idleConnectionQue_.front();
	idleConnectionQue_.pop();
	std::shared_ptr<Connection> sp(pconn,
		[this](Connection* p)->void {
			std::unique_lock<std::mutex> lock(mtx_);
			idleConnectionQue_.push(p);
			notEmpty_.notify_all();
		});
	empty_.notify_all();
	return sp;
}

//销毁连接函数
void ConnectionPool::destroyFunc() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(maxIdleTime_));
		std::unique_lock<std::mutex> lock(mtx_);
		//当连接数量大于初始连接量且空闲连接队列不为空时，查看连接是否超时
		while (curConnSize_ > initConnSize_&&!idleConnectionQue_.empty()) {
			Connection* pconn = idleConnectionQue_.front();
			clock_t startTime=pconn->getIdleTime();
			clock_t now = clock();
			if (pconn->getIdleTime() > maxIdleTime_ * 1000) {
		
				idleConnectionQue_.pop();
				delete pconn;
				curConnSize_--;
			}
			else break;
		}
	}
}

//释放连接池资源
ConnectionPool::~ConnectionPool() {}
