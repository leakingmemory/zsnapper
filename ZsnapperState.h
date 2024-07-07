//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_ZSNAPPERSTATE_H
#define ZSNAPPER_ZSNAPPERSTATE_H


#include "ZsnapperConfig.h"

class ZsnapperState {
private:
    ZsnapperConfig config;
    std::time_t nextDaily{0};
public:
    ZsnapperState() = delete;
    ZsnapperState(const ZsnapperConfig &);
private:
    void NextDailyTimer();
public:
    void DumpConf() const;
    bool IsSane() const;
    bool IsContinous() const;
    std::time_t GetWaitSeconds();
    void RunPotentialIteration();
private:
    void RunDaily(const std::string &iterationName);
};


#endif //ZSNAPPER_ZSNAPPERSTATE_H
