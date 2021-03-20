#include "VMObject.h"

int VMObject::GetFatherID() const 
{ return father->SM_Id; }


VMObject::VMObject(VirtualMachine _VMProperty, int _vm_id, SMObject* _father) : VMProperty(_VMProperty), VM_ID(_vm_id)
{
	if (_father)
	{
		_father->AddChild(this);
	}
	father = _father;
	nodeType = VM_NodeType::Unknow;
}

VMObject::~VMObject()
{
	if (father)
		father->RemoveChild(this);
}

bool VMObject::SetFather(SMObject* _father)
{
	if (father)
		return false; //已经有父亲了
	return _father->AddChild(this);
}

bool VMObject::ChangeFather(SMObject* _newfather)
{
	if (father)
		father->RemoveChild(this);
	return _newfather->AddChild(this);
}
bool VMObject::LeaveFather()
{
	if (!father)
		return false; //本来就是孤儿
	return father->RemoveChild(this);
}
