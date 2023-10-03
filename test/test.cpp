#include <Logger.hpp>
#include <thread>
#include <vector>
#include <utility>
using namespace EMIRO;

int main()
{
    Logger lg;
    lg.init("test", FileType::CSV);
    lg.start(true);
    std::vector<std::thread> th_vect = std::vector<std::thread>();
    for (int i = 0; i < 10; i++)
    {
        LogLevel ll = LogLevel::INFO;
        const char *word = "Message %d";
        int a = 1;

        std::thread th(lg.write_show, ll, word, a);
        th_vect.push_back(th);
    }
    for (int i = 0; i < th_vect.size(); ++i)
        th_vect[i].join();
    
    lg.finish();
    return 0;
}