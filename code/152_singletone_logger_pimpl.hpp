#pragma once

struct LoggerImpl;

class Logger {
public:
    Logger();

    int& get();
    void LogInfo(std::string & message);

private:
    static LoggerImpl& impl();
    LoggerImpl& impl_;
};
