#include <iostream>
#include "ServerMachine.h"
#include "VirtualMachine.h"
#include "SMObject.h"
#include "VMObject.h"
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <time.h>
using namespace std;

//服务器表
unordered_map<string, ServerMachine> serverMachines;
//虚拟机表
unordered_map<string, VirtualMachine> virtualMachines;
//每日请求添加列表
vector<pair<int, VirtualMachine>> requestAddVM;
//每日请求删除列表
vector<int> requestDelVM;
//当前维护着的服务器
vector<SMObject> currentSM;
//当前维护着的虚拟机
vector<VMObject *> currentVM;
//当日购买的服务器列表
vector<ServerMachine> purchasedMachines;
//服务器id
int server_id = 0;

int rd() //生成随机数函数
{
    srand(time(0));
    return rand() % serverMachines.size();
}

int main()
{
    ifstream ifs("training-1.txt");
    int N; //服务器数量
    ifs >> N;
    ifs.get();
    string catcher;
    string _model; //型号
    int _core, _mCp, _hC, _dC;
    for (int i = 0; i < N; ++i)
    {
        getline(ifs, catcher);

        int len = catcher.length();
        catcher = catcher.substr(1, len - 2);
        replace(catcher.begin(), catcher.end(), ',', ' ');
        stringstream stm(catcher);
        stm >> _model;
        stm >> _core >> _mCp >> _hC >> _dC;
        serverMachines[_model] = ServerMachine(_model, _core, _mCp, _hC, _dC);
    }

    ifs >> N; //虚拟机数量
    ifs.get();
    bool _tN;
    for (int i = 0; i < N; ++i)
    {
        getline(ifs, catcher);
        int len = catcher.length();
        catcher = catcher.substr(1, len - 2);
        replace(catcher.begin(), catcher.end(), ',', ' ');
        stringstream stm(catcher);
        stm >> _model;
        stm >> _core >> _mCp >> _tN;
        virtualMachines[_model] = VirtualMachine(_model, _core, _mCp, _tN);
    }

    int T; //T-多少天
    int R; //R-每天有多少请求

    string _opType;
    int _VM_id;

    ifs >> T;
    while (T--) //请求接收
    {
        ifs >> R;
        ifs.get();
        for (int i = 0; i < R; ++i)
        {
            getline(ifs, catcher);
            int len = catcher.length();
            catcher = catcher.substr(1, len - 2);
            replace(catcher.begin(), catcher.end(), ',', ' ');
            stringstream stm(catcher);
            stm >> _opType;
            if (_opType == "add") //添加请求放入添加请求队列
            {
                stm >> _model >> _VM_id;

                requestAddVM.push_back(pair<int, VirtualMachine>(_VM_id, virtualMachines[_model]));
            }
            else //del   删除请求放入删除请求队列
            {
                stm >> _VM_id;
                requestDelVM.push_back(_VM_id);
            }
        }

        //添加请求的处理
        for (auto it = requestAddVM.begin(); it != requestAddVM.end(); it++) //遍历添加请求队列
        {
            currentVM.push_back(&VMObject(it->second, it->first, NULL));
            if (!currentSM.empty()) //若当前有服务器存在
            {
                auto it1 = currentSM.begin();
                for (; it1 != currentSM.end(); it1++)
                {
                    if (it1->AddChild(currentVM.back())) //检测到可用服务器直接将虚拟机添加至该服务器
                        break;
                }
                if (it1 == currentSM.end()) //无可用空闲服务器
                {
                    auto it2 = serverMachines.begin();
                    do
                    {
                        it2 = serverMachines.begin();
                        int lim = rd(); //服务器申请采用随机申请
                        for (int i = 0; i < lim; i++)
                            it2++;
                    } while ((it2->second.GetCore() < currentVM.back()->GetProperty().GetCore()) && (it2->second.GetMemoryCapacity() < currentVM.back()->GetProperty().GetMemoryCapacity()));
                    //只有当随机到的服务器足够容纳虚拟机才会真正申请
                    purchasedMachines.push_back(it2->second);
                    purchasedMachines.back().SetPurchased_id(currentSM.size());
                    currentSM.push_back(SMObject(it2->second));
                    currentVM.back()->SetFather(&currentSM.back());
                }
            }
            else //若无服务器则申请新服务器
            {
                auto it2 = serverMachines.begin();
                do
                {
                    it2 = serverMachines.begin();
                    int lim = rd();
                    for (int i = 0; i < lim; i++)
                        it2++;
                } while ((it2->second.GetCore() < currentVM.back()->GetProperty().GetCore()) && (it2->second.GetMemoryCapacity() < currentVM.back()->GetProperty().GetMemoryCapacity()));
                purchasedMachines.push_back(it2->second);
                currentSM.push_back(SMObject(it2->second));
                currentVM.back()->SetFather(&currentSM.back());
            }
        }

        //对删除请求的处理
        for (auto it = requestDelVM.begin(); it != requestDelVM.end(); it++)
        {
            for (auto it1 = currentVM.begin(); it1 != currentVM.end(); it1++)
            {
                if ((*it1)->GetID() == (*it))
                {
                    (*it1)->LeaveFather();
                    currentVM.erase(it1);
                }
            }
        }

        //计算所购买的服务器种类
        map<string, bool> cal_aid;
        int Q = 0;
        auto it1 = cal_aid.begin();
        for (auto it = purchasedMachines.begin(); it != purchasedMachines.end(); it++)
        {
            if (cal_aid.find(it->GetModelType()) == cal_aid.end())
            {
                cal_aid.insert(pair<string, bool>(it->GetModelType(), true));
                Q++;
            }
        }
        cout << "(purchase, " << Q << ")" << endl;

        //输出已购买服务器列表
        sort(purchasedMachines.begin(), purchasedMachines.end()); //先按服务器型号对购买的服务器进行排序
        if (purchasedMachines.size() <= 1)                        //根据当天购买服务器数量做出不同抉择
        {
            if (purchasedMachines.size() != 0)
            {
                cout << "(" << purchasedMachines[0].GetModelType() << ", 1)" << endl;
                currentSM[purchasedMachines[0].GetPurchased_id()].SetId(server_id++); //按照题目要求设置服务器Id
            }
        }
        else //当天购买服务器数量大于1
        {
            string s = purchasedMachines[0].GetModelType();
            int sum = 1;
            for (int i = 1; i < purchasedMachines.size(); i++)
            {
                if (purchasedMachines[i].GetModelType() == s)
                    sum++;
                else
                {
                    cout << "(" << s << ", " << sum << ")" << endl;
                    sum = 1;
                    s = purchasedMachines[i].GetModelType();
                }
                currentSM[purchasedMachines[i].GetPurchased_id()].SetId(server_id++);
            }
            cout << "(" << s << ", " << sum << ")" << endl;
        }

        int W = 0; //W为迁移次数
        cout << "(migration, " << W << ")" << endl;

        //输出虚拟机状态
        for (auto it = requestAddVM.begin(); it != requestAddVM.end(); it++)
        {
            int i;
            for (i = 0; i < currentVM.size(); i++)
                if (currentVM[i]->GetID() == it->first)
                    break;
            if (it->second.IsTwoNode())
                cout << "(" << currentVM[i]->GetFatherID() << ")" << endl;
            else
                cout << "(" << currentVM[i]->GetFatherID() << ", " << currentVM[i]->GetNodeType() << ")" << endl;
        }

        requestAddVM.clear();
        requestDelVM.clear();
        purchasedMachines.clear();
        //每天结束后清空请求列表
    }
    return 0;
}