/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Rungame
*/

#include "Rungame.hpp"
#include "../../Shared/Logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <thread>
#include <fstream>
#include <cmath>


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
    registry_server.register_component<component::rotation>();
    registry_server.register_component<component::scale>();
    registry_server.register_component<component::dynamic_position>();
    registry_server.register_component<component::drawable>();
    registry_server.register_component<component::sprite>();
    registry_server.register_component<component::model3D>();
    registry_server.register_component<component::audio>();
    registry_server.register_component<component::text>();
    registry_server.register_component<component::font>();
    registry_server.register_component<component::clickable>();
    registry_server.register_component<component::hoverable>();
    registry_server.register_component<component::controllable>();
    registry_server.register_component<component::health>();
    registry_server.register_component<component::damage>();
    registry_server.register_component<component::collision_box>();
    registry_server.register_component<component::hitbox_link>();
    registry_server.register_component<component::type>();
    registry_server.register_component<component::client_id>();
}

void ServerGame::enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
    std::lock_guard<std::mutex> lock(packetMutex);
    pendingPackets.push(PendingPacket{data, from});
}

void ServerGame::setInitialClients(const std::map<uint32_t, bool> &clients) {
    std::lock_guard<std::mutex> lock(initialClientsMutex);
    initialClients = clients;
}

void ServerGame::run(int roomId) {
    LOG("[Server] Starting game loop...");
    _roomId = roomId;
    setup_game_state();
    const int tickMs = 16;
    const float dt = 0.016f;
    game_loop(tickMs, dt);
}

void ServerGame::setup_game_state() {
    load_players(ASSETS_PATH "/Config_assets/Players/players.json");
    load_level(ASSETS_PATH "/Config_assets/Levels/world1.json");
    initialize_player_positions();
    index_existing_entities();
}

void ServerGame::game_loop(int tickMs, float dt) {
    while (true) {
        auto tickStart = std::chrono::high_resolution_clock::now();
        service_players(dt);
        broadcast_player_updates();
        sleep_to_maintain_tick(tickStart, tickMs);
    }
}

void ServerGame::service_players(float dt) {
    process_pending_messages();
    process_player_inputs(dt);
    apply_gravity(dt);
    check_death_zone();
    check_win_condition();
}

void ServerGame::broadcast_player_updates() {
    broadcast_states_to_clients();
    broadcast_player_health();
    broadcast_global_score();
    broadcast_individual_scores();
}

bool ServerGame::check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2) {
    return right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2;
}

void ServerGame::load_players(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_players(registry_server, json);
    } catch (const std::exception &e) {
        LOG_ERROR("load_players failed: " << e.what());
    }
}

void ServerGame::load_level(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_level_entities(registry_server, json);
    } catch (const std::exception &e) {
        LOG_ERROR("load_level failed: " << e.what());
    }
}

void ServerGame::index_existing_entities() {
    _obstacles.clear();
    _platforms.clear();

    auto &types = registry_server.get_components<component::type>();

    for (std::size_t i = 0; i < types.size(); ++i) {
        if (!types[i])
            continue;

        ecs::entity_t entity = registry_server.entity_from_index(i);
        switch (types[i]->value) {
            case component::entity_type::OBSTACLE:
                _obstacles.push_back(entity);
                break;
            case component::entity_type::PLATFORM:
                _platforms.push_back(entity);
                break;
            default:
                break;
        }
    }
    
    LOG_DEBUG("[Server] Indexed entities: Obstacles=" << _obstacles.size() 
              << ", Platforms=" << _platforms.size());
}

void ServerGame::handle_client_message(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &) {
    if (data.size() < sizeof(MessageType))
        return;
    auto type = *reinterpret_cast<const MessageType *>(data.data());
    switch (type) {
        case MessageType::ClientInput:
            if (data.size() >= sizeof(ClientInputMessage))
                handle_client_input_message(*reinterpret_cast<const ClientInputMessage *>(data.data()));
            break;
        case MessageType::SceneState:
            if (data.size() >= sizeof(SceneStateMessage))
                handle_scene_state_message(*reinterpret_cast<const SceneStateMessage *>(data.data()));
            break;
        case MessageType::InitialHealth:
            if (data.size() >= sizeof(InitialHealthMessage))
                handle_initial_health_message(*reinterpret_cast<const InitialHealthMessage *>(data.data()));
            break;
        default:
            break;
    }
}

