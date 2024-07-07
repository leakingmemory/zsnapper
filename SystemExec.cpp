//
// Created by sigsegv on 7/7/24.
//

#include <unistd.h>
#include "SystemExec.h"

extern char **environ;

std::map<std::string, std::string> SystemExec::GetEnv() {
    std::map<std::string,std::string> map{};
    int c = 0;
    while (environ[c] != nullptr) {
        std::string str{environ[c]};
        auto eq = str.find('=');
        if (eq < str.size()) {
            std::string name = str.substr(0, eq);
            std::string value = str.substr(eq + 1);
            map.insert_or_assign(name, value);
        } else {
            map.insert_or_assign(str, "");
        }
        ++c;
    }
    return map;
}

void SystemExec::Exec() {
    std::vector<const char *> argv{};
    std::vector<std::string> envd{};
    std::vector<const char *> envp{};

    const std::string binstr{cmd};
    argv.push_back(binstr.c_str());
    for (const std::string &arg: args) {
        argv.push_back(arg.c_str());
    }
    argv.push_back(nullptr);

    auto env = GetEnv();
    for (const auto &e: env) {
        std::string str{e.first};
        str.append("=");
        str.append(e.second);
        envd.emplace_back(str);
    }
    for (const auto &e: envd) {
        envp.push_back(e.c_str());
    }
    envp.push_back(nullptr);

    auto argvptr = (char *const *) argv.data();
    auto envptr = (char *const *) envp.data();
    execve(binstr.c_str(), argvptr, envptr);
    throw SystemExecException();
}
