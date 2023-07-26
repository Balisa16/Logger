#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>    


enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

enum class FileType {
    TXT,
    CSV
};

class Logger
{
private:
    std::chrono::_V2::system_clock::time_point start_time, stop_time;
    std::string full_filename;
    LogLevel level;
    FileType type;
    void resume();
public:
    Logger(std::string filename, FileType type = FileType::TXT);
    void write(std::string msg, LogLevel level = LogLevel::INFO);
    void show(std::string msg, LogLevel level = LogLevel::INFO);
    void write_show(std::string msg, LogLevel level = LogLevel::INFO);
    void finish();
    ~Logger();
};

Logger::Logger(std::string filename, FileType type)
{
    this->type = type;
    switch (type)
    {
    case FileType::TXT:
        /* code */
        break;
    case FileType::CSV:

        break;
    }
    start_time = std::chrono::system_clock::now();
}

Logger::~Logger()
{
}

#endif
