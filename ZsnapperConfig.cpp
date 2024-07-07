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
    std::vector<ZUpload> uploads{};
    std::vector<ZImport> imports{};
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
        if (jf.contains("uploads")) {
            auto uploadsObj = jf.at("uploads");
            if (uploadsObj.is_array()) {
                for (const auto &uploadItem : uploadsObj.items()) {
                    const auto &uploadObj = uploadItem.value();
                    if (!uploadObj.is_object()) {
                        continue;
                    }
                    ZUpload upload{};
                    if (uploadObj.contains("ssh")) {
                        auto str = uploadObj.at("ssh");
                        if (str.is_string()) {
                            upload.ssh = str;
                        }
                    }
                    if (uploadObj.contains("uploadDirectory")) {
                        auto str = uploadObj.at("uploadDirectory");
                        if (str.is_string()) {
                            upload.uploadDir = str;
                        }
                    }
                    if (uploadObj.contains("before")) {
                        auto arr = uploadObj.at("before");
                        if (arr.is_array()) {
                            std::vector<std::vector<std::string>> &vec = upload.before;
                            for (const auto &beforeItem : arr.items()) {
                                const auto &beforeObj = beforeItem.value();
                                if (!beforeObj.is_array()) {
                                    continue;
                                }
                                bool valid{true};
                                std::vector<std::string> cmd{};
                                for (const auto &cmdItem : beforeObj.items()) {
                                    const auto &str = cmdItem.value();
                                    if (!str.is_string()) {
                                        valid = false;
                                        break;
                                    }
                                    cmd.emplace_back(str);
                                }
                                if (valid) {
                                    vec.emplace_back(cmd);
                                }
                            }
                        }
                    }
                    if (uploadObj.contains("after")) {
                        auto arr = uploadObj.at("after");
                        if (arr.is_array()) {
                            std::vector<std::vector<std::string>> &vec = upload.after;
                            for (const auto &beforeItem : arr.items()) {
                                const auto &beforeObj = beforeItem.value();
                                if (!beforeObj.is_array()) {
                                    continue;
                                }
                                bool valid{true};
                                std::vector<std::string> cmd{};
                                for (const auto &cmdItem : beforeObj.items()) {
                                    const auto &str = cmdItem.value();
                                    if (!str.is_string()) {
                                        valid = false;
                                        break;
                                    }
                                    cmd.emplace_back(str);
                                }
                                if (valid) {
                                    vec.emplace_back(cmd);
                                }
                            }
                        }
                    }
                    uploads.emplace_back(upload);
                }
            }
        }
        if (jf.contains("imports")) {
            auto importsObj = jf.at("imports");
            if (importsObj.is_array()) {
                for (const auto &importItem: importsObj.items()) {
                    const auto &importObj = importItem.value();
                    if (!importObj.is_object()) {
                        continue;
                    }
                    ZImport imp{};
                    if (importObj.contains("fromDirectory")) {
                        auto str = importObj.at("fromDirectory");
                        if (str.is_string()) {
                            imp.importFromDirectory = str;
                        }
                    }
                    if (importObj.contains("targetDirectory")) {
                        auto str = importObj.at("targetDirectory");
                        if (str.is_string()) {
                            imp.targetDirectory = str;
                        }
                    }
                    if (importObj.contains("targetUid")) {
                        auto uid = importObj.at("targetUid");
                        if (uid.is_string()) {
                            imp.targetUid = uid;
                        }
                    }
                    if (importObj.contains("targetGid")) {
                        auto uid = importObj.at("targetGid");
                        if (uid.is_string()) {
                            imp.targetGid = uid;
                        }
                    }
                    imports.emplace_back(imp);
                }
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

std::vector<ZUpload> ZsnapperConfig::GetUploads() const {
    return data->uploads;
}

std::vector<ZImport> ZsnapperConfig::GetImports() const {
    return data->imports;
}
