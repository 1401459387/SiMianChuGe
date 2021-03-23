#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ServerMachine.h"
#include "VirtualMachine.h"
#include "SMObject.h"
#include "VMObject.h"
#include "SortedArray.h"
#include <ctime>
#ifdef linux
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#endif 

typedef long long ll;
using namespace std;

//服务器类型表
unordered_map<string, ServerMachine> serverMachines;
//三种不同类型的服务器(高核心/均衡/高内存)
vector<ServerMachine> type_SMs[3];
//虚拟机类型表
unordered_map<string, VirtualMachine> virtualMachines;
//当前购买了的服务器表
unordered_map<int, SMObject*> currentSM;
//当前购买了的三类不同的服务器(高核心/均衡/高内存)
//SortedArray<SMObject*> type_currentSMs[3];
//当前配置了的虚拟机表
unordered_map<int, VMObject*> currentVM;
//当前虚拟机表的列表
vector<VMObject*> currentVMList;
//当前的请求队列
vector<pair<int, string>> requestList;

//每日购买列表
map<string, int> dailyPurchase;
//每日迁移列表
vector<tuple<int, int, VM_NodeType> > dailyMigration;
//每日创建列表
vector<pair<int, VMObject*> > dailyCreation;

//当前的服务器ID号的最大值（虚假的id）
int max_sm_id = 0;
//真正的id
int id = 0;
//记录所有虚拟机最小的内存和最小的cpu占用
int min_VMcore = 1000000000;
int min_VMmemo = 1000000000;

int rd(int mod) //生成随机数函数
{
#ifdef WIN32
	srand(time(0));
#endif 

#ifdef linux
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * 1000000 + tv.tv_usec / 1000000);
#endif 

	return rand() % mod;

}

void InitInput(ifstream* ifs)
{
	int N; //服务器数量
	*ifs >> N;
	ifs->get();
	string catcher;
	string _model; //型号
	int _core, _mCp, _hC, _dC;
	for (int i = 0; i < N; ++i)
	{
		getline(*ifs, catcher);

		int len = catcher.length();
		catcher = catcher.substr(1, len - 2);
		replace(catcher.begin(), catcher.end(), ',', ' ');
		stringstream stm(catcher);
		stm >> _model;
		stm >> _core >> _mCp >> _hC >> _dC;
		auto& sm = serverMachines[_model] = ServerMachine(_model, _core, _mCp, _hC, _dC);
		switch (sm.GetMechType())
		{
		case MachineType::HighCore:
			type_SMs[0].push_back(sm);
			break;
		case MachineType::Balance:
			type_SMs[1].push_back(sm);
			break;
		case MachineType::HighMemory:
			type_SMs[2].push_back(sm);
			break;
		}
	}

	for (int i = 0; i <= 2; ++i)
	{
		sort(type_SMs[i].begin(), type_SMs[i].end(), [](const ServerMachine& a, const ServerMachine& b)
			-> bool
			{
				if (a.GetDailyCost() == b.GetDailyCost()) return a.GetHardwareCost() < a.GetHardwareCost();
				return a.GetDailyCost() < b.GetDailyCost();
			});
	}

	*ifs >> N; //虚拟机数量
	ifs->get();
	bool _tN;
	for (int i = 0; i < N; ++i)
	{
		getline(*ifs, catcher);
		int len = catcher.length();
		catcher = catcher.substr(1, len - 2);
		replace(catcher.begin(), catcher.end(), ',', ' ');
		stringstream stm(catcher);
		stm >> _model;
		stm >> _core >> _mCp >> _tN;
		virtualMachines[_model] = VirtualMachine(_model, _core, _mCp, _tN);
	}
}

