#include <Logger.hpp>

int main()
{
    Logger lg("test", FileType::TXT);
    lg.start();
    lg.show("Sander Antonius Balisa");
    lg.finish();
    return 0;
}