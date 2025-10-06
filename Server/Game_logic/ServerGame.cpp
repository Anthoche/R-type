/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#include "Include/ServerGame.hpp"
#include "../../Engine/Utils/Include/serializer.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>
// === Couleurs pour les logs ===
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define LOG_ERROR(msg)   std::cerr << RED << "[ERROR] " << msg << RESET << std::endl
#define LOG_INFO(msg)    std::cout << GREEN << "[INFO] " << msg << RESET << std::endl
#define LOG_DEBUG(msg)   std::cout << YELLOW << "[DEBUG] " << msg << RESET << std::endl
#define LOG(msg)         std::cout << BLUE << msg << RESET << std::endl

static nlohmann::json load_json_from_file(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + path);
    }
    nlohmann::json data;
    file >> data;
    return data;
}

ServerGame::ServerGame(Connexion &conn) : connexion(conn), registry_server() {
    registry_server.register_component<component::position>();
    registry_server.register_component<component::previous_position>();
    registry_server.register_component<component::velocity>();
    registry_server.register_component<component::controllable>();
    registry_server.register_component<component::health>();
    registry_server.register_component<component::type>();
    registry_server.register_component<component::client_id>();
    registry_server.register_component<component::drawable>();
    registry_server.register_component<component::sprite>();
    registry_server.register_component<component::collision_box>();
    registry_server.register_component<component::hitbox_link>();
}

void ServerGame::run() {
    LOG("[Server] Starting game loop...");
    load_players("../Engine/Assets/Config_assets/Players/players.json");
    //load_level("../Engine/Assets/Config_assets/Levels/level_01.json");
    initialize_player_positions();
    initialize_obstacles();
    const int tick_ms = 16;
    float dt = 0.016f;
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        update_projectiles_server_only(dt);
        check_projectile_collisions();
        broadcast_projectile_positions();
        process_pending_messages();
        broadcast_states_to_clients();
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2) {
    return right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2;
}

void ServerGame::check_projectile_collisions() {
    std::vector<uint32_t> projectilesToRemove;
    std::vector<uint32_t> obstaclesToRemove;
    
    for (const auto& proj : projectiles) {
        uint32_t projId = proj.first;
        float projX = std::get<0>(proj.second);
        float projY = std::get<1>(proj.second);        
        float projLeft = projX - 5.f;
        float projRight = projX + 5.f;
        float projTop = projY - 2.5f;
        float projBottom = projY + 2.5f;
        
        for (const auto& obs : obstacles) {
            uint32_t obsId = obs.first;
            float obsX = std::get<0>(obs.second);
            float obsY = std::get<1>(obs.second);
            float obsW = std::get<2>(obs.second);
            float obsH = std::get<3>(obs.second);
        
            float obsLeft = obsX - obsW * 0.5f;
            float obsRight = obsX + obsW * 0.5f;
            float obsTop = obsY - obsH * 0.5f;
            float obsBottom = obsY + obsH * 0.5f;
            
            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  obsLeft, obsRight, obsTop, obsBottom)) {
                projectilesToRemove.push_back(projId);
                break;
            }
        }
    }
    
    for (uint32_t projId : projectilesToRemove) {
        projectiles.erase(projId);
        broadcast_projectile_despawn(projId);
    }
    
    for (uint32_t obsId : obstaclesToRemove) {
        obstacles.erase(obsId);
        broadcast_obstacle_despawn(obsId);
    }
}

