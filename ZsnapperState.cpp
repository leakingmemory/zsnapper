//
// Created by sigsegv on 7/7/24.
//

#include "ZsnapperState.h"
#include "ZfsDataset.h"
#include "ZfsSnapshot.h"
#include "SimpleExec.h"
#include <iostream>
#include <ctime>
#include <time.h>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include "Fork.h"

std::time_t GetNextDailyRun(const ZsnapperConfig &config) {
    std::time_t nowTm = std::time(nullptr);
    std::tm t{};
    if (localtime_r(&nowTm, &t) != &t) {
        throw std::exception();
    }
    t.tm_hour = config.GetHours();
    t.tm_min = config.GetMinutes();
    t.tm_sec = 0;
    std::time_t backupTm = mktime(&t);
    while (backupTm <= nowTm) {
        backupTm += 24 * 3600;
    }
    return backupTm;
}

ZsnapperState::ZsnapperState(const ZsnapperConfig &config) : config(config) {
    NextDailyTimer();
}

void ZsnapperState::NextDailyTimer() {
    nextDaily = GetNextDailyRun(config);
    std::tm t{};
    if (localtime_r(&nextDaily, &t) != &t) {
        throw std::exception();
    }
    std::cout << "Next daily run at " << (t.tm_year + 1900) << "-" << (t.tm_mon < 9 ? "0" : "") << (t.tm_mon + 1) << "-" << (t.tm_mday < 10 ? "0" : "") << t.tm_mday << " " << (t.tm_hour < 10 ? "0" : "") << t.tm_hour << ":" << (t.tm_min < 10 ? "0" : "") << t.tm_min << "\n";
}

std::time_t ZsnapperState::GetWaitSeconds() {
    std::time_t nowTm = std::time(nullptr);
    if (nextDaily > nowTm) {
        return nextDaily - nowTm;
    }
    return 0;
}

bool ZsnapperState::IsContinous() const {
    return false;
}

bool ZsnapperState::IsSane() const {
    if (config.GetDaily().empty()) {
        std::cout << "Specify backup sources (daily)\n";
        return false;
    }
    if (config.GetTargetDirectory().empty()) {
        std::cout << "Specify target directory\n";
        return false;
    }
    return true;
}

void ZsnapperState::DumpConf() const {
    std::cout << "Hours: " << config.GetHours() << "\n";
    std::cout << "Minutes: " << config.GetMinutes() << "\n";
    std::cout << "Daily sources:\n";
    for (const auto &src : config.GetDaily()) {
        std::cout << " " << src << "\n";
    }
    std::cout << "Target dir: " << config.GetTargetDirectory() << "\n";
}

void ZsnapperState::RunPotentialIteration() {
    std::time_t nowTm = std::time(nullptr);
    if (nowTm >= nextDaily) {
        std::tm t{};
        if (localtime_r(&nextDaily, &t) != &t) {
            throw std::exception();
        }
        std::string iterationName{};
        {
            std::stringstream str{};
            str << (t.tm_year + 1900) << "-" << (t.tm_mon < 9 ? "0" : "") << (t.tm_mon + 1) << "-" << (t.tm_mday < 10 ? "0" : "") << t.tm_mday << "T" << (t.tm_hour < 10 ? "0" : "") << t.tm_hour << ":" << (t.tm_min < 10 ? "0" : "") << t.tm_min;
            iterationName = str.str();
        }
        NextDailyTimer();
        if (iterationName.empty()) {
            std::cerr << "Error: Failed to generate iteration name\n";
            return;
        }
        RunDaily(iterationName);
    }
}

void ZsnapperState::RunDaily(const std::string &iterationName) {
    std::filesystem::path targetBackupDirectory = config.GetTargetDirectory();
    if (!std::filesystem::exists(targetBackupDirectory) || !std::filesystem::is_directory(targetBackupDirectory)) {
        std::cerr << "Target directory does not exist or is not a directory (" << targetBackupDirectory << ")\n";
        return;
    }
    targetBackupDirectory = targetBackupDirectory / "daily";
    if (!std::filesystem::exists(targetBackupDirectory)) {
        std::filesystem::create_directory(targetBackupDirectory);
    }
    if (!std::filesystem::exists(targetBackupDirectory) || !std::filesystem::is_directory(targetBackupDirectory)) {
        std::cerr << "Target daily directory does not exist or is not a directory (" << targetBackupDirectory << ")\n";
        return;
    }
    std::filesystem::path targetDirectory = targetBackupDirectory / iterationName;
    if (std::filesystem::exists(targetDirectory)) {
        std::cerr << "Iteration " << iterationName << " is already present in target directory\n";
        return;
    }
    std::filesystem::create_directory(targetDirectory);
    if (!std::filesystem::exists(targetDirectory)) {
        std::cerr << "Iteration directory " << iterationName << " was not created in target path\n";
        return;
    }
    if (config.GetType() == ZsnapperType::Zfs) {
        std::vector<ZfsSnapshot> snapshots{};
        for (const auto &src : config.GetDaily()) {
            try {
                snapshots.emplace_back(src, "backupsnap");
            } catch (std::exception &e) {
                std::cerr << "Failed to create snapshot of " << src << ": " << e.what() << "\n";
            } catch (...) {
                std::cerr << "Failed to create snapshot of " << src << "\n";
            }
        }
        for (const auto &snapshot : snapshots) {
            if (snapshot.GetSnapshotName().empty() || snapshot.GetSnapshorDirectory().empty()) {
                continue;
            }
            try {
                auto filename = snapshot.GetSnapshotName();
                std::transform(filename.cbegin(), filename.cend(), filename.begin(), [] (char ch) { return ch == '/' ? '_' : ch; });
                filename.append(".tar.bz2");
                auto dir = snapshot.GetSnapshorDirectory();
                std::filesystem::path path = targetDirectory / filename;
                std::cout << " ==> " << dir << " to " << path << "\n";
                Fork fork{[dir, path]() {
                    if (chdir(dir.c_str()) != 0) {
                        std::cerr << "Failed to chdir: " << dir << "\n";
                        return 1;
                    }
                    SimpleExec tarExec{"/usr/bin/tar", {"-cjvf", path, "."}};
                    return 0;
                }, ForkInputOutput::NONE};
                fork.Require();
            } catch (std::exception &e) {
                std::cerr << "Failed " << snapshot.GetSnapshotName() << ": " << e.what() << "\n";
            } catch (...) {
                std::cerr << "Failed " << snapshot.GetSnapshotName() << "\n";
            }
        }
    }
}
