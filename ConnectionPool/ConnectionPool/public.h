#pragma once
#include <iostream>
//打印日志 哪个文件 哪行 什么时间 发生什么
#define LOG(str)\
	std::cout<<__FILE__<<":"<<__LINE__<<\
	"["<<__TIMESTAMP__<<"]"<<str<<std::endl;
