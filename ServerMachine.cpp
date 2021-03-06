#include "ServerMachine.h"

using namespace std;

ServerMachine::ServerMachine(string _modelType, int _core, int _memoryCapacity, int _hardwareCost, int _dailyCost)
{
	modelType = _modelType;
	core = _core;
	memoryCapacity = _memoryCapacity;
	hardwareCost = _hardwareCost;
	dailyCost = _dailyCost;

	if (core / memoryCapacity >= SMstandrad)
	{
		machineType = MachineType::HighCore;
	}
	else if (memoryCapacity / core >= SMstandrad)
	{
		machineType = MachineType::HighMemory;
	}
	else machineType = MachineType::Balance;
}
