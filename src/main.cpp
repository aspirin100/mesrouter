#include <iostream>
#include "config.h"
#include "application.h"

int main()
{
    try
    {
        auto config = ParseConfig().value();

        Application app(config);

        app.StartProcess();
        app.ShowStats();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}