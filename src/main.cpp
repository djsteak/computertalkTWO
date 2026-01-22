#include "Client.h"
#include "Server.h"
#include <SFML/Network.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "server") {
        Server server;
        server.run();
    } else {
        Client client;
        client.run();
    }
}