void ServerGame::handle_client_input_message(const ClientInputMessage &msg) {
    uint32_t clientId = ntohl(msg.clientId);
    if (deadPlayers.find(clientId) != deadPlayers.end())
        return;
    InputCode code = static_cast<InputCode>(msg.inputCode);
    if (!is_tracked_input(code))
        return;
    bool pressed = msg.isPressed != 0;
    playerInputBuffers[clientId].push_back({code, pressed});
}

void ServerGame::handle_scene_state_message(const SceneStateMessage &msg) {
    uint32_t clientId = ntohl(msg.clientId);
    SceneState scene = static_cast<SceneState>(ntohl(msg.scene));
    {
        std::lock_guard<std::mutex> lock(mtx);
        clientScenes[clientId] = scene;
    }
    LOG_DEBUG("[Server] Client " << clientId << " switched to scene = " << static_cast<int>(scene));
    if (scene == SceneState::GAME)
        broadcast_full_registry_to(clientId);
}

void ServerGame::handle_initial_health_message(const InitialHealthMessage &msg) {
    uint32_t clientId = ntohl(msg.clientId);
    int16_t initialHealth = ntohs(msg.initialHealth);
    auto &healths = registry_server.get_components<component::health>();
    auto &clientIds = registry_server.get_components<component::client_id>();
    for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
        if (!clientIds[i] || clientIds[i]->id != clientId)
            continue;
        if (healths[i]) {
            healths[i]->current = initialHealth;
            healths[i]->max = initialHealth;
            LOG_DEBUG("[Server] Set initial health of client " << clientId << " to " << initialHealth);
        }
        break;
    }
}

bool ServerGame::is_tracked_input(InputCode code) const {
    switch (code) {
        case InputCode::Up:
        case InputCode::Down:
        case InputCode::Left:
        case InputCode::Right:
        case InputCode::J:
        case InputCode::K:
            return true;
        default:
            return false;
    }
}

void ServerGame::process_input_event(uint32_t clientId, PlayerInputState &state, const InputEvent &event) {
    bool pressed = event.pressed;
    switch (event.code) {
        case InputCode::Up:
            state.up = pressed;
            if (pressed)
                set_last_direction(state, 0, -1);
            break;
        case InputCode::Down:
            state.down = pressed;
            if (pressed)
                set_last_direction(state, 0, 1);
            break;
        case InputCode::Left:
            state.left = pressed;
            if (pressed)
                set_last_direction(state, -1, 0);
            break;
        case InputCode::Right:
            state.right = pressed;
            if (pressed)
                set_last_direction(state, 1, 0);
            break;
        case InputCode::J:
            state.j = pressed;
            if (pressed)
                resolve_attack_input(clientId, state, InputCode::J);
            break;
        case InputCode::K:
            state.k = pressed;
            if (pressed)
                resolve_attack_input(clientId, state, InputCode::K);
            break;
        default:
            break;
    }
}

void ServerGame::set_last_direction(PlayerInputState &state, int x, int y) {
    state.lastDirX = x;
    state.lastDirY = y;
}

void ServerGame::resolve_attack_input(uint32_t clientId, PlayerInputState &state, InputCode code) {
    int dirX = state.lastDirX;
    int dirY = state.lastDirY;
    if (dirX == 0 && dirY == 0)
        dirX = 1;
    if (code == InputCode::J)
        handle_melee_attack(clientId, 15.f, 10, 350.f, 450.f, dirX, dirY);
    else
        handle_melee_attack(clientId, 25.f, 25, 650.f, 600.f, dirX, dirY);
}