void InitInput()
{
	int N; //服务器数量
	cin >> N;
	cin.get();
	string catcher;
	string _model; //型号
	int _core, _mCp, _hC, _dC;
	for (int i = 0; i < N; ++i)
	{
		getline(cin, catcher);

		int len = catcher.length();
		catcher = catcher.substr(1, len - 2);
		replace(catcher.begin(), catcher.end(), ',', ' ');
		stringstream stm(catcher);
		stm >> _model;
		stm >> _core >> _mCp >> _hC >> _dC;
		serverMachines[_model] = ServerMachine(_model, _core, _mCp, _hC, _dC);
	}

	cin >> N; //虚拟机数量
	cin.get();
	bool _tN;
	for (int i = 0; i < N; ++i)
	{
		getline(cin, catcher);
		int len = catcher.length();
		catcher = catcher.substr(1, len - 2);
		replace(catcher.begin(), catcher.end(), ',', ' ');
		stringstream stm(catcher);
		stm >> _model;
		stm >> _core >> _mCp >> _tN;
		virtualMachines[_model] = VirtualMachine(_model, _core, _mCp, _tN);
		if (_core < min_VMcore)
			min_VMcore = _core;
		if (_mCp < min_VMmemo)
			min_VMmemo = _mCp;
	}
}

//决定为当前的虚拟机购买哪一款服务器
ServerMachine& ChooseServer(const VirtualMachine& vm_property)
{
	int vm_core = vm_property.GetCore();
	int vm_memory = vm_property.GetMemoryCapacity();

	int type;
	if (vm_core / vm_memory >= SMstandrad * 2)
	{
		type = 0;
	}
	else if (vm_memory / vm_core >= SMstandrad * 2)
	{
		type = 2;
	}
	else type = 1;

	int maxsearch = type_SMs[type].size() >> 1;
	//先在本类里寻找
	while (maxsearch--)
	{
		int _rand = rd(type_SMs[type].size());
		ServerMachine& serverMachine = type_SMs[type][_rand];
		int sm_core = serverMachine.GetCore();
		int sm_memory = serverMachine.GetMemoryCapacity();
		if (!vm_property.IsTwoNode())
		{
			sm_core >>= 1;
			sm_memory >>= 1;
		}
		if (vm_core <= sm_core && vm_memory <= sm_memory) return serverMachine;
	}

	//广撒网
	while (true)
	{
		int _rand = rd(serverMachines.size());
		auto iter = serverMachines.begin();
		while (--_rand && iter != serverMachines.end()) ++iter;
		if (iter == serverMachines.end()) iter = serverMachines.begin();
		ServerMachine& serverMachine = iter->second;
		int sm_core = serverMachine.GetCore();
		int sm_memory = serverMachine.GetMemoryCapacity();
		if (!vm_property.IsTwoNode())
		{
			sm_core >>= 1;
			sm_memory >>= 1;
		}
		if (vm_core <= sm_core && vm_memory <= sm_memory) return serverMachine;
	}
}

char GetStringFromNodeType(const VM_NodeType& nodeType)
{
	switch (nodeType)
	{
	case VM_NodeType::A:
		return 'A';
	case VM_NodeType::B:
		return 'B';
	default:
		return ' ';
	}
}

//从当前的虚拟机表中随机洗出数个元素
void Shuffle(vector<VMObject*>& list, int num)
{
	unordered_set<int> hasChose;
	int totalnum = currentVMList.size();
	while (num)
	{
		int trand = rd(totalnum);
		auto elem = currentVMList[trand];
		if (hasChose.find(elem->GetID()) != hasChose.end())
		{
			continue;
		}

		list.push_back(elem);
		hasChose.insert(elem->GetID());
		--num;
	}
}

/*void DailyMigration(int maxMigrationNum)
{
	if (!maxMigrationNum) return;
	vector<VMObject*> list;
	Shuffle(list, maxMigrationNum);
	for (int i = 0; i < maxMigrationNum; ++i)
	{
		VMObject* cur = list[i];
		const VirtualMachine& tproperty = cur->GetProperty();
		MachineType type;
		if (tproperty.GetCore() / tproperty.GetMemoryCapacity() >= SMstandrad * 4)
		{
			type = MachineType::HighCore;
		}
		else if (tproperty.GetMemoryCapacity() / tproperty.GetCore() >= SMstandrad * 4)
		{
			type = MachineType::HighMemory;
		}
		else
		{
			type = MachineType::Balance;
		}

		auto iter = currentSM.begin();
		for (; iter != currentSM.end(); ++iter)
		{
			if (iter->second->GetProperty().GetMechType() == type)
			{
				if (iter->second->GetId() != cur->GetFatherID() && iter->second->AddChild(cur))
				{
					dailyMigration.push_back(tuple<int, int, VM_NodeType>(cur->GetID(), iter->second->GetTrueId(), cur->GetNodeType()));
				}
			}
		}
	}
}*/

