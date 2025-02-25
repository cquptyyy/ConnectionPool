#include "ConnectionPool.h"
#include <vector>
using namespace std;
void consumeThreadFunc() {
	int n = 2000;
	ConnectionPool* pool = ConnectionPool::getConnectionPool();
	for (int i = 0; i < n; ++i) {
		char sql[1024];
		sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		shared_ptr<Connection> pconn = pool->consumeFunc();
		pconn->update(sql);
	}
}
void consumeFunc() {
	int n = 2000;
	for (int i = 0; i < n; ++i) {
		char sql[1024];
		sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		Connection* pconn = new Connection();
		pconn->connect("127.0.0.1", 3306, "root", "CQUPTyyy591420", "chat");
		pconn->update(sql);
		delete pconn;
	}
}
int main() {
	vector<thread> consumers;
	clock_t begin = clock();
	for (int i = 0; i < 1; ++i) {
		consumers.push_back(thread(consumeThreadFunc));
	}
	for (auto& t:consumers) {
		t.join();
	}
	clock_t end = clock();
	cout << "tasks consume time:" << end - begin<<"ms"<< endl;
	return 0;
}