void ServerGame::move_player_horizontally(uint32_t clientId, float dt, PlayerInputState &state,
                                     std::pair<float, float> &pos, float width, float height) {
    float inputX = 0.f;
    if (state.left != state.right)
        inputX = state.left ? -1.f : 1.f;
    if (inputX == 0.f)
        return;
    constexpr float speedPerSecond = 300.f;
    float distance = speedPerSecond * dt;
    float newX = pos.first + inputX * distance;
    if (is_position_blocked(newX, pos.second, width, height, _obstacles))
        return;
    if (is_position_blocked_platform(clientId, newX, pos.second, width, height, _platforms, false, state.down))
        return;
    pos.first = newX;
}

void ServerGame::apply_horizontal_knockback(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                        float width, float height) {
    auto &knockbackX = playerHorizontalKnockback[clientId];
    if (std::fabs(knockbackX) <= 1.f) {
        knockbackX = 0.f;
        return;
    }
    float newX = pos.first + knockbackX * dt;
    if (!is_position_blocked(newX, pos.second, width, height, _obstacles) &&
        !is_position_blocked_platform(clientId, newX, pos.second, width, height, _platforms, false, playerInputStates[clientId].down)) {
        pos.first = newX;
    } else {
        knockbackX = 0.f;
        return;
    }
    constexpr float damping = 800.f;
    if (knockbackX > 0.f)
        knockbackX = std::max(0.f, knockbackX - damping * dt);
    else
        knockbackX = std::min(0.f, knockbackX + damping * dt);
}

void ServerGame::process_pending_messages() {
    std::queue<PendingPacket> localQueue;
    {
        std::lock_guard<std::mutex> lock(packetMutex);
        while (!pendingPackets.empty()) {
            localQueue.push(std::move(pendingPackets.front()));
            pendingPackets.pop();
        }
    }

    constexpr int maxMessagesPerTick = 32;
    int processed = 0;
    while (!localQueue.empty() && processed < maxMessagesPerTick) {
        PendingPacket packet = std::move(localQueue.front());
        localQueue.pop();
        handle_client_message(packet.data, packet.endpoint);
        ++processed;
    }
}

std::pair<float, float> ServerGame::compute_respawn_position(uint32_t clientId, float playerWidth, float playerHeight) {
    auto &positions = registry_server.get_components<component::position>();
    auto &collision = registry_server.get_components<component::collision_box>();

    for (auto entity : _platforms) {
        auto idx = static_cast<std::size_t>(entity);
        if (idx < positions.size() && positions[idx] && idx < collision.size() && collision[idx]) {
            float platformTop = positions[idx]->y - collision[idx]->height * 0.5f;
            float spawnY = platformTop - playerHeight * 0.5f - 5.f;
            float spawnX = positions[idx]->x;
            return {spawnX, spawnY};
        }
    }

    auto it = playerSpawnPositions.find(clientId);
    if (it != playerSpawnPositions.end())
        return it->second;

    return {300.f + (clientId - 1) * 50.f, 300.f};
}

bool ServerGame::handle_player_defeat(uint32_t clientId, std::size_t registryIndex) {
    ensure_player_tracking(clientId);
    auto &healths = registry_server.get_components<component::health>();
    bool hasHealth = registryIndex < healths.size() && healths[registryIndex];
    if (hasHealth)
        healths[registryIndex]->current = std::max(0, healths[registryIndex]->current);
    int &lives = playerLives[clientId];
    if (lives <= 0)
        return finalize_player_death(clientId, registryIndex);
    --lives;
    if (lives > 0 && hasHealth) {
        respawn_player(clientId, registryIndex);
        return false;
    }
    if (hasHealth)
        healths[registryIndex]->current = 0;
    return finalize_player_death(clientId, registryIndex);
}

void ServerGame::process_player_inputs(float dt) {
    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;
        ensure_player_tracking(clientId);
        auto &state = playerInputStates[clientId];
        auto &buffer = playerInputBuffers[clientId];
        for (const auto &event : buffer)
            process_input_event(clientId, state, event);
        buffer.clear();
        auto &pos = kv.second;
        auto [width, height] = get_player_dimensions(clientId);
        move_player_horizontally(clientId, dt, state, pos, width, height);
        apply_horizontal_knockback(clientId, dt, pos, width, height);
    }
}

