#include <Logger.hpp>
#include <thread>

int main()
{
    // Logger lg;
    // lg.init("test", FileType::CSV);
    // lg.start();
    // for (size_t i = 0; i < 300; i++)
    // {
    //     lg.write_show("Message " + std::to_string(i));
    //     std::chrono::milliseconds sleepDuration(100);
    //     std::this_thread::sleep_for(sleepDuration);
    // }
    
    // lg.finish();


    struct passwd *pw = getpwuid(getuid());

    const char *homedir = pw->pw_dir;

    std::string str = *homedir + "/";
    
    std::cout << str << "Test" << std::endl;

    return 0;
}