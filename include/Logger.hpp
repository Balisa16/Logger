#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>    
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <boost/filesystem.hpp>

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
    uint16_t info_msg, warn_msg, err_msg;
    bool combo_msg;
public:
    Logger(std::string filename, FileType type = FileType::TXT);
    Logger();
    void init(std::string filename, FileType type = FileType::TXT);
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
    char stop_str[100];
    std::strftime(stop_str, sizeof(stop_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    std::chrono::duration<double> elapsed_seconds = stop_time - start_time;

    std::cout << "\n\nFlight Resume :\n\tStart Time\t: " << start_str << 
        "\n\tStop Time\t: " << stop_str << 
        "\n\tFlight Duration\t: " << std::to_string(elapsed_seconds.count()) << " seconds\nErrors\t\t: " << err_msg << 
        " message\nWarnings\t: " << warn_msg <<
        " message\nInformations\t: " << info_msg << " message\n";
}

Logger::Logger(std::string filename, FileType type)
{
    init(filename, type);
}

inline Logger::Logger()
{

}

inline void Logger::init(std::string filename, FileType type)
{
    struct passwd *pw = getpwuid(getuid());
    const char *home_char = pw->pw_dir;
    std::string home_dir = home_char;
    std::string flight_dir = "Flight Log";
    std::string temp_1 = home_dir + "/" + flight_dir;

    bool is_exist = boost::filesystem::exists(temp_1);

    if(!is_exist){
        std::cout << "Create folder " + flight_dir + " in " + home_dir << std::endl;
        temp_1 = "mkdir " + home_dir + "/'" + flight_dir +"'";
        system(temp_1.c_str());
    }

    // Set file type
    this->type = type;

    std::string file_ext = ".csv";
    if(type == FileType::TXT)
        file_ext = ".txt";

    std::string temp_filedir = home_dir + "/'" + flight_dir +"'/";
    
    temp_1 = temp_filedir + filename + file_ext;
    if(boost::filesystem::exists(temp_1.c_str()))
    {
        std::cout << "File : " << temp_1 << " already exist." << std::endl;
        int file_idx = 1;
        temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
        while (boost::filesystem::exists(temp_1.c_str()))
        {
            file_idx++;
            temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
        }
    }
    
    this->full_filename = "/home/a/Flight Log/test2.csv";
    std::cout << "Log file stored in : " << temp_1 << std::endl;
    info_msg = 0;
    warn_msg = 0;
    err_msg = 0;
    combo_msg = false;
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
    switch (level)
    {
    case LogLevel::INFO:
        info_msg++;
        break;
    case LogLevel::WARNING:
        warn_msg++;
        break;
    case LogLevel::ERROR:
        err_msg++;
        break;
    default:
        break;
    }
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

    if(!combo_msg)
        switch (level)
        {
        case LogLevel::INFO:
            info_msg++;
            break;
        case LogLevel::WARNING:
            warn_msg++;
            break;
        case LogLevel::ERROR:
            err_msg++;
            break;
        default:
            break;
        }
}

inline void Logger::write_show(std::string msg, LogLevel level)
{
    combo_msg = true;
    write(msg, level);
    show(msg, level);
    combo_msg = false;
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
