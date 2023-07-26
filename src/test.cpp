#include <Logger.hpp>
#include <thread>

int main()
{
    Logger lg("test", FileType::CSV);
    lg.start();
    for (size_t i = 0; i < 300; i++)
    {
        lg.write_show("Message " + std::to_string(i));
        std::chrono::milliseconds sleepDuration(100);
        std::this_thread::sleep_for(sleepDuration);
    }
    
    lg.finish();
    return 0;
}