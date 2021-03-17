#pragma once

#include <string>

using namespace std;

//虚拟机
class VirtualMachine
{
public:
	//构造函数
	VirtualMachine(string _modelType, int _core, int _memoryCapacity, bool isTwoNode);
	VirtualMachine(){};

	string GetModelType() const { return modelType; }
	int GetCore() const { return core; }
	int GetMemoryCapacity() const { return memoryCapacity; }
	bool IsTwoNode() const { return isTwoNode; }

	void SetRequest_Id(int id) { request_id = id; }

private:
	string modelType;	//型号
	int core;			//核心数
	int memoryCapacity; //内存大小

	bool isTwoNode; //是否双节点部署
	int request_id; //连接虚拟机请求和虚拟机实体的id
};