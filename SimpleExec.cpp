//
// Created by sigsegv on 7/7/24.
//

#include "SimpleExec.h"
#include "Fork.h"
#include "SystemExec.h"

SimpleExec::SimpleExec(const std::string &cmd, const std::vector<std::string> &args) {
    Fork fork{[cmd, args] () {
        SystemExec systemExec{cmd, args};
        systemExec.Exec();
        return 0;
    }, ForkInputOutput::NONE};
    fork.Require();
}