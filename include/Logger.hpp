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
    uint64_t line_counter;
    LogLevel level;
    FileType type;
    std::ofstream writer;
    void resume();
public:
    Logger(std::string filename, FileType type = FileType::TXT);
    void start();
    void write(std::string msg, LogLevel level = LogLevel::INFO);
    void show(std::string msg, LogLevel level = LogLevel::INFO);
    void write_show(std::string msg, LogLevel level = LogLevel::INFO);
    void finish();
    std::string getLvl(LogLevel lvl = LogLevel::INFO);
    ~Logger();
};

inline void Logger::resume()
{
    std::time_t time = std::chrono::system_clock::to_time_t(start_time);
    char start_str[100];
    std::strftime(start_str, sizeof(start_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    time = std::chrono::system_clock::to_time_t(stop_time);
    char start_str[100];
    std::strftime(start_str, sizeof(start_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    std::cout << "\n\nFlight Resume :\nStart Time\t: " << 
}

Logger::Logger(std::string filename, FileType type)
{
    this->type = type;
    switch (type)
    {
    case FileType::TXT:
        full_filename = filename + ".txt";
        break;
    case FileType::CSV:
        full_filename = filename + ".csv";
        break;
    }
}

inline void Logger::start()
{
    start_time = std::chrono::system_clock::now();
    writer.open(this->full_filename);
    if(type == FileType::CSV)
        writer << "Level,Id,Datetime,Flight Time(s),Message\n";
    line_counter = 0;
}

inline std::string Logger::getLvl(LogLevel lvl)
{
    std::string lvl_string = "[";
    switch (lvl)
    {
    case LogLevel::INFO:
        lvl_string += "INFO ";
        break;
    case LogLevel::WARNING:
        lvl_string += "WARN ";
        break;
    case LogLevel::ERROR:
        lvl_string += "ERROR";
        break;
    
    default:
        break;
    }
    lvl_string += "]";
    return lvl_string;
}

inline void Logger::write(std::string msg, LogLevel level)
{
    line_counter++;
    std::string header = getLvl(level);
    std::time_t currentTime = std::time(nullptr);
    char timeString[100];
    std::string separator = "   ";
    if(type == FileType::CSV)
        separator = ',';
    header += separator;
    header += std::to_string(line_counter);
    header += separator;
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    header += timeString;
    header += separator;
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;

    header += std::to_string(elapsed_seconds.count());
    header += separator + msg;
    writer << header << '\n';
}

inline void Logger::show(std::string msg, LogLevel level)
{
    std::string header = getLvl(level);
    std::time_t currentTime = std::time(nullptr);
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%H:%M:%S", std::localtime(&currentTime));
    // std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    header += ' ';
    header += timeString;
    header += ' ';


    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;
    int int_duration = elapsed_seconds.count();
    header += std::to_string(int_duration);
    header += "s : " + msg;

    std::cout << header << '\n';
}

inline void Logger::write_show(std::string msg, LogLevel level)
{
    write(msg, level);
    show(msg, level);
}

inline void Logger::finish()
{
    stop_time = std::chrono::system_clock::now();
    writer.close();
    resume();
}

Logger::~Logger()
{
}

#endif
