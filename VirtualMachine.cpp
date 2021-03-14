#include "VirtualMachine.h"

using namespace std;

VirtualMachine::VirtualMachine(string _modelType,int _core,int _memoryCapacity,bool _isTwoNode)
{
	modelType = _modelType;
	core = _core;
	memoryCapacity = _memoryCapacity;
	isTwoNode = _isTwoNode;
}
