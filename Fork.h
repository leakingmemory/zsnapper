//
// Created by sigsegv on 7/7/24.
//

#ifndef ZSNAPPER_FORK_H
#define ZSNAPPER_FORK_H

extern "C" {
#include <sys/types.h>
};
#include <functional>
#include <string>
#include <ostream>

enum class ForkInputOutput {
    NONE,
    INPUT,
    INPUTOUTPUT
};

class Fork {
private:
    pid_t pid;
    int stdinpipe;
    int stdoutpipe;
    int result;
    bool waited;
public:
    Fork();
    Fork(const std::function<int ()> &func, ForkInputOutput inputOutput = ForkInputOutput::NONE);
    Fork(const Fork &) = delete;
    Fork(Fork &&);
    Fork &operator =(const Fork &) = delete;
    Fork &operator =(Fork &&);
    void Swap(Fork &other);
    ssize_t Read(void *buf, size_t count) const;
    ssize_t Write(const void *buf, size_t count) const;
    void CloseInput();
    void Wait();
    void Require();
    ~Fork();

    Fork &operator << (const std::string &str);
    const Fork &operator >> (std::basic_ostream<char> &ostream) const;
    const Fork &operator << (std::basic_istream<char> &istream) const;
    const Fork &operator << (const Fork &other) const;
};

#endif //ZSNAPPER_FORK_H
