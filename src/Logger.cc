#include <Logger.hpp>

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

    void wait_thread(LoggerFormat *format)
    {
        std::lock_guard<std::mutex> lg(format->mtx);
        std::string _cap = f_magenta + "[WAIT ]" + s_reset;
        std::time_t _current_time = std::time(nullptr);
        char _time_char[100];
        std::strftime(_time_char, sizeof(_time_char), "%H:%M:%S", std::localtime(&_current_time));
        _cap += ' ';
        _cap += _time_char;
        _cap += ' ';

        std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - format->start_time;
        auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
        _cap += std::to_string(ms_elapsed.count() / 1000000.0f);

        _cap += "s : " + format->message + " ";

        std::chrono::_V2::system_clock::time_point start_wait = std::chrono::system_clock::now();

        std::cout << _cap;
        std::cout.flush();
        while (format->status == LoggerStatus::Wait)
        {
            std::cout << ".";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        t_elapsed = std::chrono::system_clock::now() - start_wait;
        ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);

        if (format->status == LoggerStatus::Wait_Success)
            std::cout << " (" << check;
        else
            std::cout << " (" << cross;
        std::cout << " ) " << std::to_string(ms_elapsed.count() / 1000000.0f) << " s\n";

        format->status = LoggerStatus::Run;
    }

    Logger::Logger()
    {
        log_fmt.status = LoggerStatus::None;
    }

    Logger::Logger(std::string filename, FileType type)
    {
        log_fmt.status = LoggerStatus::None;
        init(filename, type);
    }

    LoggerStatus Logger::get_status()
    {
        return log_fmt.status;
    }

    void Logger::update_counter(LogLevel level)
    {
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

    void Logger::resume()
    {
        std::time_t time = std::chrono::system_clock::to_time_t(log_fmt.start_time);
        char start_str[100];
        std::strftime(start_str, sizeof(start_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

        time = std::chrono::system_clock::to_time_t(stop_time);
        char stop_str[100];
        std::strftime(stop_str, sizeof(stop_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

        std::chrono::duration<double> elapsed_seconds = stop_time - log_fmt.start_time;

        std::cout << '\n'
                  << f_blue << s_bold << "Logger Resume :" << s_reset << "\n\tStart Time\t: " << start_str << "\n\tStop Time\t: " << stop_str << "\n\tDuration\t: " << std::to_string(elapsed_seconds.count()) << " seconds\nErrors\t\t: " << err_msg << " message\nWarnings\t: " << warn_msg << " message\nInformations\t: " << info_msg << " message\n\n";
    }

    void Logger::init(std::string filename, FileType type)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if (log_fmt.status == LoggerStatus::None)
        {
            struct passwd *pw = getpwuid(getuid());
            const char *home_char = pw->pw_dir;
            std::string home_dir = home_char;
            std::string flight_dir = "Logger";
            std::string temp_1 = home_dir + "/" + flight_dir;

            if (!boost::filesystem::exists(temp_1))
                if (!boost::filesystem::create_directory(temp_1))
                {
                    std::cout << f_red << s_bold << "ERROR\t:" << s_reset << "Failed to create \"" << temp_1 << "\"\n ";
                    throw;
                }

            // Set file type
            this->type = type;

            std::string file_ext = ".csv";
            if (type == FileType::TXT)
                file_ext = ".txt";

            std::string temp_filedir = home_dir + "/" + flight_dir + "/";

            temp_1 = temp_filedir + filename + file_ext;
            if (boost::filesystem::exists(temp_1.c_str()))
            {
                std::cout << f_blue << s_bold << "File\t: " << s_reset << temp_1 << f_green + s_italic << " [already exist]" << s_reset << std::endl;
                int file_idx = 1;
                temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
                while (boost::filesystem::exists(temp_1.c_str()))
                {
                    file_idx++;
                    temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
                }
            }

            this->full_filename = temp_1;
            std::cout << f_blue << s_bold << "Stored\t: " << s_reset << temp_1 << "\n\n";
            info_msg = 0;
            warn_msg = 0;
            err_msg = 0;
            log_fmt.status = LoggerStatus::Init;
        }
    }

    void Logger::start(bool reset_prev_counter)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if (log_fmt.status == LoggerStatus::Init || log_fmt.status == LoggerStatus::Stop)
        {
            if (reset_prev_counter)
            {
                warn_msg = 0;
                info_msg = 0;
                err_msg = 0;
            }
            log_fmt.start_time = std::chrono::system_clock::now();
            writer.open(this->full_filename);
            if (type == FileType::CSV)
                writer << "Level,Id,Date Time,Duration (s),Message\n";
            line_counter = 0;
            log_fmt.status = LoggerStatus::Run;
        }
        else
        {
            if (log_fmt.status == LoggerStatus::Run)
                std::cout << f_yellow << "Logger is already activated.";
            else if (log_fmt.status == LoggerStatus::None)
                std::cout << f_red << "Logger is not initialized.";
            else
                std::cout << f_yellow << "Logger in Waiting Mode.";
            std::cout << s_reset << '\n';
        }
    }

    std::string Logger::getLvl(LogLevel lvl, bool no_color)
    {
        std::string lvl_string = "";
        if (!no_color)
        {
            switch (lvl)
            {
            case LogLevel::INFO:
                lvl_string += f_green + "[INFO ";
                break;
            case LogLevel::WARNING:
                lvl_string += f_yellow + "[WARN ";
                break;
            case LogLevel::ERROR:
                lvl_string += f_red + "[ERROR";
                break;
            case LogLevel::ASK:
                lvl_string += f_cyan + "[ASK  ";
                break;
            default:
                break;
            }
            lvl_string += ']' + s_reset;
        }
        else
            switch (lvl)
            {
            case LogLevel::INFO:
                lvl_string += "INFO";
                break;
            case LogLevel::WARNING:
                lvl_string += "WARNING";
                break;
            case LogLevel::ERROR:
                lvl_string += "ERROR";
                break;
            case LogLevel::ASK:
                lvl_string += "ASK";
                break;
            default:
                break;
            }
        return lvl_string;
    }

    std::string Logger::cust_printf(const char *format, va_list args)
    {
        char buffer[5000];
        vsprintf(buffer, format, args);
        return buffer;
    }

    void Logger::unavailable_msg()
    {
        std::string _msg = f_red + "[Unavailabe]" + s_reset + " Logger Status : ";
        switch (log_fmt.status)
        {
        case LoggerStatus::None:
            _msg += "None";
            break;
        case LoggerStatus::Init:
            _msg += "Init";
            break;
        case LoggerStatus::Run:
            _msg += "Run";
            break;
        case LoggerStatus::Wait:
            _msg += "Wait";
            break;
        case LoggerStatus::Wait_Success:
            _msg += "Wait-Success";
            break;
        case LoggerStatus::Wait_Failed:
            _msg += "Wait-Failed";
            break;
        case LoggerStatus::Stop:
            _msg += "Stop";
            break;
        default:
            _msg += "Unknown";
            break;
        }
        std::cout << _msg << '\n';
    }

    void Logger::write(LogLevel level, const char *format, ...)
    {
        // Lock mutex
        std::lock_guard<std::mutex> lg(log_fmt.mtx);

        if (log_fmt.status == LoggerStatus::Run)
        {
            va_list args;
            va_start(args, format);
            std::string msg = cust_printf(format, args);
            va_end(args);

            line_counter++;
            std::string header = getLvl(level, true);
            std::time_t currentTime = std::time(nullptr);
            char timeString[100];
            std::string separator = "   ";
            if (type == FileType::CSV)
                separator = ',';
            header += separator;
            header += std::to_string(line_counter);
            header += separator;
            std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
            header += timeString;
            header += separator;

            std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
            auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
            header += std::to_string(ms_elapsed.count() / 1000000.0f);

            header += separator + msg;
            writer << header << '\n';
            update_counter(level);
        }
        else
            unavailable_msg();
    }

    void Logger::show(LogLevel level, const char *format, ...)
    {
        // Lock mutex
        std::lock_guard<std::mutex> lg(log_fmt.mtx);

        if (log_fmt.status == LoggerStatus::Run)
        {
            va_list args;
            va_start(args, format);
            std::string msg = cust_printf(format, args);
            va_end(args);

            std::string header = getLvl(level);
            std::time_t currentTime = std::time(nullptr);
            char timeString[100];
            std::strftime(timeString, sizeof(timeString), "%H:%M:%S", std::localtime(&currentTime));
            header += ' ';
            header += timeString;
            header += ' ';

            std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
            auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
            header += std::to_string(ms_elapsed.count() / 1000000.0f);
            header += "s : " + msg;
            if (level == LogLevel::ASK)
            {
                std::cout << header;
                std::cout.flush();
            }
            else
                std::cout << header << '\n';
            update_counter(level);
        }
        else
            unavailable_msg();
    }

    void Logger::write_show(LogLevel level, const char *format, ...)
    {
        // Lock mutex
        std::lock_guard<std::mutex> lg(log_fmt.mtx);

        if (log_fmt.status == LoggerStatus::Run)
        {
            va_list args;
            va_start(args, format);
            std::string msg = cust_printf(format, args);
            va_end(args);

            line_counter++;
            std::string header = getLvl(level, true);
            std::time_t currentTime = std::time(nullptr);
            std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
            auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
            char timeString[100];
            {
                std::string separator = "   ";
                if (type == FileType::CSV)
                    separator = ',';
                header += separator;
                header += std::to_string(line_counter);
                header += separator;
                std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
                header += timeString;
                header += separator;
                header += std::to_string(ms_elapsed.count() / 1000000.0f);

                header += separator + msg;
                writer << header << '\n';
            }
            header = getLvl(level);
            {
                std::strftime(timeString, sizeof(timeString), "%H:%M:%S", std::localtime(&currentTime));
                header += ' ';
                header += timeString;
                header += ' ';
                header += std::to_string(ms_elapsed.count() / 1000000.0f);

                header += "s : " + msg;

                if (level == LogLevel::ASK)
                {
                    std::cout << header;
                    std::cout.flush();
                }
                else
                    std::cout << header << '\n';
            }
            update_counter(level);
        }
        else
            unavailable_msg();
    }

    char Logger::get_hidden_char()
    {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        std::cin >> ch;
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    bool Logger::ask(std::string question)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        std::time_t currentTime = std::time(nullptr);
        std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
        auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);

        std::string msg = question;
        std::string header = getLvl(LogLevel::ASK);
        char timeString[100];
        {
            std::strftime(timeString, sizeof(timeString), "%H:%M:%S", std::localtime(&currentTime));
            header += ' ';
            header += timeString;
            header += ' ';
            header += std::to_string(ms_elapsed.count() / 1000000.0f);
            header += "s : " + msg + " (y/n) ";

            std::cout << header;
            std::cout.flush();
        }

        bool result = false;
        while (true)
        {
            char choice = get_hidden_char();
            if (choice == 'y' || choice == 'Y')
            {
                result = true;
                std::cout << "\t[" << check << " ]\n";
                break;
            }
            else if (choice == 'n' || choice == 'N')
            {
                std::cout << "\t[" << cross << " ]\n";
                break;
            }
        }

        line_counter++;
        header = getLvl(LogLevel::ASK, true);
        {
            std::string separator = "   ";
            if (type == FileType::CSV)
                separator = ',';
            header += separator;
            header += std::to_string(line_counter);
            header += separator;
            std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
            header += timeString;
            header += separator;
            header += std::to_string(ms_elapsed.count() / 1000000.0f);

            header += separator + msg;
            if (result)
                header += " [Accept]";
            else
                header += " [Reject]";
            writer << header << '\n';
        }
        return result;
    }

    template <typename T>
    void Logger::list_show(std::string header, std::vector<ListItem<T>> items)
    {
        if (!items.size())
            return;
        std::cout << f_blue << s_bold << "〘 " << header << " 〙" << s_reset << std::endl;
        if (items.size() == 1)
        {
            std::cout << "  ─" << s_bold << items[0].row_header << "\t: " << s_reset << items[0].value << '\n';
            return;
        }
        for (int i = 0; i < items.size(); ++i)
        {
            std::cout << "    ";
            if (i == 0)
                std::cout << ul_line;
            else if (i == items.size() - 1)
                std::cout << ll_line;
            else
                std::cout << cl_line;
            std::cout << s_bold << items[i].row_header << "\t: " << s_reset << items[i].value << " " << items[i].unit << '\n';
        }
    }

    template void Logger::list_show<double>(std::string header, std::vector<ListItem<double>> items);
    template void Logger::list_show<int>(std::string header, std::vector<ListItem<int>> items);
    template void Logger::list_show<float>(std::string header, std::vector<ListItem<float>> items);

    void Logger::finish(bool show)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if (log_fmt.status == LoggerStatus::Run)
        {
            stop_time = std::chrono::system_clock::now();
            writer.close();
            if (show)
                resume();
            log_fmt.status = LoggerStatus::Stop;
        }
        else if (show)
            unavailable_msg();
    }

    Logger &Logger::wait(std::string wait_msg)
    {
        log_fmt.status = LoggerStatus::Wait;
        log_fmt.message = wait_msg;
        std::thread th = std::thread(wait_thread, &log_fmt);
        th.detach();
        return *this;
    }

    Logger &Logger::wait_success()
    {
        log_fmt.status = LoggerStatus::Wait_Success;
        while (!log_fmt.mtx.try_lock())
            ;
        log_fmt.mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return *this;
    }

    Logger &Logger::wait_failed()
    {
        log_fmt.status = LoggerStatus::Wait_Failed;
        while (!log_fmt.mtx.try_lock())
            ;
        log_fmt.mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return *this;
    }

    Logger::~Logger()
    {
    }
}
