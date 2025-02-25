#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "Connection.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <atomic>
//�����ӳ���Ƴɵ���ģʽ
class ConnectionPool {
private:
	//��ʼ���̳߳�
	ConnectionPool();
	//���������ļ���ʼ�����ӳ�
	bool loadConfigFile();
public:
	//�ͷ����ӳ���Դ
	~ConnectionPool();
	//���ⲿ�ṩ��ȡ�̳߳صĽӿ�
	static ConnectionPool* getConnectionPool();

	//Ϊ�ⲿ�����߳��ṩ�������Ӻ����ӿ�
	std::shared_ptr<Connection> consumeFunc();
	//Ϊ�����߳��ṩ�������Ӻ���
	void produceFunc();
	//Ϊ�����߳��ṩ���������Ӻ���
	void destroyFunc();

	ConnectionPool(const Connection&)= delete;
	ConnectionPool& operator=(const Connection&)= delete;
private:
	std::string ip_;//MySQL Server��ip��ַ
	unsigned short port_;//MySQL Server���ŵĶ˿ں�
	std::string userName_;//�û���   �û����������ݿ�����������֤
	std::string passWord_;//�û���Ӧ������
	std::string dbName_;//���ʵ����ݿ�

	int initConnSize_;//��ʼ������
	int maxConnSize_;//���������
	int maxIdleTime_;//���ӵ�������ʱ��
	int connTimeOut_;//�������ӵĳ�ʱʱ��
	std::atomic_int curConnSize_;//��ǰ���ӵ�����

	std::condition_variable empty_;//�ȴ������������������������ʱʹ�õ���������
	std::condition_variable notEmpty_;//�ȴ��������Ӷ��в�Ϊ��ʱʹ�õ���������
	std::mutex mtx_;//��֤�̻߳���ʹ�õĻ�����
	std::queue<Connection*> idleConnectionQue_;//�������Ӷ���
};

#endif