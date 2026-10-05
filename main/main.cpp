#include "core/application.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main()
{
    engine::Application app{};

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}