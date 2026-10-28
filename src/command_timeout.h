#ifndef GHIDRADEC_COMMAND_TIMEOUT_H
#define GHIDRADEC_COMMAND_TIMEOUT_H

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <thread>

// The caller services protocol callbacks; only process interruption is off-thread.
class CommandDeadline
{
    std::promise<void> completed;
    std::atomic<bool> expired{false};
    std::thread watchdog;

public:
    CommandDeadline(std::chrono::milliseconds timeout, std::function<void()> interrupt)
        : watchdog([this, timeout, interrupt, done = completed.get_future()]() mutable {
            if (done.wait_for(timeout) == std::future_status::timeout) {
                expired = true;
                try {
                    interrupt();
                } catch (...) {
                    // A watchdog must not throw across the thread boundary.
                }
            }
        })
    {
    }

    bool finish()
    {
        if (watchdog.joinable()) {
            completed.set_value();
            watchdog.join();
        }
        return expired;
    }

    ~CommandDeadline()
    {
        finish();
    }
};

#endif
