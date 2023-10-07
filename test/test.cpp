#include <Logger.hpp>

using namespace EMIRO;

int main()
{
    Logger lg;
    lg.init("test", FileType::CSV);
    lg.start(true);

    for (int i = 1; i <= 3; i++)
    {
        lg.write_show(LogLevel::INFO, "Data %d", i);
        lg.write_show(LogLevel::WARNING, "Data %d", i);
        lg.write_show(LogLevel::ERROR, "Data %d", i);
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // Witing for "Something" task and let it run in another thread
    lg.wait("Something");
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    // Send trigger to stop waiting period of "Something" task
    lg.wait_stop();

    lg.finish();
    return 0;
}