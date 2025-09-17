/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "client.hpp"


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <client_name>" << std::endl;
        return 1;
    }
    std::string serverIp = argv[1];
    std::string clientName = argv[2];
    GameClient client(serverIp, 4141, clientName);
    client.run();
    return 0;
}
