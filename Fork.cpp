//
// Created by sigsegv on 7/7/24.
//

#include "Fork.h"
extern "C" {
#include <unistd.h>
#include <sys/wait.h>
}
#include <iostream>

class ForkException : public std::exception {
private:
    const char *error;
public:
    ForkException(const char *error) : error(error) {}
    const char * what() const noexcept override {
        return error;
    }
};

Fork::Fork() : pid(0), stdinpipe(-1), stdoutpipe(-1), result(0), waited(true) {
}

Fork::Fork(const std::function<int()> &func, ForkInputOutput forkInputOutput) : stdinpipe(-1), stdoutpipe(-1), result(0), waited(false) {
    int inpipefds[2];
    int outpipefds[2];
    if (forkInputOutput != ForkInputOutput::NONE) {
        if (pipe(inpipefds) != 0) {
            throw ForkException("Pipe failed (in,fork)");
        }
        if (forkInputOutput == ForkInputOutput::INPUTOUTPUT) {
            if (pipe(outpipefds) != 0) {
                throw ForkException("Pipe failed (out,fork)");
            }
        }
    }
    auto pid = fork();
    if (pid < 0) {
        throw ForkException("Fork failed");
    }
    if (pid == 0) {
        if (forkInputOutput != ForkInputOutput::NONE) {
            if (close(0) != 0) {
                throw ForkException("close(0) failed for piping");
            }
            if (dup2(inpipefds[0], 0) != 0) {
                throw ForkException("dup2(p, 0) failed for piping");
            }
            if (forkInputOutput == ForkInputOutput::INPUTOUTPUT) {
                if (close(1) != 0) {
                    throw ForkException("close(1) failed for piping");
                }
                if (dup2(outpipefds[1], 1) != 1) {
                    throw ForkException("dup2(p, 1) failed for piping");
                }
            }
            if (close(inpipefds[0]) != 0) {
                throw ForkException("close(p0) failed for piping");
            }
            if (close(inpipefds[1]) != 0) {
                throw ForkException("close(p1) failed for piping");
            }
            if (forkInputOutput == ForkInputOutput::INPUTOUTPUT) {
                if (close(outpipefds[0]) != 0) {
                    throw ForkException("close(p0) failed for piping");
                }
                if (close(outpipefds[1]) != 0) {
                    throw ForkException("close(p1) failed for piping");
                }
            }
        }
        try {
            auto res = func();
            exit(res);
        } catch (const std::exception &e) {
            const auto *what = e.what();
            std::cerr << "error: " << (what != nullptr ? what : "std::exception(what=nullptr)") << "\n";
            exit(1);
        }
    }
    if (forkInputOutput != ForkInputOutput::NONE) {
        if (close(inpipefds[0]) != 0) {
            throw ForkException("close(p) failed for piping");
        }
        stdinpipe = inpipefds[1];
        if (forkInputOutput == ForkInputOutput::INPUTOUTPUT) {
            if (close(outpipefds[1]) != 0) {
                throw ForkException("close(p) failed for piping");
            }
            stdoutpipe = outpipefds[0];
        }
    }
    this->pid = pid;
}

Fork::Fork(Fork &&mv) :
        pid(mv.pid),
        stdinpipe(mv.stdinpipe),
        stdoutpipe(mv.stdoutpipe),
        result(mv.result),
        waited(mv.waited)
{
    mv.stdinpipe = -1;
    mv.stdoutpipe = -1;
    mv.waited = true;
}

Fork &Fork::operator=(Fork &&mv) {
    Fork sw{std::move(mv)};
    Swap(sw);
    return *this;
}

void Fork::Swap(Fork &other) {
    auto pid = other.pid;
    auto stdinpipe = other.stdinpipe;
    auto stdoutpipe = other.stdoutpipe;
    auto result = other.result;
    auto waited = other.waited;
    other.pid = this->pid;
    other.stdinpipe = this->stdinpipe;
    other.stdoutpipe = this->stdoutpipe;
    other.result = this->result;
    other.waited = this->waited;
    this->pid = pid;
    this->stdinpipe = stdinpipe;
    this->stdoutpipe = stdoutpipe;
    this->result = result;
    this->waited = waited;
}

ssize_t Fork::Read(void *buf, size_t count) const {
    return read(stdoutpipe, buf, count);
}

ssize_t Fork::Write(const void *buf, size_t count) const {
    return write(stdinpipe, buf, count);
}

void Fork::CloseInput() {
    if (stdinpipe != -1 && close(stdinpipe) != 0) {
        stdinpipe = -1;
        throw ForkException("Closing stdin failed");
    }
    stdinpipe = -1;
}

void Fork::Wait() {
    if (!waited) {
        auto res = waitpid(pid, &result, 0);
        if (res < 0) {
            std::cerr << "waitpid: error\n";
            result = -1;
        }
        waited = true;
    }
}

void Fork::Require() {
    Wait();
    if (result) {
        throw ForkException("Error: Failure");
    }
}

Fork::~Fork() {
    if (stdinpipe != -1 && close(stdinpipe) != 0) {
        std::cerr << "close pipe failed (fork)\n";
    }
    stdinpipe = -1;
    if (stdoutpipe != -1 && close(stdoutpipe) != 0) {
        std::cerr << "close pipe failed (fork)\n";
    }
    stdoutpipe = -1;
    if (!waited) {
        Wait();
        if (result) {
            std::cerr << "Error: Failure, but not handled\n";
        }
    }
}

Fork &Fork::operator<<(const std::string &str) {
    if (Write(str.c_str(), str.length()) != str.length()) {
        throw ForkException("Write error (fork)");
    }
    return *this;
}

const Fork &Fork::operator>>(std::basic_ostream<char> &ostream) const {
    while (true) {
        char buf[4096];
        auto res = Read(buf, sizeof(buf));
        if (res > 0) {
            ostream.write(buf, res);
        }
        if (res <= 0) {
            break;
        }
    }
    return *this;
}

const Fork &Fork::operator<<(std::basic_istream<char> &istream) const {
    while (true) {
        char buf[4096];
        istream.read(buf, sizeof(buf));
        if (istream) {
            Write(buf, sizeof(buf));
        } else {
            auto sz = istream.gcount();
            if (sz > 0) {
                Write(buf, sz);
            }
            break;
        }
    }
    return *this;
}

const Fork &Fork::operator<<(const Fork &other) const {
    while (true) {
        char buf[4096];
        auto res = other.Read(buf, sizeof(buf));
        if (res > 0) {
            Write(buf, res);
        }
        if (res <= 0) {
            break;
        }
    }
    return *this;
}
