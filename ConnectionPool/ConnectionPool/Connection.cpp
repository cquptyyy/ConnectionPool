#include "Connection.h"
//���캯����ʼ��mysql����
std::mutex mtx;
Connection::Connection()
{
	std::lock_guard<std::mutex> lock(mtx);
	conn_ = mysql_init(nullptr);
	if (conn_ == nullptr) {
		std::cout << "conn_ == nullptr" << std::endl;
	}
}
//����������mysql���ӹر�
Connection::~Connection()
{
	if (conn_ != nullptr)
		mysql_close(conn_);
}
//����MySQL Server��ip��ַ�����Ŷ˿ڣ��û������û�������,�����ʵ����ݿ⣬��MySQL Server��������
bool Connection::connect(std::string ip, unsigned short port, std::string user, std::string password,std::string dbname)
{
	MYSQL* p = mysql_real_connect(conn_, ip.c_str(), user.c_str(),password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
//�����SQL�������ݿ���и���
bool Connection::update(std::string sql)
{
	if (mysql_query(conn_, sql.c_str()))
	{
		std::cout << "conn_=" <<conn_<< std::endl;
		std::cout <<"---------------------" << mysql_error(conn_) << std::endl;
		LOG("����ʧ��:" + sql);
		return false;
	}
	return true;
}
//�����ѯ��SQL����ȡ��ѯ������
MYSQL_RES* Connection::query(std::string sql)
{
	if (mysql_query(conn_, sql.c_str()))
	{
		LOG("��ѯʧ��:" + sql);
		return nullptr;
	}
	return mysql_use_result(conn_);
}
//ˢ�����ӵĿ��п�ʼʱ��
void Connection::refreshIdleStartTime() {
	idleStartTime_ = clock();
}
//��ȡ���ӽ������״̬�Ŀ�ʼʱ��
clock_t Connection::getIdleTime() { return clock()-idleStartTime_; }