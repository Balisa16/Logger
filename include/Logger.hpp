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
#include <stdio.h>
#include <stdarg.h>
#include <mutex>
#include <thread>
#include <exception>
#include <vector>

namespace EMIRO
{   
    const char cl_line[8] = "├─";
    const char cr_line[8] = "─┤";
    const char ul_line[8] = "┌─";
    const char ur_line[8] = "─┐";
    const char ll_line[8] = "└─";
    const char lr_line[8] = "─┘";

    enum class LogLevel {
        INFO,
        WARNING,
        ERROR
    };

    enum class FileType {
        TXT,
        CSV
    };

    enum class LoggerStatus {
        None,
        Init,
        Run,
        Wait,
        Stop,
    };

    typedef struct{
        LoggerStatus status;
        std::mutex mtx;
        std::chrono::_V2::system_clock::time_point start_time;
        std::string message;
    }LoggerFormat;

    template <typename T>
    struct BranchItem{
        std::string row_header;
        T value;
        std::string unit;
    };

    class Logger
    {
    private:
        std::chrono::_V2::system_clock::time_point start_time, stop_time;
        std::string full_filename;
        uint64_t line_counter;

        LoggerFormat log_fmt;
        LogLevel level;
        FileType type;

        std::ofstream writer;
        uint16_t info_msg, warn_msg, err_msg;

        void resume();
        std::string getLvl(LogLevel lvl = LogLevel::INFO, bool no_color = false);
        std::string cust_printf(const char *format, va_list args);
        void unavailable_msg();
        void update_counter(LogLevel level);

    public:
        Logger(std::string filename, FileType type = FileType::TXT);
        Logger();

        /**
         * @brief Initialize filename and type of file
         * 
         * @param filename  Filename
         * @param type      Type of file CSV or TXT
         */
        void init(std::string filename, FileType type = FileType::TXT);

        /**
         * @brief Starting Logger System
         * 
         */
        void start(bool reset_prev_counter = false);

        /**
         * @brief Write message into file without showing message into terminal
         * 
         * @param level     Message Level (ERROR, WARNING, INFO).
         * @param format    Message format
         * @param ...       Additional of message variable
         * 
         * @note Message format same as printf in C.
         */
        void write(LogLevel level, const char *format, ...);

        /**
         * @brief           Just showing message into terminal without write in logger file.
         * 
         * @param level     Message Level (ERROR, WARNING, INFO).
         * @param format    Message format
         * @param ...       Additional of message variable
         * 
         * @note Message format same as printf in C.
         */
        void show(LogLevel level, const char *format, ...);

        /**
         * @brief           Write message into logger file and show up message into terminal.
         * 
         * @param level     Message Level (ERROR, WARNING, INFO).
         * @param format    Message format
         * @param ...       Additional of message variable
         * 
         * @note Message format same as printf in C.
         */
        void write_show(LogLevel level, const char *format, ...);

        void branch_show(std::string header, std::vector<BranchItem<float>> items);

        /**
         * @brief Finished Logger System.
         * 
         */
        void finish();

        LoggerStatus get_status();

        Logger& wait(std::string wait_msg);

        Logger& wait_stop();

        ~Logger();
    };
}


#endif