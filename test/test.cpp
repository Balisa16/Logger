#include <Logger.hpp>
#include <thread>

using namespace EMIRO;

int main()
{
    Logger lg;
    lg.init("test", FileType::CSV);
    lg.start(true);
    for (size_t i = 0; i < 10; i++)
    {
        lg.write_show(LogLevel::INFO, "Message %d", i);
        lg.write_show(LogLevel::WARNING, "Message %d", i);
        lg.write_show(LogLevel::ERROR, "Message %d", i);
        std::chrono::milliseconds sleepDuration(100);
        std::this_thread::sleep_for(sleepDuration);
    }
    
    lg.finish();
    return 0;
}