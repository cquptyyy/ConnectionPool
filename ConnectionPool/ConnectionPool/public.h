#pragma once
#include <iostream>
//��ӡ��־ �ĸ��ļ� ���� ʲôʱ�� ����ʲô
#define LOG(str)\
	std::cout<<__FILE__<<":"<<__LINE__<<\
	"["<<__TIMESTAMP__<<"]"<<str<<std::endl;