void ServerGame::broadcast_projectile_positions() {
    for (const auto& kv : projectiles) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        
        ProjectileUpdateMessage msg;
        msg.type = MessageType::ProjectileUpdate;
        msg.projectileId = htonl(id);
        
        uint32_t xb, yb;
        std::memcpy(&xb, &x, sizeof(float));
        std::memcpy(&yb, &y, sizeof(float));
        
        msg.posXBits = htonl(xb);
        msg.posYBits = htonl(yb);
        
        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::update_projectiles_server_only(float dt) {
    std::vector<uint32_t> toRemove;
    
    for (auto& kv : projectiles) {
        uint32_t id = kv.first;
        float& x = std::get<0>(kv.second);
        float& y = std::get<1>(kv.second);
        float vx = std::get<2>(kv.second);
        float vy = std::get<3>(kv.second);
        x += vx * dt;
        y += vy * dt;

        if (x < -50.f || x > 850.f || y < -50.f || y > 650.f) {
            toRemove.push_back(id);
        }
    }
    for (uint32_t id : toRemove) {
        projectiles.erase(id);
        broadcast_projectile_despawn(id);
    }
}

void ServerGame::broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, 
                                    float x, float y, float vx, float vy) {
    ProjectileSpawnMessage msg;
    msg.type = MessageType::ProjectileSpawn;
    msg.projectileId = htonl(projId);
    msg.ownerId = htonl(ownerId);
    
    uint32_t xb, yb, vxb, vyb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    
    msg.posXBits = htonl(xb);
    msg.posYBits = htonl(yb);
    msg.velXBits = htonl(vxb);
    msg.velYBits = htonl(vyb);
    
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_projectile_despawn(uint32_t projId) {
    ProjectileDespawnMessage msg;
    msg.type = MessageType::ProjectileDespawn;
    msg.projectileId = htonl(projId);
    connexion.broadcast(&msg, sizeof(msg));
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


void ServerGame::load_players(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_players(registry_server, json);
        LOG_DEBUG("===== Loaded Players =====");
        auto &positions = registry_server.get_components<component::position>();
        auto &clientIds = registry_server.get_components<component::client_id>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value() && clientIds[i].has_value()) {
                auto &pos = positions[i].value();
                auto &cid = clientIds[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
                LOG("Player entity " << entity
                          << " client_id=" << cid.id
                          << " pos=(" << pos.x << ", " << pos.y << ")");
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("load_players failed: " << e.what());
    }
}

void ServerGame::load_level(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_level_entities(registry_server, json);
        LOG_DEBUG("===== Loaded Level Entities =====");
        auto &positions = registry_server.get_components<component::position>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value()) {
                auto &pos = positions[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
                LOG("Entity " << entity
                          << " pos=(" << pos.x << ", " << pos.y << ")");
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("load_level failed: " << e.what());
    }
}

void ServerGame::initialize_player_positions() {
    LOG_DEBUG("Initializing player positions...");
    for (const auto& kv : connexion.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void ServerGame::process_pending_messages() {
    connexion.asyncReceive([this](const asio::error_code& ec, std::vector<uint8_t> data, asio::ip::udp::endpoint endpoint) {
        if (ec)
            return;
        handle_client_message(data, endpoint);
    });
    // Note: asyncReceive appelle le handler plus tard, donc pas de boucle while ici.
}

bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                        const std::unordered_map<uint32_t, std::tuple<float, float, float, float>>& obstacles) {
    float halfW = playerWidth * 0.5f;
    float halfH = playerHeight * 0.5f;
    float left   = testX - halfW;
    float right  = testX + halfW;
    float top    = testY - halfH;
    float bottom = testY + halfH;
    for (const auto& kv : obstacles) {
        float obsX = std::get<0>(kv.second);
        float obsY = std::get<1>(kv.second);
        float obsW = std::get<2>(kv.second);
        float obsH = std::get<3>(kv.second);
        float obsLeft   = obsX - obsW * 0.5f;
        float obsRight  = obsX + obsW * 0.5f;
        float obsTop    = obsY - obsH * 0.5f;
        float obsBottom = obsY + obsH * 0.5f;
        if (check_aabb_overlap(left, right, top, bottom,
                              obsLeft, obsRight, obsTop, obsBottom)) {
            return true;
        }
    }
    return false;
}

void ServerGame::handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from) {
    if (data.size() < sizeof(MessageType))
        return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    switch(type) {
        case MessageType::ClientInput: {
            if (data.size() >= sizeof(ClientInputMessage)) {
                const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
                uint32_t id = ntohl(msg->clientId);
                float inputX, inputY;
                uint32_t xbits = ntohl(msg->inputXBits);
                uint32_t ybits = ntohl(msg->inputYBits);
                std::memcpy(&inputX, &xbits, sizeof(float));
                std::memcpy(&inputY, &ybits, sizeof(float));
                auto& pos = playerPositions[id];
                float speed = 200.f / 60.f;
                const float playerWidth = 30.f;
                const float playerHeight = 30.f;
                float newX = pos.first + inputX * speed;
                float newY = pos.second + inputY * speed;
                if (!is_position_blocked(newX, pos.second, playerWidth, playerHeight, obstacles))
                    pos.first = newX;
                if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, obstacles))
                    pos.second = newY;
            }
            break;
        }
        case MessageType::SceneState: {
            if (data.size() >= sizeof(SceneStateMessage)) {
                const SceneStateMessage* msg = reinterpret_cast<const SceneStateMessage*>(data.data());
                uint32_t id = ntohl(msg->clientId);
                SceneState scene = static_cast<SceneState>(ntohl(msg->scene));
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    clientScenes[id] = scene;
                }
                LOG_DEBUG("[Server] Client " << id << " switched to scene = " << static_cast<int>(scene));
                if (scene == SceneState::GAME) {
                    broadcast_full_registry_to(id);
                }
            }
            break;
        }
        case MessageType::ClientShoot: {
            if (data.size() >= sizeof(ClientShootMessage)) {
                const ClientShootMessage* msg = reinterpret_cast<const ClientShootMessage*>(data.data());
                uint32_t clientId = ntohl(msg->clientId);
                
                auto it = playerPositions.find(clientId);
                if (it != playerPositions.end()) {
                    float playerX = it->second.first;
                    float playerY = it->second.second;
                    
                    uint32_t projId = nextProjectileId++;
                    float projX = playerX + 20.f;
                    float projY = playerY;
                    float projVelX = 400.f;
                    float projVelY = 0.f;
                    
                    projectiles[projId] = std::make_tuple(projX, projY, projVelX, projVelY);
                    broadcast_projectile_spawn(projId, clientId, projX, projY, projVelX, projVelY);
                    
                    LOG_DEBUG("[Server] Client " << clientId << " shot projectile " << projId);
                }
            }
            break;
        }
        default:
            break;
    }
}

