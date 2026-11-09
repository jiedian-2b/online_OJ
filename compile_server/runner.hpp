#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
using namespace ns_log;

class runner
{
    
    public:
        runner()
        {

        }
        ~runner()
        {

        }
        static void SetProcLimit(int _cpu_limit, int _mem_limit)
        {
            // 设置CPU时长
            struct rlimit cpu_rlimit;
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = _cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            // 设置内存大小
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = _mem_limit * 1024; //转化成为KB
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }


        int Run(const std::string &file_name,int _cpu_limit, int _mem_limit)
        {

            std::string _stdin_pathname=PathUtil::Stdin(file_name, ".stdin");
            std::string _stdout_pathname=PathUtil::Stdout(file_name, ".stdout");
            std::string _stderr_pathname=PathUtil::Stderr(file_name, ".stderr");
            
            umask(0);
                //程序替换，进行运行动作
                //把stdin,stdout,stderr，文件重定向到 _stdin,_stdout,_stderr
                int _stdin=open(_stdin_pathname.c_str() , O_RDONLY|O_CREAT, 0644);
                int _stdout=open(_stdout_pathname.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                int _stderr=open(_stderr_pathname.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                //改路径

                if(_stdin<0||_stdout<0||_stderr<0)
                {
                    //文件打开失败，程序退出
                    if(_stdin >=0) close(_stdin);
                    if(_stdout >=0) close(_stdout);
                    if(_stderr >=0) close(_stderr);
                    LOG(ERROR) << "文件打开失败，程序退出" << "\n";
                    return -2;
                }

                

                //输出信息更改完成
            pid_t pid=fork();

            

            if(pid<0)
            {
                close(_stdin);
                close(_stderr);
                close(_stdout);
                //子进程创建失败，这几个文件描述符没用了。
                LOG(ERROR) << "子进程创建失败" << "\n";

                return -1;//创建子进程出错，父进程结束
            }

            



            else if(pid==0)
            {
                //改输出流
                dup2(_stdin,0);
                dup2(_stdout,1);
                dup2(_stderr,2);

                SetProcLimit(_cpu_limit, _mem_limit);


                close(_stdin);
                close(_stderr);
                close(_stdout);


                std::string _execute=PathUtil::Exe(file_name, ".exe");
                execl(_execute.c_str()/*我要执行谁*/, _execute.c_str()/*我想在命令行上如何执行该程序*/, nullptr);
                LOG(ERROR) << "exe创建失败" << "\n";

                _exit(127);//子进程结束要退出
                
            }

            else{

                close(_stdin);
                close(_stderr);
                close(_stdout);

                int status = 0;
                waitpid(pid, &status, 0);
                
                if (WIFEXITED(status))
                {
                    
                    int exit_code = WEXITSTATUS(status);

                    if(exit_code==127)
                    {
                        LOG(ERROR) << "程序替换失败" << "\n";
                        return -3;
                    }

                    else
                    {
                        LOG(INFO) << "子进程运行结束，运行结束" << "\n";
                        return 0;
                    }
                }

                else if(WIFSIGNALED(status))
                {
                    std::string error=PathUtil::Stderr(file_name,".stderr");
                    LOG(ERROR) << "发生错误，错误请看:" << error<<  "\n";

                    return WTERMSIG(status);
                }

                else 
                {
                    LOG(ERROR) << "发生未知错误" << "\n";

                    return -4;
                }
            }


        }
};