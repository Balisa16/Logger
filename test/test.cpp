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

    std::vector<BranchItem<float>> it;
    it.push_back({"Item 1", 5.94, "m"});
    it.push_back({"Item 2", 2.44, "m"});
    it.push_back({"Item 3", 7.13, "m"});

    // Witing for "Something" task and let it run in another thread
    lg.wait("Success Task");
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    // Send trigger to stop waiting period of "Something" task
    lg.wait_success();
    lg.wait("Failed Task");
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    // Send trigger to stop waiting period of "Something" task
    lg.wait_failed();
    lg.branch_show("Parameter", it);
    lg.finish();
    return 0;
}