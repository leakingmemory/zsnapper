//
// Created by sigsegv on 7/7/24.
//

#include "ZsnapperConfig.h"
#include <nlohmann/json.hpp>
#include <fstream>

class ZsnapperConfigData {
private:
    friend ZsnapperConfig;
    ZsnapperType type{ZsnapperType::Zfs};
    int hours{0};
    int minutes{0};
    std::vector<std::string> daily{};
    std::string targetDirectory{};
public:
    ZsnapperConfigData();
    ZsnapperConfigData(const std::string &filename);
};

ZsnapperConfigData::ZsnapperConfigData() {}
ZsnapperConfigData::ZsnapperConfigData(const std::string &filename) {
    nlohmann::json jf{};
    {
        std::ifstream ifs(filename);
        jf = nlohmann::json::parse(ifs);
    }
    if (jf.is_object()) {
        if (jf.contains("type")) {
            auto typeObj = jf.at("type");
            if (typeObj.is_string()) {
                auto str = typeObj.get<std::string>();
                if (str == "ZFS") {
                    type = ZsnapperType::Zfs;
                }
            }
        }
        if (jf.contains("hours")) {
            auto hoursObj = jf.at("hours");
            if (hoursObj.is_number()) {
                hours = hoursObj.get<int>();
            }
        }
        if (jf.contains("minutes")) {
            auto minutesObj = jf.at("minutes");
            if (minutesObj.is_number()) {
                minutes = minutesObj.get<int>();
            }
        }
        if (jf.contains("daily")) {
            auto dailyObj = jf.at("daily");
            if (dailyObj.is_array()) {
                for (const auto &item : dailyObj.items()) {
                    if (item.value().is_string()) {
                        daily.emplace_back(item.value().get<std::string>());
                    }
                }
            }
        }
        if (jf.contains("targetDirectory")) {
            auto targetObj = jf.at("targetDirectory");
            if (targetObj.is_string()) {
                targetDirectory = targetObj.get<std::string>();
            }
        }
    }
}

ZsnapperConfig::ZsnapperConfig() : data(std::make_shared<ZsnapperConfigData>()) {
}

ZsnapperConfig::ZsnapperConfig(const std::string &filename) : data(std::make_shared<ZsnapperConfigData>(filename)) {}

ZsnapperType ZsnapperConfig::GetType() const {
    return data->type;
}

int ZsnapperConfig::GetHours() const {
    return data->hours;
}

int ZsnapperConfig::GetMinutes() const {
    return data->minutes;
}

std::vector<std::string> ZsnapperConfig::GetDaily() const {
    return data->daily;
}

std::string ZsnapperConfig::GetTargetDirectory() const {
    return data->targetDirectory;
}
