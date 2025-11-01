/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGBroadcast
*/

#include "../../../Shared/Logger.hpp"
#include "Rungame.hpp"
#include <nlohmann/json.hpp>
#include <cstring>

static uint32_t to_network_bits(float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(float));
    return htonl(bits);
}

void ServerGame::broadcast_full_registry_to(uint32_t clientId) {
    nlohmann::json root;
    root["type"] = "FullRegistry";
    root["entities"] = nlohmann::json::array();

    for (auto entity : registry_server.alive_entities()) {
        auto j = game::serializer::serialize_entity(registry_server, entity);
        if (!j.empty()) {
            root["entities"].push_back(j);
        }
    }
    connexion.sendJsonToClient(clientId, root);
    LOG_DEBUG("[Server] Sent full registry to client " << clientId 
              << " with " << root["entities"].size() << " entities");
}

void ServerGame::broadcast_player_death(uint32_t clientId) {
    PlayerDeathMessage msg;
    msg.type = MessageType::PlayerDeath;
    msg.clientId = htonl(clientId);
    connexion.broadcast(&msg, sizeof(msg));
    LOG_INFO("[Server] Player " << clientId << " died!");
}

void ServerGame::broadcast_states_to_clients() {
    for (const auto &kv : connexion.getClients()) {
        uint32_t id = kv.second;
        if (deadPlayers.find(id) != deadPlayers.end())
            continue;
        auto it = playerPositions.find(id);
        if (it == playerPositions.end())
            continue;
        StateUpdateMessage m;
        m.type = MessageType::StateUpdate;
        m.clientId = htonl(id);
        m.pos.xBits = to_network_bits(it->second.first);
        m.pos.yBits = to_network_bits(it->second.second);
        m.pos.zBits = to_network_bits(0.f);
        connexion.broadcast(&m, sizeof(m));
    }
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::broadcast_player_health() {
    auto& healths = registry_server.get_components<component::health>();
    auto& clientIds = registry_server.get_components<component::client_id>();
    
    for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
        if (healths[i] && clientIds[i]) {
            PlayerHealthMessage msg;
            msg.type = MessageType::PlayerHealth;
            msg.clientId = htonl(clientIds[i]->id);
            msg.currentHealth = htons(static_cast<int16_t>(healths[i]->current));
            msg.maxHealth = htons(static_cast<int16_t>(healths[i]->max));
            
            connexion.broadcast(&msg, sizeof(msg));
        }
    }
}

void ServerGame::broadcast_global_score() {
    GlobalScoreMessage msg;
    msg.type = MessageType::GlobalScore;
    msg.totalScore = htonl(totalScore);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_individual_scores() {
    for (const auto &kv : playerIndividualScores) {
        IndividualScoreMessage msg;
        msg.type = MessageType::IndividualScore;
        msg.clientId = htonl(kv.first);
        msg.score = htonl(kv.second);
        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::broadcast_game_winner(uint32_t winnerId) {
    LOG_INFO("[Server] Broadcasting winner: Player " << winnerId);
}
