//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_ZSNAPPERLOOP_H
#define ZSNAPPER_ZSNAPPERLOOP_H

#include "ZsnapperState.h"

class ZsnapperLoop {
private:
    ZsnapperState state;
public:
    ZsnapperLoop() = delete;
    ZsnapperLoop(const ZsnapperConfig &config);
    void Run();
};


#endif //ZSNAPPER_ZSNAPPERLOOP_H
