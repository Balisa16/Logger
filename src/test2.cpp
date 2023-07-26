#include <Logger.hpp>
#include <thread>

int main()
{
    Logger lg;
    lg.init("test", FileType::CSV);
    lg.start();
    for (size_t i = 0; i < 50; i++)
    {
        lg.write_show("Message " + std::to_string(i));
        std::chrono::milliseconds sleepDuration(100);
        std::this_thread::sleep_for(sleepDuration);
    }
    
    lg.finish();


    // struct passwd *pw = getpwuid(getuid());
    // const char *home_char = pw->pw_dir;
    // std::string home_dir = home_char;
    // std::string flight_dir = "Flight Log";
    // std::string temp_1 = home_dir + "/" + flight_dir;
    // std::cout << temp_1 << std::endl;

    // bool is_exist = boost::filesystem::exists(temp_1);

    // if(is_exist)
    //     std::cout << "Exist" << std::endl;
    // else
    // {
    //     std::cout << "Not Exist" << std::endl;
    //     temp_1 = "mkdir " + home_dir + "/'" + flight_dir +"'";
    //     system(temp_1.c_str());
    // }

    return 0;
}