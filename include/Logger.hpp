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
#include <termios.h>

namespace EMIRO
{
    // Style
    extern std::string s_reset;
    extern std::string s_bold;
    extern std::string s_dim;
    extern std::string s_italic;
    extern std::string s_underline;

    // Foreground
    extern std::string f_black;
    extern std::string f_red;
    extern std::string f_green;
    extern std::string f_yellow;
    extern std::string f_blue;
    extern std::string f_magenta;
    extern std::string f_cyan;
    extern std::string f_white;

    // Background
    extern std::string b_black;
    extern std::string b_red;
    extern std::string b_green;
    extern std::string b_yellow;
    extern std::string b_blue;
    extern std::string b_magenta;
    extern std::string b_cyan;
    extern std::string b_white;

    extern std::string cl_line;
    extern std::string cr_line;
    extern std::string ul_line;
    extern std::string ur_line;
    extern std::string ll_line;
    extern std::string lr_line;

    extern std::string check;
    extern std::string cross;

    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        ASK
    };

    enum class FileType
    {
        TXT,
        CSV
    };

    enum class LoggerStatus
    {
        None,
        Init,
        Run,
        Wait,
        Wait_Success,
        Wait_Failed,
        Stop,
    };

    typedef struct
    {
        LoggerStatus status;
        std::mutex mtx;
        std::chrono::_V2::system_clock::time_point start_time;
        std::string message;
    } LoggerFormat;

    template <typename T>
    struct ListItem
    {
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
        FileType type;

        std::ofstream writer;
        uint16_t info_msg, warn_msg, err_msg;

        void resume();
        std::string getLvl(LogLevel lvl = LogLevel::INFO, bool no_color = false);
        std::string cust_printf(const char *format, va_list args);
        void unavailable_msg();
        void update_counter(LogLevel level);
        char get_hidden_char();

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

        bool ask(std::string question);

        template <typename T>
        void list_show(std::string header, std::vector<ListItem<T>> items);

        /**
         * @brief Finished Logger System.
         *
         */
        void finish(bool show = true);

        LoggerStatus get_status();

        Logger &wait(std::string wait_msg);

        Logger &wait_success();

        Logger &wait_failed();

        ~Logger();
    };
}

#endif