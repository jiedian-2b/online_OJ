#include<iostream>
#include"/root/my_oj/comm/log.hpp"
#include"/root/my_oj/comm/util.hpp"
#include"/root/my_oj/compile_server/compiler.hpp"
using namespace ns_log;
int main()
{
    Compiler cmp;
    std::string filename="test_1";
    bool res=cmp.Compile(filename);
    if(!res)
    {
        LOG(ERROR) << " 编译失败，请查看编译错误文件：" << PathUtil::CompilerError(filename,".compiler_error") << "\n"<< std::flush;
        return -1;
    }
    else
    {
        LOG(INFO) << "✅ 编译成功！" << "\n"<< std::flush;
        return 0;
    }

    perror("未知错误");
    return -3;
}