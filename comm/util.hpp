
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
//#include <boost/algorithm/string.hpp>


class PathUtil
{
    public:

        static std::string temp_path ;
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path;
            path=file_name+suffix;
            return path;
        }

        static std::string  Src(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }

        static std::string  Exe(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }

        static std::string  Stdin(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }

        static std::string  CompilerError(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }


        static std::string  Stdout(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }


        static std::string  Stderr(const std::string &file_name, const std::string &suffix)
         {
            std::string path=temp_path+"/";
            path=path+file_name;
            
            return AddSuffix(path,suffix);
         }
};
std::string PathUtil::temp_path = "./temp";

class TimeUtil {
    public:
      static std::string GetTimeStamp() {
        struct timeval _time;
        gettimeofday(&_time, nullptr);
        return std::to_string(_time.tv_sec);
      }
      //获得毫秒时间戳
      static std::string GetTimeMs() {
        struct timeval _time;
        gettimeofday(&_time, nullptr);
        return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
      }
    };   
class FileUtil
{
    public:
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                //获取属性成功，文件已经存在
                return true;
            }

            return false;
        }
        static std::string UniqFileName()
        {
            static std::atomic_uint id(0);
            id++;
            // 毫秒级时间戳+原子性递增唯一值: 来保证唯一性
            std::string ms = TimeUtil::GetTimeMs();
            std::string uniq_id = std::to_string(id);
            return ms + "_" + uniq_id;
        }
        static bool WriteFile(const std::string &target, const std::string &content)
        {
            std::ofstream out(target);
            if (!out.is_open())
            {
                return false;
            }
            out.write(content.c_str(), content.size());
            out.close();
            return true;
        }
        static bool ReadFile(const std::string &target, std::string *content, bool keep = false)
        {
            (*content).clear();

            std::ifstream in(target);
            if (!in.is_open())
            {
                return false;
            }
            std::string line;
            // getline:不保存行分割符,有些时候需要保留\n,
            // getline内部重载了强制类型转化
            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }

        
    
    
};

    