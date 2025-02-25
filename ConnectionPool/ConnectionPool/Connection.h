#ifndef CONNECTION_H
#define CONNECTION_H

//�۹�����������������������ͷԽ
#include <iostream>
#include <mysql.h>
#include <string>
#include <mutex>
#include "public.h"
class Connection {
public:
	//��ʼ������
	Connection();
	//�ر�����
	~Connection();
	//����MySQL Server��ip��ַ���˿ڣ��û��������룬���ݿ����ƣ������������ݿ������
	bool connect(std::string ip, unsigned short port, std::string user, std::string password, std::string dbname);
	//����sql�������ݿ�
	bool update(std::string sql);
	//����sql��ѯ���ݿ�
	MYSQL_RES* query(std::string sql);
	//�������ӽ������״̬�Ŀ�ʼʱ��
	void refreshIdleStartTime();
	//��ȡ���ӽ������״̬�Ŀ�ʼʱ��
	clock_t getIdleTime();
private:
	MYSQL* conn_;//��ʶһ������
	clock_t idleStartTime_;//���ӿ��п�ʼʱ��
 };

#endif