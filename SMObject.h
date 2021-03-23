#pragma once
#include "VMObject.h"
#include <algorithm>
#include "ServerMachine.h"
#include <unordered_map>

class VMObject;
class ServerMachine;

//服务器当前某节点状态
struct ServerState
{
	int core;
	int memory;
	ServerState() : core(0), memory(0) {}
	ServerState(int _core, int _memory) : core(_core), memory(_memory) {}
};

/// <summary>
/// 用法：
/// 1.构造函数声明服务器属性类型和服务器号
/// 2.使用AddChild添加虚拟机
/// 3.使用RemoveChild移除虚拟机（会导致虚拟机的father为空）
/// 4.使用IsRunning判断服务器此时是否需要支付每日费用
/// </summary>
class SMObject
{
private:
	const ServerMachine SMProperty;
	ServerState nodeA;
	ServerState nodeB;
	int SM_Id;
	int true_id;
	unordered_map<int, VMObject *> childs;

public:
	friend class VMObject;

	SMObject(const ServerMachine& _SMProperty,int _sm_id);

	//并不需要析构函数，因为服务器不会被销毁
	//~SMObject() {}

	//检查此服务器当前是否开着机（没有虚拟机的时候是不需要每日费用的）
	bool IsRunning() const;

	int GetDailyCost() const; //获取每日消耗费用

	//添加虚拟机并设置虚拟机的father为自己，如果虚拟机有父亲则其原父亲会删掉他
	bool AddChild(VMObject* child);
	//判断能否把目标虚拟机设置为自己的child
	bool AbleToAddChild(VMObject* child);
	
	//删除虚拟机（会导致虚拟机的father为空）
	bool RemoveChild(VMObject* child);
	
	//删除虚拟机（会导致虚拟机的father为空）
	bool RemoveChild(int vm_id);

	string Gettype() { return SMProperty.GetModelType(); }

	void SetId(int id) { this->SM_Id = id; }
	void SetTrueId(int id){ this->true_id = id; }
	int GetTrueId () { return true_id; }
	int GetId() const { return SM_Id; }
	int GetBMemo() const { return nodeB.memory; }
	int GetAMemo() const { return nodeA.memory; }
	int GetBCore() const { return nodeB.core; }
	int GetACore() const { return nodeA.core; }
	const ServerMachine& GetProperty() const { return SMProperty; }

	bool operator< (const SMObject& b)
	{
		MachineType type = SMProperty.GetMechType();
		if (type == MachineType::HighCore)
		{
			if (nodeA.core == b.nodeA.core) return nodeB.core < b.nodeB.core;
			return nodeA.core < b.nodeA.core;
		}
		else if (type == MachineType::HighMemory)
		{
			if (nodeA.memory == b.nodeA.memory) return nodeB.memory < b.nodeB.memory;
			return nodeA.memory < b.nodeA.memory;
		}
		else
		{
			int max1 = min(nodeA.core, nodeA.memory);
			int max2 = min(b.nodeA.core, b.nodeA.memory);
			if (max1 == max2)
			{
				return min(nodeB.core, nodeB.memory) < min(b.nodeB.core, b.nodeB.memory);
			}
			return max1 < max2;

		}
	}

	bool operator>(const SMObject& b)
	{
		MachineType type = SMProperty.GetMechType();
		if (type == MachineType::HighCore)
		{
			if (nodeA.core == b.nodeA.core) return nodeB.core > b.nodeB.core;
			return nodeA.core > b.nodeA.core;
		}
		else if (type == MachineType::HighMemory)
		{
			if (nodeA.memory == b.nodeA.memory) return nodeB.memory > b.nodeB.memory;
			return nodeA.memory > b.nodeA.memory;
		}
		else
		{
			int max1 = min(nodeA.core, nodeA.memory);
			int max2 = min(b.nodeA.core, b.nodeA.memory);
			if (max1 == max2)
			{
				return min(nodeB.core, nodeB.memory) > min(b.nodeB.core, b.nodeB.memory);
			}
			return max1 > max2;

		}
	}

	bool operator<=(const SMObject& b)
	{
		return !(*this > b);
	}

	bool operator>=(const SMObject& b)
	{
		return !(*this < b);
	}

	bool operator==(const SMObject& b)
	{
		return nodeA.core == b.nodeA.core && nodeB.core == b.nodeB.core;
	}

	bool operator!=(const SMObject& b)
	{
		return !(*this == b);
	}
};