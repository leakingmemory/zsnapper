//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_EXECGETOUTPUT_H
#define ZSNAPPER_EXECGETOUTPUT_H

#include <string>
#include <vector>

class ExecGetOutput {
private:
    std::string output;
public:
    ExecGetOutput(const std::string &cmd, const std::vector<std::string> &args);
    [[nodiscard]] std::string GetOutput() const {
        return output;
    }
};


#endif //ZSNAPPER_EXECGETOUTPUT_H
