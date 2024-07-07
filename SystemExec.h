//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_SYSTEMEXEC_H
#define ZSNAPPER_SYSTEMEXEC_H

#include <vector>
#include <map>

class SystemExecException : public std::exception {
};

class SystemExec {
private:
    std::string cmd;
    std::vector<std::string> args{};
public:
    SystemExec(const std::string &cmd, const std::vector<std::string> &args) : cmd(cmd), args(args) {}
    static std::map<std::string,std::string> GetEnv();
    void Exec();
};


#endif //ZSNAPPER_SYSTEMEXEC_H
