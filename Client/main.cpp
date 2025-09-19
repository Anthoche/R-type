/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "client.hpp"
#include "../Engine/Rendering/scene/SceneHandler.hpp"
#include <thread>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <client_name>" << std::endl;
        return 1;
    }
    std::string serverIp = argv[1];
    std::string clientName = argv[2];
    
    // Créer le client réseau
    GameClient client(serverIp, 4141, clientName);
    
    // Lancer le client réseau dans un thread séparé
    std::thread networkThread([&client]() {
        client.run();
    });
    
    // Attendre que le thread réseau se termine (quand GameStart est reçu)
    networkThread.join();
    
    // Maintenant que le jeu peut commencer, ouvrir la fenêtre
    std::cout << "Ouverture de la fenêtre de jeu..." << std::endl;
    SceneHandler handler;
    handler.openMenu();
    
    return 0;
}
