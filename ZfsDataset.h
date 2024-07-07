//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_ZFSDATASET_H
#define ZSNAPPER_ZFSDATASET_H

#include <string>

class ZfsDataset {
private:
    std::string name;
public:
    ZfsDataset() = default;
    ZfsDataset(const std::string &name) : name(name) {}
    std::string GetProperty(const std::string &property) const;
    std::string GetMountpoint() const;
};


#endif //ZSNAPPER_ZFSDATASET_H
