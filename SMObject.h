#pragma once
#include "VMObject.h"
#include <unordered_map>

class VMObject;

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
	ServerMachine SMProperty;
	ServerState nodeA;
	ServerState nodeB;
	int Id;
	unordered_map<int, VMObject *> childs;

public:
	friend class VMObject;

	SMObject(ServerMachine _SMProperty) : SMProperty(_SMProperty)
	{
		int sumCore = SMProperty.GetCore();
		int sumMemory = SMProperty.GetMemoryCapacity();
		nodeA.core = sumCore >> 1;
		nodeA.memory = sumMemory >> 1;
		nodeB.core = nodeA.core;
		nodeB.memory = nodeA.memory;
	}

	//并不需要析构函数，因为服务器不会被销毁
	//~SMObject() {}

	//检查此服务器当前是否开着机（没有虚拟机的时候是不需要每日费用的）
	bool IsRunning() const
	{
		return childs.empty(); //如果是空的说明没有虚拟机在运行
	}

	int GetDailyCost() const //获取每日消耗费用
	{
		return SMProperty.GetDailyCost();
	}

	//添加虚拟机并设置虚拟机的father为自己，如果虚拟机有父亲则其原父亲会删掉他
	bool AddChild(VMObject *child)
	{
		if (child == nullptr)
			return false;//throw exception("尝试往服务器中添加空指针");
		VirtualMachine property = child->GetProperty();
		bool isTwoNode = property.IsTwoNode();
		int core = property.GetCore();
		int memory = property.GetMemoryCapacity();
		int vm_id = child->GetID();
		if (isTwoNode)
		{
			core >>= 1;
			memory >>= 1;
			if (nodeA.core < core || nodeA.memory < memory || nodeB.core < core || nodeB.memory < memory)
			{
				return false;
			}
			else
			{
				nodeA.core -= core;
				nodeB.core -= core;
				nodeA.memory -= memory;
				nodeB.memory -= memory;
				child->SetNodeType(Both);
				childs[vm_id] = child;
				if (child->father)
					child->father->RemoveChild(child);
				child->father = this;
			}
		}
		else
		{
			//TODO 决定放在哪个节点里
			//先优先放在A节点中
			if (nodeA.core < core || nodeA.memory < memory)
			{
				if (nodeB.core < core || nodeB.memory < memory)
				{
					return false; //throw exception("服务器塞不下此虚拟机"); //都塞不下
				}
				else
				{
					nodeB.core -= core;
					nodeB.memory -= memory;
					child->SetNodeType(B);
					childs[vm_id] = child;
					if (child->father)
						child->father->RemoveChild(child);
					child->father = this;
				}
			}
			else
			{
				nodeA.core -= core;
				nodeA.memory -= memory;
				child->SetNodeType(A);
				childs[vm_id] = child;
				if (child->father)
					child->father->RemoveChild(child);
				child->father = this;
			}
		}
		return true;
	}
	//删除虚拟机（会导致虚拟机的father为空）
	bool RemoveChild(VMObject *child)
	{
		int vm_id = child->GetID();
		if (childs.find(vm_id) == childs.end())
			return false; //throw exception("服务器中并没有此虚拟机");

		VM_NodeType nodeType = child->GetNodeType();
		VirtualMachine property = child->GetProperty();
		int core = property.GetCore();
		int memory = property.GetMemoryCapacity();
		switch (nodeType)
		{
		case A:
			nodeA.core += core;
			nodeA.memory += memory;
			break;
		case B:
			nodeB.core += core;
			nodeB.memory += memory;
			break;
		case Both:
			core >>= 1;
			memory >>= 1;
			nodeA.core += core;
			nodeA.memory += memory;
			nodeB.core += core;
			nodeB.memory += memory;
			break;
		default:
			return false; //throw exception("要删除的虚拟机的节点属性未知");
			break;
		}
		this->childs.erase(vm_id);
		return true;
	}
	//删除虚拟机（会导致虚拟机的father为空）
	bool RemoveChild(int vm_id)
	{
		if (this->childs.find(vm_id) == childs.end())
			return false; //throw exception("服务器中并没有此虚拟机");
		VMObject *child = this->childs[vm_id];

		return RemoveChild(child);
	}

	void SetId(int id) { this->Id = id; }
	int GetId() const { return Id; }
};