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
using namespace std;

//服务器表
unordered_map<string, ServerMachine> serverMachines;
//虚拟机表
unordered_map<string, VirtualMachine> virtualMachines;
//每日请求列表
map<int, VirtualMachine> requestVM;
//当前维护着的服务器
map<int, SMObject*> currentSM;
//当前维护着的虚拟机
map<int, VMObject*> currentVM;

void InputSMandVM()//将服务器信息及虚拟机信息接收进来
{
    ifstream ifs("C:\\Users\\Lantyais\\Desktop\\华为比赛\\training-1.txt");
    int N;  //服务器数量
    ifs >> N;
    ifs.get();
    string catcher;
    string _model;      //型号 
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

    ifs >> N;        //虚拟机数量
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

    int T;          //T-多少天
    int R;          //R-每天有多少请求

    string _opType;
    int _VM_id;

    ifs >> T;
    while (T--)
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
            if (_opType == "add")
            {
                stm >> _model >> _VM_id;

                requestVM[_VM_id] = virtualMachines[_model];
            }
            else//del
            {
                stm >> _VM_id;
                if (requestVM.find(_VM_id) != requestVM.end())      //在同一天先创建后删除
                {
                    requestVM.erase(_VM_id);
                }
                else
                {
                    //TODO 删除的是之前日子里创建的虚拟机
                }
            }
        }

        //TODO 每天的请求得到之后进行处理
        //TODO 
        //TODO

        requestVM.clear();          //每天结束后清空请求列表
    }
}



int main()
{ 
    InputSMandVM();

    return 0;
}