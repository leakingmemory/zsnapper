#include <iostream>
#include "ZsnapperConfig.h"
#include "ZsnapperLoop.h"

int main(int argc, const char * const * const argv) {
    ZsnapperConfig config{};
    if (argc > 1) {
        if (argc > 2) {
            std::cerr << "Usage: " << argv[0] << " [<config>]\n";
            return 1;
        }
        config = ZsnapperConfig(argv[1]);
    }
    ZsnapperLoop loop{config};
    loop.Run();
    return 0;
}
