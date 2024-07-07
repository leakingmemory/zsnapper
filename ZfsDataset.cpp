//
// Created by sigsegv on 7/7/24.
//

#include "ZfsDataset.h"
#include "ExecGetOutput.h"

std::string ZfsDataset::GetProperty(const std::string &property) const {
    ExecGetOutput execGetOutput{"/sbin/zfs", {"get", "-H", "-o", "value", property, name}};
    std::string str{execGetOutput.GetOutput()};
    while (!str.empty() && (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r')) {
        str.resize(str.size() - 1);
    }
    return str;
}

std::string ZfsDataset::GetMountpoint() const {
    return GetProperty("mountpoint");
}