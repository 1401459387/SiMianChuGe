#pragma once
#include <string>
#include "SMObject.h"
#include "ServerMachine.h"
#include "VirtualMachine.h"

class SMObject;

//虚拟机节点类型枚举
enum class VM_NodeType
{
	Unknow,
	A,
	B,
	Both
};

/// <summary>
/// 用法：
/// 1.声明虚拟机时需要在构造函数中传递其服务器，若传递nullptr则需要使用SetFather方法设置
/// 2.使用ChangeFather可以从原服务器中删除自己，然后把自己添加进新服务器
/// 3.使用LeaveFather可以从原服务器中删除自己
/// </summary>
class VMObject
{
private:
	const VirtualMachine& VMProperty; //属性
	int VM_ID;				   //虚拟机ID
	VM_NodeType nodeType;	   //虚拟机节点类型
	SMObject* father;		   //所寄存的服务器

public:
	friend class SMObject;
	//构造函数（属性，VM号，宿主服务器）
	VMObject(const VirtualMachine& _VMProperty, int _vm_id, SMObject* _father);

	~VMObject();
	

	//初始化father，若已有father则会返回false
	bool SetFather(SMObject* _father);
	

	//修改寄存的服务器（会从原服务器里删除自己（如果有原服务器的话））
	bool ChangeFather(SMObject* _newfather);

	//从原服务器中离开
	bool LeaveFather();

	VM_NodeType GetNodeType() const { return nodeType; } //获取节点类型
	void SetNodeType(VM_NodeType _nodeType) { nodeType = _nodeType; }
	const VirtualMachine& GetProperty() const { return VMProperty; } //获取属性
	int GetID() const { return VM_ID; }						  //获取VM_ID
	int GetFatherID() const;
};
