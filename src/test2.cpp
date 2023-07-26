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
    std::string str = homedir;
    str += "/Flight Log";
    std::cout << str << std::endl;

    bool is_exist = boost::filesystem::exists(str);

    if(is_exist)
        std::cout << "Exist" << std::endl;
    else
        std::cout << "Exist" << std::endl;

    return 0;
}