float ServerGame::snap_to_platform_top(float x, float y, float playerWidth, float playerHeight) {
    constexpr float epsilon = 5.f;
    
    for (auto entity : _platforms) {
        auto pos = component_ptr<component::position>(registry_server, entity);
        auto box = component_ptr<component::collision_box>(registry_server, entity);

        if (!pos || !box)
            continue;

        float pLeft = x - playerWidth / 2.f;
        float pRight = x + playerWidth / 2.f;
        float oLeft = pos->x - box->width / 2.f;
        float oRight = pos->x + box->width / 2.f;

        if (pRight > oLeft && pLeft < oRight) {
            float platformTop = pos->y - box->height / 2.f;
            if (std::abs(y + playerHeight / 2.f - platformTop) < epsilon) {
                return platformTop - playerHeight / 2.f;
            }
        }
    }
    return y;
}

void ServerGame::check_death_zone() {
    constexpr float deathY = 1180.f;
    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;
        ensure_player_tracking(clientId);
        auto &pos = kv.second;
        if (pos.second <= deathY && pos.first >= -150 && pos.first <= 2070)
            continue;
        LOG_INFO("[Server] Player " << clientId << " went out of bounds and died");
        std::size_t index = registry_index_for_client(clientId);
        auto &healths = registry_server.get_components<component::health>();
        if (index < healths.size() && healths[index])
            healths[index]->current = 0;
        handle_player_defeat(clientId, index);
    }
}

void ServerGame::check_win_condition() {
    auto winner = find_last_alive_player();
    if (!winner)
        return;
    handle_victory(*winner);
}

std::optional<uint32_t> ServerGame::find_last_alive_player() {
    int alivePlayers = 0;
    uint32_t lastAlive = 0;
    const auto &clients = connexion.getClients();
    for (const auto &kv : clients) {
        uint32_t id = kv.second;
        if (deadPlayers.find(id) != deadPlayers.end())
            continue;
        ++alivePlayers;
        lastAlive = id;
    }
    if (alivePlayers == 1 && lastAlive != 0 && clients.size() >= 2)
        return lastAlive;
    return std::nullopt;
}

void ServerGame::handle_victory(uint32_t winnerId) {
    LOG_INFO("[Server] Player " << winnerId << " is the last one alive and WINS!");
    totalScore = std::max(totalScore, 30);
    broadcast_game_winner(winnerId);
    auto &clientIds = registry_server.get_components<component::client_id>();
    auto &positions = registry_server.get_components<component::position>();
    auto &drawables = registry_server.get_components<component::drawable>();
    auto &healths = registry_server.get_components<component::health>();
    for (std::size_t i = 0; i < clientIds.size(); ++i) {
        if (!clientIds[i] || clientIds[i]->id != winnerId)
            continue;
        ecs::entity_t entity = registry_server.entity_from_index(i);
        std::size_t idx = entity.value();
        float screenBottom = 1080.f;
        if (idx < positions.size() && positions[idx] && idx < drawables.size() && drawables[idx] &&
            idx < healths.size() && healths[idx]) {
            float bottom = positions[idx]->y + drawables[idx]->height * 0.5f;
            healths[idx]->current = (bottom < screenBottom - 5.f) ? healths[idx]->max : 0;
        }
        break;
    }
}

std::size_t ServerGame::registry_index_for_client(uint32_t clientId) {
    auto &clientIds = registry_server.get_components<component::client_id>();
    for (std::size_t i = 0; i < clientIds.size(); ++i) {
        if (clientIds[i] && clientIds[i]->id == clientId)
            return i;
    }
    return clientIds.size();
}

bool ServerGame::finalize_player_death(uint32_t clientId, std::size_t registryIndex) {
    (void)registryIndex;
    reset_player_motion(clientId);
    playerLives[clientId] = 0;
    bool inserted = deadPlayers.insert(clientId).second;
    if (inserted)
        broadcast_player_death(clientId);
    return true;
}

void ServerGame::reset_player_motion(uint32_t clientId) {
    playerHorizontalKnockback[clientId] = 0.f;
    playerVerticalKnockback[clientId] = 0.f;
    playerVelocities[clientId] = 0.f;
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
