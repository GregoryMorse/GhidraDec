#include "command_timeout.h"
#include <stdexcept>

int main()
{
    bool cancelled = false;
    {
        CommandDeadline deadline(std::chrono::seconds(5), [&]() { cancelled = true; });
        if (deadline.finish() || cancelled)
            return 1;
    }
    std::promise<void> interrupted;
    auto read = interrupted.get_future();
    const auto caller = std::this_thread::get_id();
    std::thread::id watchdog;
    CommandDeadline deadline(std::chrono::milliseconds(10), [&]() {
        watchdog = std::this_thread::get_id();
        interrupted.set_value();
    });
    // Model an interruptible blocking read on the protocol's calling thread.
    read.wait();
    if (std::this_thread::get_id() != caller || !deadline.finish() || watchdog == caller)
        return 2;
    try {
        CommandDeadline onException(std::chrono::seconds(5), [&]() { cancelled = true; });
        throw std::runtime_error("simulated protocol failure");
    } catch (const std::runtime_error&) {
    }
    return cancelled ? 3 : 0;
}
