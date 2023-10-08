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
    // Style
    std::string s_reset = "\033[0m";
    std::string s_bold = "\033[1m";
    std::string s_dim = "\033[02m";
    std::string s_italic = "\033[3m";
    std::string s_underline = "\033[4m";

    // Foreground
    std::string f_black = "\033[30m";
    std::string f_red = "\033[31m";
    std::string f_green = "\033[32m";
    std::string f_yellow = "\033[33m";
    std::string f_blue = "\033[34m";
    std::string f_magenta = "\033[35m";
    std::string f_cyan = "\033[36m";
    std::string f_white = "\033[37m";

    // Background
    std::string b_black = "\033[40m";
    std::string b_red = "\033[41m";
    std::string b_green = "\033[42m";
    std::string b_yellow = "\033[43m";
    std::string b_blue = "\033[44m";
    std::string b_magenta = "\033[45m";
    std::string b_cyan = "\033[46m";
    std::string b_white = "\033[47m";
    
    std::string cl_line = "├─";
    std::string cr_line = "─┤";
    std::string ul_line = "┌─";
    std::string ur_line = "─┐";
    std::string ll_line = "└─";
    std::string lr_line = "─┘";

    std::string check = f_green + "✔" + s_reset;
    std::string cross = f_red + "✘" + s_reset;

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
        Wait_Success,
        Wait_Failed,
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

        Logger& wait_success();

        Logger& wait_failed();

        ~Logger();
    };
}


#endif