#include <stdlib.h>
#include <iostream>
#include "src/odoorpc.h"
#include <string>


int main(int argc, char *argv[]) {
    if(argc < 6){
        std::cerr << "Parameters: URL DATABASE LOGIN PASSWORD MODEL METHOD [ARGS]" << std::endl;
        return EXIT_FAILURE;
    }

    const char* URL = argv[1];
    const char* DATABASE = argv[2];
    const char* LOGIN = argv[3];
    const char* PASSWORD = argv[4];
    const char* MODEL = argv[5];
    const char* METHOD = argv[6];

    size_t args_size = (size_t)(argc - 7);
    std::vector<std::string> ARGS(args_size);
    for(size_t index = 0; index < args_size; ++index)
        ARGS[index] = argv[7 + index];

    std::cout   << "URL: "      << URL          << '\n'
                << "DATABASE: " << DATABASE     << '\n'
                << "LOGIN: "    << LOGIN        << '\n'
                << "PASSWORD: " << "******"     << '\n'
                << "MODEL: "    << MODEL        << '\n'
                << "METHOD: "   << METHOD       << '\n'
    << std::endl;


    // TODO: Verify if id is the session id return in the cookies
    OdooRPC client (
        URL,
        DATABASE,
        {
            LOGIN,
            PASSWORD
        }
    );

    std::cout << client.raw_query(
        MODEL,
        METHOD,
        ARGS
    ) << std::endl;


    return EXIT_SUCCESS;
}