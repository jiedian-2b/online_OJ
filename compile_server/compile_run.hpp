#pragma once

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "compiler.hpp"
#include "runner.hpp"

#include <nlohmann/json.hpp>
#include <signal.h>
#include <unistd.h>

/***************************************
 * 输入:
 * code： 用户提交的代码
 * input: 用户给自己提交的代码对应的输入，不做处理
 * cpu_limit: 时间要求
 * mem_limit: 空间要求
 *
 * 输出:
 * 必填
 * status: 状态码
 * reason: 请求结果
 * 选填：
 * stdout: 我的程序运行完的结果
 * stderr: 我的程序运行完的错误结果
 *
 * 参数：
 * in_json: {"code": "#include...", "input": "","cpu_limit":1,
 * "mem_limit":10240} out_json: {"status":"0",
 * "reason":"","stdout":"","stderr":"",}
 * ************************************/

*out_json = root_2.dump();

void Start(const std::string &in_json, std::string *out_json) {
  json root_2;
  json root_1 = json::parse(in_json);
  std::string code = root_1["code"].get<std::string>();
  std::string input = root_1["input"].get<std::string>();
  int cpu_limit = root_1["cpu_limit"].get<int>();
  int mem_limit = root_1["mem_limit"].get<int>();

  //
  std::string file_name;
  file_name = FileUtil::UniqFileName(); //获取唯一的文件名

  if (!(FileUtil::WriteFile(PathUtil::Src(file_name, ".cpp"), code))) {
    //代码写入文件失败
    root_2["status"] = -1; //代码写入出错
    root_2["reason"] = "代码写入出错";
    

    *out_json = root_2.dump();
    //结束
  }

  if (!(compiler::Compile(PathUtil::Src(file_name, ".cpp")))) {
    std::string err_content;
    if (FileUtil::ReadFile(
            PathUtil::CompilerError(file_name, ".compiler_error"), &err_content,
            true) &&
        !err_content.empty()) {
      root_2["status"] = -5;          //编译时出错
      root_2["reason"] = err_content; //_compiler_error读

    } else {
      root_2["status"] = -2; //内部错误
      root_2["reason"] = "内部错误";
    }
    //结束
    *out_json = root_2.dump();
    return;
  }

  int num = runner::Run(PathUtil::Exe(file_name, ".exe"));

  if (num < 0) {
    if (num == -1 || num == -2) {
      root_2["status"] = -2; //内部错误
      root_2["reason"] = "内部错误";

    }

    else if (num == -4) {
      root_2["status"] = -4;
      root_2["reason"] = "不知名错误";
    }

    else if (num == -3) {
      std::string err_content;
      FileUtil::ReadFile(PathUtil::Stderr(file_name, ".stderr"), &err_content,
                         true);

      root_2["status"] = -3;
      root_2["reason"] = "可执行文件启动失败";
      root_2["stderr"] = err_content;
    }

  }

  else if (num > 0) {
    std::string err_content;
    FileUtil::ReadFile(PathUtil::Stderr(file_name, ".stderr"), &err_content,
                       true);
    if (num == 6) {
      root_2["status"] = -6; //具体信号名
      root_2["reason"] = "内存超过范围";
      root_2["stderr"] = err_content;
    }

    else if (num == 24) {
      root_2["status"] = -7; //具体信号名
      root_2["reason"] = "CPU使用超时";
      root_2["stderr"] = err_content;

    }

    else if (num == 8) {
      root_2["status"] = -8; //具体信号名
      root_2["reason"] = "浮点数溢出";
      root_2["stderr"] = err_content;

    }

    else {
      root_2["status"] = -9; //具体信号名
      root_2["reason"] = "不知名错误";
      root_2["stderr"] = err_content;
    }
  }

  else {
    root_2["status"] = 0; //具体信号名
    root_2["reason"] = "程序编译加运行成功";

    std::string err_content;
    FileUtil::ReadFile(PathUtil::Stdout(file_name, ".stdout"), &err_content,
                       true);

    root_2["stdout"] = err_content;
  }

  *out_json = root_2.dump();

  return;
}
