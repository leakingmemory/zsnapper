//
// Created by sigsegv on 7/7/24.
//

#include "ExecGetOutput.h"
#include <sstream>
#include "Fork.h"
#include "SystemExec.h"

ExecGetOutput::ExecGetOutput(const std::string &cmd, const std::vector<std::string> &args) {
    Fork fork{[cmd, args] () {
        SystemExec systemExec{cmd, args};
        systemExec.Exec();
        return 0;
    }, ForkInputOutput::INPUTOUTPUT};
    fork.CloseInput();
    std::stringstream str{};
    fork >> str;
    output = str.str();
    fork.Require();
}
