#pragma once

#include <chrono>
#include <string>
#include <functional>

class TimingScope {
public:
    using Clock = std::chrono::steady_clock;

    TimingScope(
            std::string name,
            std::function<void(const std::string&, long)> onFinish
    )
            : name_(std::move(name)),
              onFinish_(std::move(onFinish)),
              start_(Clock::now()) {}

    ~TimingScope() {
        auto end = Clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
        onFinish_(name_, ms);
    }

private:
    std::string name_;
    std::function<void(const std::string&, long)> onFinish_;
    Clock::time_point start_;
};