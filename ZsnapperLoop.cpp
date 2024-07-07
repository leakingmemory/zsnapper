//
// Created by sigsegv on 7/7/24.
//

#include "ZsnapperLoop.h"
#include "ZsnapperConfig.h"
#include <thread>

ZsnapperLoop::ZsnapperLoop(const ZsnapperConfig &config) : state(config) {}

void ZsnapperLoop::Run() {
    state.DumpConf();
    if (!state.IsSane()) {
        return;
    }
    while (true) {
        if (!state.IsContinous()) {
            auto seconds = state.GetWaitSeconds();
            if (seconds > 0) {
                if (seconds <= 2) {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(500ms);
                } else if (seconds < 70) {
                    if (seconds > 20) {
                        std::this_thread::sleep_for(std::chrono::seconds(seconds - 10));
                    } else {
                        std::this_thread::sleep_for(std::chrono::seconds(seconds - 2));
                    }
                } else if (seconds < 600) {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(60s);
                } else {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(300s);
                }
            }
        }
        state.RunPotentialIteration();
    }
}