void DailyMigration(int maxMigrationNum)
{
	if (!maxMigrationNum) return;
	bool isFull = false;
	for (auto it = currentSM.begin();!isFull && it != currentSM.end(); it++)
	{
		if (it->second->GetACore() > min_VMcore && it->second->GetBCore() > min_VMcore && it->second->GetAMemo() > min_VMmemo && it->second->GetBMemo() > min_VMmemo)
		{
			for (auto it2 = currentVM.begin(); !isFull && it2 != currentVM.end(); it2++)
			{
				if (it2->second->GetFatherID() != it->second->GetId() && it->second->AbleToAddChild(it2->second))
				{
					dailyMigration.push_back(tuple<int, int, VM_NodeType>(it2->second->GetID(), it->second->GetId(), it2->second->GetNodeType()));
					if (dailyMigration.size() >= maxMigrationNum)
						break;
				}
			}
		}
	}

	for (auto iter = dailyMigration.begin(); iter != dailyMigration.end(); ++iter)
	{
		int vm_id = get<0>(*iter);
		int sm_id = get<1>(*iter);
		currentSM[sm_id]->AddChild(currentVM[vm_id]);
	}
}

void DailyOutput()
{
	int Q = dailyPurchase.size();
	printf("(purchase,%d)\n", Q);
	for (auto iter = dailyPurchase.begin(); iter != dailyPurchase.end(); ++iter)
	{
		printf("(%s,%d)\n", iter->first.c_str(), iter->second);
		for (auto it = currentSM.begin(); it != currentSM.end(); it++)
		{
			if (it->second->Gettype() == iter->first.c_str() && it->second->GetTrueId() == -1)
			{
				it->second->SetTrueId(id++);
			}
		}
	}
	int W = dailyMigration.size();
	printf("(migration,%d)\n", W);
	for (auto iter = dailyMigration.begin(); iter != dailyMigration.end(); ++iter)
	{
		int vm_id = get<0>(*iter);
		int sm_id = get<1>(*iter);
		VM_NodeType& nodeType = get<2>(*iter);
		sm_id = currentSM[sm_id]->GetTrueId();
		if (nodeType == VM_NodeType::Both)
		{
			printf("(%d,%d)\n", vm_id, sm_id);
		}
		else
		{
			char s_nodeType = GetStringFromNodeType(nodeType);
			printf("(%d,%d,%c)\n", vm_id, sm_id, s_nodeType);
		}
	}

	for (auto iter = dailyCreation.begin(); iter != dailyCreation.end(); ++iter)
	{
		int sm_id = iter->second->GetFatherTureID();
		VM_NodeType nodeType = iter->second->GetNodeType();
		if (nodeType == VM_NodeType::Both)
		{
			printf("(%d)\n", sm_id);
		}
		else
		{
			char s_nodeType = GetStringFromNodeType(nodeType);
			printf("(%d,%c)\n", sm_id, s_nodeType);
		}
	}
}

void DailyClear()
{
	requestList.clear();
	dailyPurchase.clear();
	dailyMigration.clear();
	dailyCreation.clear();
}

