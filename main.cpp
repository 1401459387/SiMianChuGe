#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "ServerMachine.h"
#include "VirtualMachine.h"
#include "SMObject.h"
#include "VMObject.h"
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
//虚拟机类型表
unordered_map<string, VirtualMachine> virtualMachines;
//当前购买了的服务器表
unordered_map<int, SMObject*> currentSM;
//当前配置了的虚拟机表
unordered_map<int, VMObject*> currentVM;
//当前的请求队列
vector<pair<int, string>> requestList;

//每日购买列表
map<string, int> dailyPurchase;
//每日迁移列表
vector<tuple<int, int, VM_NodeType> > dailyMigration;
//每日创建列表
vector<pair<int, VM_NodeType> > dailyCreation;

//当前的服务器ID号的最大值
int max_sm_id = 0;

int rd() //生成随机数函数
{
#ifdef WIN32
    srand(time(0));
#endif 

#ifdef linux
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif 
    
    return rand() % serverMachines.size();
    
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
        serverMachines[_model] = ServerMachine(_model, _core, _mCp, _hC, _dC);
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
    }
}

//决定为当前的虚拟机购买哪一款服务器
ServerMachine& ChooseServer(const VirtualMachine& vm_property)
{
    int vm_core = vm_property.GetCore();
    int vm_memory = vm_property.GetMemoryCapacity();

    while (true)
    {
        int _rand = rd();
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
    /*auto iter = serverMachines.begin();
    for (; iter != serverMachines.end(); ++iter)
    {
        ServerMachine& serverMachine = iter->second;
        int sm_core = serverMachine.GetCore();
        int sm_memory = serverMachine.GetMemoryCapacity();
        if (vm_core <= sm_core && vm_memory <= sm_memory) return serverMachine;
    }
    if (iter == serverMachines.end()) throw new exception("No server match!");*/
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

void DailyOutput()
{
    int Q = dailyPurchase.size();
    printf("(purchase,%d)\n", Q);
    for (auto iter = dailyPurchase.begin(); iter != dailyPurchase.end(); ++iter)
    {
        printf("(%s,%d)\n", iter->first.c_str(), iter->second);
    }
    int W = dailyMigration.size();
    printf("(migration,%d)\n", W);
    for (auto iter = dailyMigration.begin(); iter != dailyMigration.end(); ++iter)
    {
        int vm_id = get<0>(*iter);
        int sm_id = get<1>(*iter);
        VM_NodeType& nodeType = get<2>(*iter);
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
        int sm_id = iter->first;
        VM_NodeType& nodeType = iter->second;
        if (nodeType == VM_NodeType::Both)
        {
            printf("(%d)\n",sm_id);
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

    //ifstream in("training-1.txt");
    InitInput();

    int T;              //天数
    int R;              //每天的请求数

    string catcher;
    string _model;
    string _opType;
    int _VM_id;

    cin >> T;
    while (T--)
    {
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
        //TODO 尝试迁移

        //为请求队列寻找目标服务器/删除目标虚拟机
        for (auto iter = requestList.begin(); iter != requestList.end(); ++iter)
        {
            if (iter->second == "")
            {
                currentVM[iter->first]->LeaveFather();
                currentVM.erase(iter->first);
            }
            else
            {
                VirtualMachine& vm_property = virtualMachines[iter->second];
                VMObject* vmObject = new VMObject(vm_property, iter->first, nullptr);
                currentVM[iter->first] = vmObject;

                auto iter2 = currentSM.begin();
                for (; iter2 != currentSM.end(); ++iter2)
                {
                    if (iter2->second->AddChild(vmObject))
                    {
                        //找到了可以塞的下的服务器
                        dailyCreation.push_back(make_pair(iter2->first, vmObject->GetNodeType()));
                        break;
                    }
                }
                //找不到可以塞的下的服务器
                if (iter2 == currentSM.end())
                {
                    ServerMachine& sm_property = ChooseServer(vm_property);
                    SMObject* smObject = new SMObject(sm_property, max_sm_id);
                    smObject->AddChild(vmObject);
                    dailyCreation.push_back(make_pair(max_sm_id, vmObject->GetNodeType()));

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
        //DailyOutput();

        //清理垃圾
        DailyClear();
    }

    fflush(stdout);

    //clock_t finish = clock();
    //cout << (finish - start) / 1000 << "s" << endl;
}