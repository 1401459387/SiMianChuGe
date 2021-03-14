#pragma once

#include <string>

using namespace std;

//虚拟机
class VirtualMachine
{
public:
	//构造函数
	VirtualMachine(string _modelType,int _core,int _memoryCapacity,bool isTwoNode);
	VirtualMachine() {};

	string GetModelType() const { return modelType; }
	int GetCore() const { return core; }
	int GetMemoryCapacity() const { return memoryCapacity; }
	bool IsTwoNode() const { return isTwoNode; }

private:
	string modelType;		//型号
	int core;				//核心数
	int memoryCapacity;		//内存大小

	bool isTwoNode;			//是否双节点部署
};