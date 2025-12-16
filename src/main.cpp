#include <iostream>
#include "config.h"

int main()
{
    try
    {
        auto conf = ParseConfig();
        if (!conf)
            std::cout << "failed to read config" << std::endl;
        else
            std::cout << conf->producers.messages_per_sec << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cout << "exception catched" << std::endl;
        std::cout << ex.what() << std::endl;
    }

    return 0;
}