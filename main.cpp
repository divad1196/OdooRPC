#include <stdlib.h>
#include <iostream>
#include "src/odoorpc.h"
#include <string>


int main(int argc, char *argv[]) {
    if(argc < 5) return EXIT_FAILURE;
    std::cout << "Right number of parameters" << std::endl;

    const std::string URL = argv[1];
    const std::string DATABASE = argv[2];

    // TODO: Verify if id is the session id return in the cookies
    OdooRPC client (
        URL,
        DATABASE,
        {
            argv[3],
            argv[4]
        }
    );

    if(argc > 5) {
        client.forceUid(argv[5]);
        std::cout << client.raw_query(
            "res.partner",
            "search",
            {
                "[]"
            }
        ) << std::endl;
    }


    return EXIT_SUCCESS;
}