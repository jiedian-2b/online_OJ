#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

class Compiler
{
    public:
    Compiler(){}
    ~Compiler(){}

    bool Compile(const std::string &file_name)
    {
        std::string err_file_path = PathUtil::CompilerError(file_name, ".compiler_error");
        ns_log::LOG(INFO) << "准备打开错误文件，完整路径 = " << err_file_path << "\n"
                          << std::flush;

        int _compiler_error=open(PathUtil::CompilerError(file_name,".compiler_error").c_str(), O_CREAT | O_WRONLY |O_TRUNC, 0644);
        if(_compiler_error<0)
        {
            ns_log::LOG(ERROR) << "open 编译错误文件失败, filename: " << file_name 
                           << ", errno: " << errno << "\n"<< std::flush;
            return false;
        }


        pid_t pid=fork();
        if(pid<0)
        {
            ns_log::LOG(ERROR) << "fork 创建编译子进程失败" << "\n";
            close(_compiler_error);
            return false;//子进程创建失败，重新去创建
        }

        else if(pid==0)
        {
            dup2(_compiler_error,2);
            close(_compiler_error);
            execlp("g++", "g++", "-o", PathUtil::Exe(file_name,".exe").c_str(),\
                PathUtil::Src(file_name,".cpp").c_str(), "-D", "COMPILER_ONLINE","-std=c++11",  nullptr);

            ns_log::LOG(FATAL) << "execlp 调用g++失败" << "\n"<< std::flush;
            _exit(127);
        }

        else
        {
            close(_compiler_error);

            int status=0;
            int ret=waitpid(pid, &status, 0);
            
            if(ret < 0)
            {
                // waitpid失败，系统异常
                ns_log::LOG(ERROR) << "waitpid for compiler failed"<<"\n"<< std::flush;
                return false;
            }

            if(WIFEXITED(status))
            {
                int code = WEXITSTATUS(status);
                if(code == 0)
                {
                    ns_log::LOG(INFO) << "g++子进程正常退出，编译成功" << "\n"<< std::flush;
                    return true;
                }
                else
                {
                    // 编译失败（两种情况混在这里）
                    ns_log::LOG(WARNING) << "g++子进程正常退出，编译出错，退出码：" << code << "\n"<< std::flush;
                    return false;
                }
            }
            else
            {
                ns_log::LOG(ERROR) << "编译子进程被信号终止" << "\n"<< std::flush;
                // 编译器进程被信号杀掉（比如被kill、段错误），系统异常
                return false;
            }
        }
               
    }

};