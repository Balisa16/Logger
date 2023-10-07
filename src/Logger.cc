#include <Logger.hpp>

namespace EMIRO{
    void wait_thread(LoggerFormat *format)
    {
        std::lock_guard<std::mutex> lg(format->mtx);
        std::string _cap = "\033[35m[WAIT ]\033[0m";
        std::time_t _current_time = std::time(nullptr);
        char _time_char[100];
        std::strftime(_time_char, sizeof(_time_char), "%H:%M:%S", std::localtime(&_current_time));
        _cap += ' ';
        _cap += _time_char;
        _cap += ' ';

        std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - format->start_time;
        auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
        _cap += std::to_string(ms_elapsed.count()/1000000.0f);

        _cap += "s : " + format->message + " ";

        std::chrono::_V2::system_clock::time_point start_wait = std::chrono::system_clock::now();

        std::cout << _cap;
        std::cout.flush();
        while(format->status == LoggerStatus::Wait)
        {
            std::cout << ".";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        t_elapsed = std::chrono::system_clock::now() - start_wait;
        ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);

        std::cout << " (OK) " << std::to_string(ms_elapsed.count()/1000000.0f) << " s\n";
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

    void Logger::resume()
    {
        std::time_t time = std::chrono::system_clock::to_time_t(log_fmt.start_time);
        char start_str[100];
        std::strftime(start_str, sizeof(start_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

        time = std::chrono::system_clock::to_time_t(stop_time);
        char stop_str[100];
        std::strftime(stop_str, sizeof(stop_str), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

        std::chrono::duration<double> elapsed_seconds = stop_time - log_fmt.start_time;

        std::cout << "\n\033[34m\033[1mFlight Resume :\033[0m\n\tStart Time\t: " << start_str << 
            "\n\tStop Time\t: " << stop_str << 
            "\n\tFlight Duration\t: " << std::to_string(elapsed_seconds.count()) << " seconds\nErrors\t\t: " << err_msg << 
            " message\nWarnings\t: " << warn_msg <<
            " message\nInformations\t: " << info_msg << " message\n\n";
    }

    void Logger::init(std::string filename, FileType type)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if(log_fmt.status  == LoggerStatus::None)
        {
            struct passwd *pw = getpwuid(getuid());
            const char *home_char = pw->pw_dir;
            std::string home_dir = home_char;
            std::string flight_dir = "Flight Log";
            std::string temp_1 = home_dir + "/" + flight_dir;

            if(!boost::filesystem::exists(temp_1))
                if(!boost::filesystem::create_directory(temp_1))
                {
                    std::cout << "\033[31m\033[1mERROR\t: \033[0m Failed to create \"" << temp_1 << "\"\n ";
                    throw;
                }

            // Set file type
            this->type = type;

            std::string file_ext = ".csv";
            if(type == FileType::TXT)
                file_ext = ".txt";

            std::string temp_filedir = home_dir + "/" + flight_dir +"/";
            
            temp_1 = temp_filedir + filename + file_ext;
            if(boost::filesystem::exists(temp_1.c_str()))
            {
                std::cout << "\033[34m\033[1mFile\t: \033[0m" << temp_1 << " \033[32m\033[3m[already exist]\033[0m" << std::endl;
                int file_idx = 1;
                temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
                while (boost::filesystem::exists(temp_1.c_str()))
                {
                    file_idx++;
                    temp_1 = temp_filedir + filename + "-" + std::to_string(file_idx) + file_ext;
                }
            }
            
            this->full_filename = temp_1;
            std::cout << "\033[34m\033[1mStored\t: \033[0m" << temp_1 << std::endl << std::endl;
            info_msg = 0;
            warn_msg = 0;
            err_msg = 0;
            combo_msg = false;
            log_fmt.status = LoggerStatus::Init;
        }
    }

    void Logger::start(bool reset_prev_counter)
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if(log_fmt.status  == LoggerStatus::Init || log_fmt.status  == LoggerStatus::Stop)
        {
            if(reset_prev_counter)
            {
                warn_msg = 0;
                info_msg = 0;
                err_msg = 0;
            }
            log_fmt.start_time = std::chrono::system_clock::now();
            writer.open(this->full_filename);
            if(type == FileType::CSV)
                writer << "Level,Message Id,Datetime (yyyy-MM-dd hh:mm:ss),Flight Time(s),Message\n";
            line_counter = 0;
            is_start = true;
            log_fmt.status = LoggerStatus::Run;
        }
        else if(log_fmt.status  == LoggerStatus::Run)
            std::cout << "\033[33mLogger is already activated.\033[30m\n";
        else if(log_fmt.status  == LoggerStatus::None)
            std::cout << "\033[31mLogger is not initialized.\033[30m\n";
        else
            std::cout << "\033[33mLogger in Waiting Mode.\033[30m\n";
    }

    std::string Logger::getLvl(LogLevel lvl, bool no_color)
    {
        std::string lvl_string = "";
        if(no_color)
            switch (lvl)
            {
            case LogLevel::INFO:
                lvl_string += "\033[32m[INFO ]\033[0m";
                break;
            case LogLevel::WARNING:
                lvl_string += "\033[33m[WARN ]\033[0m";
                break;
            case LogLevel::ERROR:
                lvl_string += "\033[31m[ERROR]\033[0m";
                break;
            
            default:
                break;
            }
        else
            switch (lvl)
            {
            case LogLevel::INFO:
                lvl_string += "INFO";
                break;
            case LogLevel::WARNING:
                lvl_string += "WARN";
                break;
            case LogLevel::ERROR:
                lvl_string += "ERROR";
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
        std::string _msg = "\033[31m[Unavailabe]\033[0m Logger Status : ";
        switch(log_fmt.status)
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

        if(log_fmt.status  == LoggerStatus::Run)
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
            if(type == FileType::CSV)
                separator = ',';
            header += separator;
            header += std::to_string(line_counter);
            header += separator;
            std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
            header += timeString;
            header += separator;
            
            std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
            auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
            header += std::to_string(ms_elapsed.count()/1000000.0f);

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
        }else
            unavailable_msg();
    }

    void Logger::show(LogLevel level, const char *format, ...)
    {
        // Lock mutex
        std::lock_guard<std::mutex> lg(log_fmt.mtx);

        if(log_fmt.status  == LoggerStatus::Run){
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
            header += std::to_string(ms_elapsed.count()/1000000.0f);

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
        }else
            unavailable_msg();
    }

    void Logger::write_show(LogLevel level, const char *format, ...)
    {
        // Lock mutex
        std::lock_guard<std::mutex> lg(log_fmt.mtx);

        if(log_fmt.status  == LoggerStatus::Run)
        {
            combo_msg = true;

            va_list args;
            va_start(args, format);
            std::string msg = cust_printf(format, args);
            va_end(args);
            
            line_counter++;
            std::string header = getLvl(level);
            std::time_t currentTime = std::time(nullptr);
            char timeString[100];
            {
                std::string separator = "   ";
                if(type == FileType::CSV)
                    separator = ',';
                header += separator;
                header += std::to_string(line_counter);
                header += separator;
                std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
                header += timeString;
                header += separator;
                
                std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
                auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
                header += std::to_string(ms_elapsed.count()/1000000.0f);

                header += separator + msg;
                writer << header << '\n';
            }
            header = getLvl(level, true);
            {
                std::strftime(timeString, sizeof(timeString), "%H:%M:%S", std::localtime(&currentTime));
                header += ' ';
                header += timeString;
                header += ' ';

                std::chrono::duration<double> t_elapsed = std::chrono::system_clock::now() - log_fmt.start_time;
                auto ms_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t_elapsed);
                header += std::to_string(ms_elapsed.count()/1000000.0f);

                header += "s : " + msg;

                std::cout << header << '\n';
            }
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
        }else
            unavailable_msg();
    }

    void Logger::finish()
    {
        std::lock_guard<std::mutex> lg(log_fmt.mtx);
        if(log_fmt.status  == LoggerStatus::Run)
        {
            stop_time = std::chrono::system_clock::now();
            writer.close();
            resume();
            log_fmt.status = LoggerStatus::Stop;
        }else
            unavailable_msg();
    }

    Logger& Logger::wait(std::string wait_msg)
    {
        log_fmt.status = LoggerStatus::Wait;
        log_fmt.message = wait_msg;
        std::thread th = std::thread(wait_thread, &log_fmt);
        th.detach();
        return *this;
    }

    Logger& Logger::wait_stop()
    {
        log_fmt.status = LoggerStatus::Run;
        return *this;
    }

    Logger::~Logger()
    {
    }
}
