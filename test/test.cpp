#include <Logger.hpp>
#include <thread>
#include <vector>
#include <utility>
using namespace EMIRO;

int main()
{
    Logger lg;
    lg.init("test", FileType::CSV);
    // lg.start(true);

    for (int i = 1; i <= 1000; i++)
    {
        lg.write_show(LogLevel::INFO, "Data %d", i);
        lg.write_show(LogLevel::WARNING, "Data %d", i);
        lg.write_show(LogLevel::ERROR, "Data %d", i);
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    lg.finish();
    return 0;
}