#include <iostream>
#include "config.h"

int main()
{
    Config config;

    try
    {
        auto conf = ParseConfig();
        if (!conf)
        {
            std::cerr << "failed to read config" << std::endl;
            return -1;
        }
     
        config = std::move(conf.value()); // TODO: rule of 5
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}