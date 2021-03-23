#pragma once

#include <string>

using namespace std;

//服务器分类标准
constexpr auto SMstandrad = 2;

enum class MachineType
{
	HighCore,
	Balance,
	HighMemory
};

//服务器
class ServerMachine
{
public:
	//构造函数（型号，核心数，内存大小，硬件成本，每日能耗成本）
	ServerMachine(string _modelType, int _core, int _memoryCapacity, int _hardwareCost, int _dailyCost);
	ServerMachine(){};

	string GetModelType() const { return modelType; }
	int GetCore() const { return core; }
	int GetMemoryCapacity() const { return memoryCapacity; }
	int GetHardwareCost() const { return hardwareCost; }
	int GetDailyCost() const { return dailyCost; }
	int GetPurchased_id() const { return purchased_id; }
	void SetPurchased_id(int id){purchased_id=id;}
	MachineType GetMechType() const { return machineType; }
	bool operator <(const ServerMachine &a)
	{
		return this->modelType<a.modelType;
	}

private:
	string modelType;	//型号
	int core;			//核心数
	int memoryCapacity; //内存大小

	int hardwareCost; //硬件成本
	int dailyCost;	  //每日能耗成本

	int purchased_id; //连接currentSM和purchased_Machines的id

	MachineType machineType; //机器属性
};