void ServerGame::broadcast_states_to_clients() {
    for (const auto& kv : connexion.getClients()) {
        uint32_t id = kv.second;
        auto it = playerPositions.find(id);
        if (it == playerPositions.end()) continue;
        StateUpdateMessage m;
        m.type = MessageType::StateUpdate;
        m.clientId = htonl(id);
        uint32_t xbits, ybits;
        std::memcpy(&xbits, &it->second.first, sizeof(float));
        std::memcpy(&ybits, &it->second.second, sizeof(float));
        m.posXBits = htonl(xbits);
        m.posYBits = htonl(ybits);
        connexion.broadcast(&m, sizeof(m));
    }
}

void ServerGame::initialize_obstacles() {
    LOG_DEBUG("Initializing obstacles...");
    struct O { float x,y,w,h; }; O list[3] = {
        {200.f, 400.f, 60.f, 60.f},
        {400.f, 400.f, 60.f, 60.f},
        {600.f, 400.f, 60.f, 60.f}
    };
    uint32_t oid = 1;
    for (auto &o : list) {
        obstacles[oid] = std::make_tuple(o.x, o.y, o.w, o.h);
        broadcast_obstacle_spawn(oid, o.x, o.y, o.w, o.h);
        ++oid;
    }
}

void ServerGame::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h) {
    ObstacleSpawnMessage m;
    m.type = MessageType::ObstacleSpawn;
    m.obstacleId = htonl(obstacleId);
    uint32_t xb, yb, wb, hb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&wb, &w, sizeof(float));
    std::memcpy(&hb, &h, sizeof(float));
    m.posXBits = htonl(xb);
    m.posYBits = htonl(yb);
    m.widthBits = htonl(wb);
    m.heightBits = htonl(hb);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