int main()
{
	//clock_t start = clock();

	//ifstream in("C:/Users/14014/Desktop/Simianchuge/SiMianChuGe-main/training-1.txt");
	InitInput();

	int T;              //天数
	int TotalT;         //总天数
	int R;              //每天的请求数

	string catcher;
	string _model;
	string _opType;
	int _VM_id;

	cin >> T;
	TotalT = T;
	while (T--)
	{
		if (T == TotalT >> 1)
		{
			for (int i = 0; i <= 2; ++i)
			{
				sort(type_SMs[i].begin(), type_SMs[i].end(), [](const ServerMachine& a, const ServerMachine& b)
					-> bool
					{
						if (a.GetHardwareCost() == b.GetDailyCost()) return a.GetDailyCost() < b.GetDailyCost();
						return a.GetHardwareCost() < a.GetHardwareCost();

					});
			}
		}
		cin >> R;
		cin.get();
		//获取当日的创建申请
		for (int i = 0; i < R; ++i)
		{
			getline(cin, catcher);
			int len = catcher.length();
			catcher = catcher.substr(1, len - 2);
			replace(catcher.begin(), catcher.end(), ',', ' ');
			stringstream stm(catcher);
			stm >> _opType;
			if (_opType == "add")
			{
				stm >> _model >> _VM_id;
				//创建一台虚拟机
				requestList.push_back({ _VM_id,_model });
			}
			else //del   
			{
				stm >> _VM_id;
				requestList.push_back({ _VM_id,"" });
			}
		}

		//尝试迁移
		//int maxMigrateNum = currentVM.size() /1000;
		//DailyMigration(maxMigrateNum);


		//为请求队列寻找目标服务器/删除目标虚拟机
		for (auto iter = requestList.begin(); iter != requestList.end(); ++iter)
		{
			if (iter->second == "")
			{
				VMObject* vm = currentVM[iter->first];
				vm->LeaveFather();
				currentVM.erase(iter->first);
				VMObject* theback = currentVMList.back();
				currentVMList[vm->index] = theback;
				theback->index = vm->index;
				currentVMList.pop_back();
				delete vm;
			}
			else
			{
				VirtualMachine vm_property = virtualMachines[iter->second];
				VMObject* vmObject = new VMObject(vm_property, iter->first, nullptr);
				currentVM[iter->first] = vmObject;
				vmObject->index = currentVMList.size();
				currentVMList.push_back(vmObject);
				//改进部分：按照类型来选择服务器
				auto iter2 = currentSM.begin();
				auto it3 = currentSM.end();
				for (; iter2 != currentSM.end(); ++iter2)
				{

					int vm_core = vm_property.GetCore();
					int vm_memory = vm_property.GetMemoryCapacity();
					MachineType type;
					if (vm_core / vm_memory >= SMstandrad * 2)
					{
						type = MachineType::HighCore;
					}
					else if (vm_memory / vm_core >= SMstandrad * 2)
					{
						type = MachineType::HighMemory;
					}
						else type = MachineType::Balance;

					if (type==iter2->second->GetProperty().GetMechType()&&iter2->second->AddChild(vmObject))
					{
						//找到了可以塞的下的服务器
						dailyCreation.push_back(make_pair(iter2->first, vmObject));
						break;
					}
					else
						if(it3==currentSM.end()&&iter2->second->AbleToAddChild(vmObject))//保留随机选择结果
							it3 = iter2;
					
				}
				//找不到可以塞的下的服务器
				if (iter2 == currentSM.end())
				{
					if (it3 != currentSM.end())//还原随机选择结果
					{
						it3->second->AddChild(vmObject);
						dailyCreation.push_back(make_pair(it3->first, vmObject));
						continue;
					}
					ServerMachine& sm_property = ChooseServer(vm_property);
					SMObject* smObject = new SMObject(sm_property, max_sm_id);
					smObject->AddChild(vmObject);
					dailyCreation.push_back(make_pair(max_sm_id, vmObject));

					currentSM[max_sm_id] = smObject;
					if (dailyPurchase.find(sm_property.GetModelType()) != dailyPurchase.end())
					{
						++dailyPurchase[sm_property.GetModelType()];
					}
					else
					{
						dailyPurchase[sm_property.GetModelType()] = 1;
					}

					++max_sm_id;
				}
			}
		}

		//输出
		DailyOutput();

		//清理垃圾
		DailyClear();
	}

	fflush(stdout);

	//clock_t finish = clock();
	//cout << (finish - start) / 1000 << "s" << endl;
}