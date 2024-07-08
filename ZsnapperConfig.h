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

struct ZUpload {
    std::string ssh;
    std::string uploadDir;
    std::vector<std::vector<std::string>> before;
    std::vector<std::vector<std::string>> after;
};

struct ZImport {
    std::string importFromDirectory;
    std::string targetDirectory;
    std::string targetUid{"root"};
    std::string targetGid{"wheel"};
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
    [[nodiscard]] std::vector<ZUpload> GetUploads() const;
    [[nodiscard]] std::vector<ZImport> GetImports() const;
    [[nodiscard]] std::vector<std::string> GetApplyRetention() const;
    int GetRetainDaily() const;
    int GetConcurrency() const;
};


#endif //ZSNAPPER_ZSNAPPERCONFIG_H
