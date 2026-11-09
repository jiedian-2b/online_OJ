#include<iostream>
#include"/root/my_oj/comm/log.hpp"
#include"/root/my_oj/comm/util.hpp"
#include"/root/my_oj/compile_server/runner.hpp"
using namespace ns_log;

int main()
{
    runner r;
    std::string file_name="test_1";
    int res=r.Run(file_name,2, 262144);
    if(res==0)
    {
        LOG(INFO) << "程序运行成功" << "\n";
    }

    else 
    {
        LOG(ERROR) << "程序运行失败" << "\n";
    }
    return 0;
}