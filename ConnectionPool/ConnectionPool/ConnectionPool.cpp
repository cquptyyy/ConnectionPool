#include "ConnectionPool.h"
//���������ļ�
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

//�ṩ�ⲿ��ȡ���ӳصĽӿ�
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;
	return &pool;
}

//��ʼ�����ӳ�
ConnectionPool::ConnectionPool()
	:curConnSize_(0) {
	//���������ļ�����ʼ���ӳص�����
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

//�������Ӻ���
void ConnectionPool::produceFunc() {
	while (true) {
		std::unique_lock<std::mutex> lock(mtx_);
		//���������Ӷ�����û�����������ӵ�����С�����������������ȥ��������
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

//Ϊ�ⲿ�ṩ�������Ӻ����ӿ�
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

//�������Ӻ���
void ConnectionPool::destroyFunc() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(maxIdleTime_));
		std::unique_lock<std::mutex> lock(mtx_);
		//�������������ڳ�ʼ�������ҿ������Ӷ��в�Ϊ��ʱ���鿴�����Ƿ�ʱ
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

//�ͷ����ӳ���Դ
ConnectionPool::~ConnectionPool() {}
