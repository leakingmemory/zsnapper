//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_ZSNAPPERCONFIG_H
#define ZSNAPPER_ZSNAPPERCONFIG_H

#include <vector>
#include <string>
#include <memory>

enum class ZsnapperType {
    Zfs
};

class ZsnapperConfigData;

class ZsnapperConfig {
private:
    std::shared_ptr<ZsnapperConfigData> data{};
public:
    ZsnapperConfig();
    ZsnapperConfig(const std::string &filename);
    ZsnapperType GetType() const;
    int GetHours() const;
    int GetMinutes() const;
    [[nodiscard]] std::vector<std::string> GetDaily() const;
    [[nodiscard]] std::string GetTargetDirectory() const;
};


#endif //ZSNAPPER_ZSNAPPERCONFIG_H
