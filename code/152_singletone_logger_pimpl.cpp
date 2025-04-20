#include <mutex>
#include <iostream>

#include "152_singletone_logger_pimpl.hpp"

struct LoggerImpl {
    LoggerImpl(): count_(0) {}

    void LogInfo(std::string & message) {
        std::lock_guard<std::mutex> guard(lock_);
        std::cout << "-- Info: " << message << std::endl;
    }

    std::mutex lock_;
    int count_;
};

Logger::Logger() : impl_(impl()) {}

LoggerImpl& Logger::impl() {
    static LoggerImpl instance;
    return instance;
}

int& Logger::get() {
    // return impl().count_;
    return impl_.count_;
}

void Logger::LogInfo(std::string & message) {
    // impl().LogInfo(message);
    return impl_.LogInfo(message);
}
