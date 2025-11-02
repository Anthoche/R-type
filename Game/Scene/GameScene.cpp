/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** GameScene
*/

#include "Include/GameScene.hpp"
#include <iostream>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <string>
#include <nlohmann/json.hpp>
#include "../../Engine/Utils/Include/serializer.hpp"
#include <vector>
#include <filesystem>
#include "WeaponDefinition.hpp"

namespace {
    std::string normalizeNetworkAssetPath(const std::string &rawPath)
    {
        if (rawPath.empty())
            return rawPath;

        std::filesystem::path inputPath(rawPath);
        std::string generic = inputPath.generic_string();

        try {
            if (std::filesystem::exists(inputPath))
                return generic;
        } catch (const std::exception &) {
        }

        const std::string marker = "assets/";
        auto pos = generic.find(marker);
        if (pos != std::string::npos) {
            std::string relative = generic.substr(pos + marker.size());
            if (!relative.empty() && relative.front() == '/')
                relative.erase(relative.begin());
            return std::string(ASSETS_PATH) + "/" + relative;
        }

        if (!generic.empty() && generic.front() == '/')
            generic.erase(generic.begin());

        return std::string(ASSETS_PATH) + "/" + generic;
    }
}

namespace game::scene {
    GameScene::GameScene(Game &game)
        : AScene(1920, 1080, "R-Type"), _player(ecs::entity_t{0}), _game(game), _ui(*this, _registry, _raylib), _chat(_raylib) {
        _raylib = Raylib();
        _game_running = true;
        _startTime = 0.f;
    }

    void GameScene::init() {
        _registry.clear();
        _isOpen = true;
        _startTime = _raylib.getTime();
        _raylib.disableCursor();
        _raylib.setTargetFPS(60);
        toggleFullScreen();

        _registry.register_component<component::position>();
        _registry.register_component<component::previous_position>();
        _registry.register_component<component::velocity>();
        _registry.register_component<component::rotation>();
        _registry.register_component<component::scale>();
        _registry.register_component<component::dynamic_position>();
        _registry.register_component<component::drawable>();
        _registry.register_component<component::sprite>();
        _registry.register_component<component::model3D>();
        _registry.register_component<component::audio>();
        _registry.register_component<component::text>();
        _registry.register_component<component::font>();
        _registry.register_component<component::clickable>();
        _registry.register_component<component::hoverable>();
        _registry.register_component<component::controllable>();
        _registry.register_component<component::health>();
        _registry.register_component<component::damage>();
        _registry.register_component<component::collision_box>();
        _registry.register_component<component::hitbox_link>();
        _registry.register_component<component::type>();
        _registry.register_component<component::client_id>();
        _registry.register_component<component::pattern_element>();
        _ui.init();
        _chat.init();
        _chat.setUsername(_game.getGameClient().getClientName());
        _game.getGameClient().sendSceneState(SceneState::GAME, &_registry);

        setup_movement_system();
        setup_render_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height), 0.f);
        game::entities::setup_hitbox_sync_system(_registry);

        game::entities::create_text(_registry, {20.f, 30.f}, "R-Type", WHITE, 1.0f, 32);
        game::entities::create_sound(_registry, std::string(ASSETS_PATH) + "/sounds/BATTLE-PRESSURE.wav", 0.8f, true, true);
        
        if (!processPendingFullRegistry()) {
            std::cerr << "[WARN] Aucun full registry disponible lors de l'initialisation." << std::endl;
        }
        load_projectile_textures();
        load_music();
    }

    void GameScene::index_existing_entities() {
        _player = ecs::entity_t{0};
        _obstacles.clear();
        _enemys.clear();
        _elements.clear();
        _playerEntities.clear();

        auto &types = _registry.get_components<component::type>();
        auto &clientIds = _registry.get_components<component::client_id>();

        for (std::size_t i = 0; i < types.size(); ++i) {
            if (!types[i]) continue;

            ecs::entity_t entity = _registry.entity_from_index(i);

            switch (types[i]->value) {
                case component::entity_type::PLAYER:
                    if (clientIds.size() > i && clientIds[i]) {
                        uint32_t clientId = clientIds[i]->id;
                        _playerEntities.insert_or_assign(clientId, entity);
                        if (clientId == _game.getGameClient().clientId) {
                            _player = entity;
                        }
                    }
                    break;

                case component::entity_type::ENEMY:
                    _enemys.push_back(entity);
                    break;
                case component::entity_type::RANDOM_ELEMENT:
                    _elements.push_back(entity);
                    break;
                case component::entity_type::OBSTACLE:
                    _obstacles.push_back(entity);
                    break;

                default:
                    break;
            }
        }

        std::cout << "[DEBUG] Indexed entities: "
                << "Players=" << _playerEntities.size()
                << ", Enemies=" << _enemys.size()
                << ", Elements=" << _elements.size()
                << ", Obstacles=" << _obstacles.size() << std::endl;
    }

    void GameScene::load_projectile_textures() {
        _projectileTextures["player_missile"] = _raylib.loadTexture(std::string(ASSETS_PATH) + "/sprites/r-typesheet1.png");
        _projectileTextures["enemy_missile"] = _raylib.loadTexture(std::string(ASSETS_PATH) + "/sprites/r-typesheet1.png");
    }

    void GameScene::load_entity_textures() {
        auto &sprites = _registry.get_components<component::sprite>();
        auto &types = _registry.get_components<component::type>();
        auto &clientIds = _registry.get_components<component::client_id>();

        const std::string &selectedSkin = _game.getSelectedSkinPath();
        uint32_t localClientId = _game.getGameClient().clientId;

        for (std::size_t i = 0; i < sprites.size(); ++i) {
            if (sprites[i]) {
                if (!selectedSkin.empty() &&
                    i < types.size() && types[i] &&
                    types[i]->value == component::entity_type::PLAYER &&
                    i < clientIds.size() && clientIds[i] &&
                    clientIds[i]->id == localClientId &&
                    sprites[i]->image_path != selectedSkin) {
                    sprites[i]->image_path = selectedSkin;
                }

                if (sprites[i]->image_path.empty()) {
                    continue;
                }

                ecs::entity_t entity = _registry.entity_from_index(i);
                
                if (_entityTextures.find(entity.value()) == _entityTextures.end()) {
                    try {
                        Texture2D texture = _raylib.loadTexture(sprites[i]->image_path);
                        _entityTextures[entity.value()] = texture;
                        std::cout << "[DEBUG] Loaded texture for entity " << entity.value()
                                << " from " << sprites[i]->image_path << std::endl;
                    } catch (const std::exception &e) {
                        std::cerr << "[ERROR] Failed to load texture for entity " << entity.value()
                                << " from " << sprites[i]->image_path << ": " << e.what() << std::endl;
                    }
                }
            }
        }
    }

    void GameScene::unload_projectile_textures() {
        for (auto &pair : _projectileTextures) {
            _raylib.unloadTexture(pair.second);
        }
        _projectileTextures.clear();
        std::cout << "[DEBUG] Unloaded all projectile textures" << std::endl;
    }

    void GameScene::unload_entity_textures() {
        for (auto &pair : _entityTextures) {
            _raylib.unloadTexture(pair.second);
        }
        _entityTextures.clear();
        std::cout << "[DEBUG] Unloaded all entity textures" << std::endl;
    }

    Texture2D* GameScene::get_entity_texture(ecs::entity_t entity) {
        auto cachedIt = _entityTextures.find(entity.value());
        if (cachedIt != _entityTextures.end()) {
            return &cachedIt->second;
        }

        auto &sprites = _registry.get_components<component::sprite>();
        if (entity.value() < sprites.size() && sprites[entity.value()]) {
            const std::string &imagePath = sprites[entity.value()]->image_path;
            if (!imagePath.empty()) {
                try {
                    Texture2D texture = _raylib.loadTexture(imagePath);
                    auto [insertIt, inserted] = _entityTextures.emplace(entity.value(), texture);
                    if (inserted) {
                        std::cout << "[DEBUG] Lazily loaded texture for entity "
                                  << entity.value() << " from " << imagePath << std::endl;
                    }
                    return &insertIt->second;
                } catch (const std::exception &e) {
                    std::cerr << "[ERROR] Failed to lazily load texture for entity "
                              << entity.value() << " from " << imagePath
                              << ": " << e.what() << std::endl;
                }
            }
        }
        return nullptr;
    }

void GameScene::update() {
    if (!_game_running) return;

    std::unordered_map<uint32_t, std::tuple<float, float, float>> netPlayers;
    std::unordered_map<uint32_t, std::string> skinSelections;
    {
        std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
        netPlayers = _game.getGameClient().players;
        skinSelections = _game.getGameClient().playerSkins;
    }

    for (auto it = _playerEntities.begin(); it != _playerEntities.end(); ) {
        if (netPlayers.find(it->first) == netPlayers.end()) {
            _registry.kill_entity(it->second);
            it = _playerEntities.erase(it);
        } else {
            ++it;
        }
    }

    auto &positions = _registry.get_components<component::position>();
    const std::string &selectedSkinPath = _game.getSelectedSkinPath();
    uint32_t myClientId = _game.getGameClient().clientId;
    const std::string assetsPlayerDir = std::string(ASSETS_PATH) + "/sprites/player/";
    const std::string fallbackPlayerSkin = assetsPlayerDir + "r-typesheet42.png";
    for (auto const &kv : netPlayers) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);
        auto f = _playerEntities.find(id);
        if (f == _playerEntities.end()) {
            bool isLocalPlayer = (id == myClientId);
            std::string spritePath = fallbackPlayerSkin;
            auto skinIt = skinSelections.find(id);
            if (skinIt != skinSelections.end() && !skinIt->second.empty()) {
                spritePath = assetsPlayerDir + skinIt->second;
            }
            if (isLocalPlayer && !selectedSkinPath.empty()) {
                spritePath = selectedSkinPath;
            }

            ecs::entity_t e = game::entities::create_player(_registry, x, y, z, 32.f, 32.f, 0.f, spritePath, "", id);
            _playerEntities.emplace(id, e);
        } else {
            ecs::entity_t e = f->second;
            auto skinIt = skinSelections.find(id);
            std::string desiredPath = fallbackPlayerSkin;
            if (skinIt != skinSelections.end() && !skinIt->second.empty()) {
                desiredPath = assetsPlayerDir + skinIt->second;
            }
            if (id == myClientId && !selectedSkinPath.empty()) {
                desiredPath = selectedSkinPath;
            }

            auto &sprites = _registry.get_components<component::sprite>();
            if (e.value() < sprites.size() && sprites[e.value()]) {
                if (sprites[e.value()]->image_path != desiredPath && !desiredPath.empty()) {
                    auto texIt = _entityTextures.find(e.value());
                    if (texIt != _entityTextures.end()) {
                        _raylib.unloadTexture(texIt->second);
                        _entityTextures.erase(texIt);
                    }
                    sprites[e.value()]->image_path = desiredPath;
                }
            }

            if (e.value() < positions.size() && positions[e.value()]) {
                positions[e.value()]->x = x;
                positions[e.value()]->y = y;
                positions[e.value()]->z = z;
            }
        }
    }
    
    auto myPlayerIt = _playerEntities.find(myClientId);
    if (myPlayerIt != _playerEntities.end())
        _player = myPlayerIt->second;
    
    auto &pp = _registry.get_components<component::previous_position>();
    for (std::size_t i = 0; i < positions.size() && i < pp.size(); ++i) {
        if (positions[i] && pp[i]) {
            pp[i]->x = positions[i]->x; 
            pp[i]->y = positions[i]->y;
            pp[i]->z = positions[i]->z;
        }
    }

    std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float>> netEnemies;
    {
        std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
        netEnemies = _game.getGameClient().enemies;
    }

    for (auto it = _enemyMap.begin(); it != _enemyMap.end(); ) {
        uint32_t serverId = it->first;
        
        if (netEnemies.find(serverId) == netEnemies.end()) {
            ecs::entity_t clientEntity = it->second;
            _registry.kill_entity(clientEntity);
            _enemys.erase(
                std::remove(_enemys.begin(), _enemys.end(), clientEntity),
                _enemys.end()
            );
            it = _enemyMap.erase(it);
        } else {
            ++it;
        }
    }

    auto &velocities = _registry.get_components<component::velocity>();

    for (auto const &kv : netEnemies) {
        uint32_t serverId = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        float vz = std::get<5>(kv.second);
        float width = std::get<6>(kv.second);
        float height = std::get<7>(kv.second);
        
        auto it = _enemyMap.find(serverId);
        
        if (it == _enemyMap.end()) {
            std::string spritePath = std::string(ASSETS_PATH) + "/sprites/ennemies/r-typesheet19.png";
            
            auto spriteIt = _enemySpriteMap.find(serverId);
            if (spriteIt != _enemySpriteMap.end()) {
                spritePath = spriteIt->second;
            }
            ecs::entity_t newEnemy = game::entities::create_enemy(_registry, x, y, z, spritePath, width, height);
            _enemyMap.emplace(serverId, newEnemy);
            _enemys.push_back(newEnemy);
            if (newEnemy.value() < positions.size() && positions[newEnemy.value()]) {
                positions[newEnemy.value()]->x = x;
                positions[newEnemy.value()]->y = y;
                positions[newEnemy.value()]->z = z;
            }
            if (newEnemy.value() < velocities.size() && velocities[newEnemy.value()]) {
                velocities[newEnemy.value()]->vx = vx;
                velocities[newEnemy.value()]->vy = vy;
                velocities[newEnemy.value()]->vz = vz;
            }
        } else {
            ecs::entity_t clientEnemy = it->second;
            if (clientEnemy.value() < positions.size() && positions[clientEnemy.value()]) {
                positions[clientEnemy.value()]->x = x;
                positions[clientEnemy.value()]->y = y;
                positions[clientEnemy.value()]->z = z;
            }
            if (clientEnemy.value() < velocities.size() && velocities[clientEnemy.value()]) {
                velocities[clientEnemy.value()]->vx = vx;
                velocities[clientEnemy.value()]->vy = vy;
                velocities[clientEnemy.value()]->vz = vz;
            }
        }
    }

    std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float, float>> netObstacles;
    {
        std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
        netObstacles = _game.getGameClient().obstacles;
    }

    for (auto it = _obstacleMap.begin(); it != _obstacleMap.end(); ) {
        uint32_t serverId = it->first;
        
        if (netObstacles.find(serverId) == netObstacles.end()) {
            ecs::entity_t clientEntity = it->second;
            _registry.kill_entity(clientEntity);
            _obstacles.erase(
                std::remove(_obstacles.begin(), _obstacles.end(), clientEntity),
                _obstacles.end()
            );
            it = _obstacleMap.erase(it);
        } else {
            ++it;
        }
    }

    for (auto const &kv : netObstacles) {
        uint32_t serverId = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);
        float width = std::get<3>(kv.second);
        float height = std::get<4>(kv.second);
        float depth = std::get<5>(kv.second);
        float vx = std::get<6>(kv.second);
        float vy = std::get<7>(kv.second);
        float vz = std::get<8>(kv.second);
        
        auto it = _obstacleMap.find(serverId);
        
        if (it == _obstacleMap.end()) {
            std::string spritePath = std::string(ASSETS_PATH) + "/sprites/obstacles/default_obstacle.png";
            
            auto spriteIt = _obstacleSpriteMap.find(serverId);
            if (spriteIt != _obstacleSpriteMap.end()) {
                spritePath = spriteIt->second;
            }
            
            ecs::entity_t newObstacle = game::entities::create_obstacle(
                _registry, x, y, z, spritePath, "", vx, width, height
            );
            
            _obstacleMap.emplace(serverId, newObstacle);
            _obstacles.push_back(newObstacle);
            
            if (newObstacle.value() < positions.size() && positions[newObstacle.value()]) {
                positions[newObstacle.value()]->x = x;
                positions[newObstacle.value()]->y = y;
                positions[newObstacle.value()]->z = z;
            }
            if (newObstacle.value() < velocities.size() && velocities[newObstacle.value()]) {
                velocities[newObstacle.value()]->vx = vx;
                velocities[newObstacle.value()]->vy = vy;
                velocities[newObstacle.value()]->vz = vz;
            }
        } else {
            ecs::entity_t clientObstacle = it->second;

            if (clientObstacle.value() < velocities.size() && velocities[clientObstacle.value()]) {
                velocities[clientObstacle.value()]->vx = vx;
                velocities[clientObstacle.value()]->vy = vy;
                velocities[clientObstacle.value()]->vz = vz;
            }
        }
    }

       std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float>> netElements;
    {
        std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
        netElements = _game.getGameClient().elements;
    }

    if (!netElements.empty() && _elementMap.empty()) {
        std::cout << "[DEBUG] Received " << netElements.size() << " elements from network" << std::endl;
    }

    for (auto it = _elementMap.begin(); it != _elementMap.end(); ) {
        uint32_t serverId = it->first;
        
        if (netElements.find(serverId) == netElements.end()) {
            ecs::entity_t clientEntity = it->second;
            std::cout << "[DEBUG] Removing element serverId=" << serverId << std::endl;
            _registry.kill_entity(clientEntity);
            _elements.erase(
                std::remove(_elements.begin(), _elements.end(), clientEntity),
                _elements.end()
            );
            it = _elementMap.erase(it);
        } else {
            ++it;
        }
    }

    for (auto const &kv : netElements) {
        uint32_t serverId = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        float vz = std::get<5>(kv.second);
        float width = std::get<6>(kv.second);
        float height = std::get<7>(kv.second);
        
        auto it = _elementMap.find(serverId);
        
        if (it == _elementMap.end()) {
            std::string spritePath = "";

            auto spriteIt = _elementSpriteMap.find(serverId);
            if (spriteIt != _elementSpriteMap.end()) {
                spritePath = spriteIt->second;
            }
            
            ecs::entity_t newElement = game::entities::create_random_element(
                _registry, x, y, z, spritePath, width, height, "powerup", 0.0f
            );

            auto texIt = _entityTextures.find(newElement.value());
            if (texIt != _entityTextures.end()) {
                std::cout << "[DEBUG] Clearing old texture for entity " << newElement.value() << std::endl;
                _raylib.unloadTexture(texIt->second);
                _entityTextures.erase(texIt);
            }
            
            _elementMap.emplace(serverId, newElement);
            _elements.push_back(newElement);
                
            if (newElement.value() < positions.size() && positions[newElement.value()]) {
                positions[newElement.value()]->x = x;
                positions[newElement.value()]->y = y;
                positions[newElement.value()]->z = z;
            }
            if (newElement.value() < velocities.size() && velocities[newElement.value()]) {
                velocities[newElement.value()]->vx = vx;
                velocities[newElement.value()]->vy = vy;
                velocities[newElement.value()]->vz = vz;
            }
        } else {
            ecs::entity_t clientElement = it->second;
            if (clientElement.value() < positions.size() && positions[clientElement.value()]) {
                positions[clientElement.value()]->x = x;
                positions[clientElement.value()]->y = y;
                positions[clientElement.value()]->z = z;
            }
            if (clientElement.value() < velocities.size() && velocities[clientElement.value()]) {
                velocities[clientElement.value()]->vx = vx;
                velocities[clientElement.value()]->vy = vy;
                velocities[clientElement.value()]->vz = vz;
            }
        }
    }
    float dt = _raylib.getFrameTime();
    std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
    for (auto &kv : _game.getGameClient().projectiles) {
        float &x = std::get<0>(kv.second);
        float &y = std::get<1>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        x += vx * dt;
        y += vy * dt;
    }
    for (auto &kv : _game.getGameClient().enemyProjectiles) {
        float &x = std::get<0>(kv.second);
        float &y = std::get<1>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        x += vx * dt;
        y += vy * dt;
    }
    _registry.run_systems();
}

    void GameScene::clearLevelEntitiesForReload() {
        auto &types = _registry.get_components<component::type>();
        std::vector<ecs::entity_t> toKill;

        for (std::size_t i = 0; i < types.size(); ++i) {
            if (!types[i])
                continue;

            switch (types[i]->value) {
                case component::entity_type::ENEMY:
                case component::entity_type::OBSTACLE:
                case component::entity_type::PROJECTILE:
                case component::entity_type::RANDOM_ELEMENT:
                case component::entity_type::BACKGROUND:
                case component::entity_type::PLAYER:
                    toKill.push_back(_registry.entity_from_index(i));
                    break;
                default:
                    break;
            }
        }

        for (auto entity : toKill) {
            auto texIt = _entityTextures.find(entity.value());
            if (texIt != _entityTextures.end()) {
                _raylib.unloadTexture(texIt->second);
                _entityTextures.erase(texIt);
            }
            _registry.kill_entity(entity);
        }

        _enemyMap.clear();
        _obstacleMap.clear();
        _elementMap.clear();
        _enemys.clear();
        _obstacles.clear();
        _elements.clear();
        _playerEntities.clear();
        moovePlayer.clear();
        _player = ecs::entity_t{0};
    }

    void GameScene::removeEntitiesOfType(component::entity_type type) {
        auto &types = _registry.get_components<component::type>();
        std::vector<ecs::entity_t> toKill;

        for (std::size_t i = 0; i < types.size(); ++i) {
            if (types[i] && types[i]->value == type) {
                toKill.push_back(_registry.entity_from_index(i));
            }
        }

        for (auto entity : toKill) {
            auto texIt = _entityTextures.find(entity.value());
            if (texIt != _entityTextures.end()) {
                _raylib.unloadTexture(texIt->second);
                _entityTextures.erase(texIt);
            }
            _registry.kill_entity(entity);
        }
    }

    void GameScene::buildSpriteMapsFromRegistry(const nlohmann::json &registryJson) {
        _enemySpriteMap.clear();
        _obstacleSpriteMap.clear();
        _elementSpriteMap.clear();

        if (registryJson.is_null())
            return;

        const nlohmann::json &entities = registryJson.contains("entities")
            ? registryJson.at("entities")
            : registryJson;

        if (!entities.is_array())
            return;

        for (const auto &entity : entities) {
            if (!entity.contains("entity_id") || !entity.contains("type"))
                continue;

            uint32_t serverEntityId = entity.at("entity_id").get<uint32_t>();
            int typeValue = entity.at("type").get<int>();
            auto imageIt = entity.find("image_path");
            if (imageIt == entity.end() || !imageIt->is_string())
                continue;

            std::string imagePath = imageIt->get<std::string>();

            switch (static_cast<component::entity_type>(typeValue)) {
                case component::entity_type::ENEMY:
                    if (!imagePath.empty())
                        _enemySpriteMap[serverEntityId] = normalizeNetworkAssetPath(imagePath);
                    break;
                case component::entity_type::OBSTACLE:
                    if (!imagePath.empty())
                        _obstacleSpriteMap[serverEntityId] = normalizeNetworkAssetPath(imagePath);
                    break;
                case component::entity_type::RANDOM_ELEMENT:
                    if (!imagePath.empty())
                        _elementSpriteMap[serverEntityId] = normalizeNetworkAssetPath(imagePath);
                    break;
                default:
                    break;
            }
        }
    }

    bool GameScene::processPendingFullRegistry() {
        auto registryOpt = _game.getGameClient().consumeFullRegistry();
        if (!registryOpt.has_value())
            return false;

        const nlohmann::json &fullRegistry = registryOpt.value();

        if (_hasLevelData) {
            clearLevelEntitiesForReload();
            game::serializer::deserialize_entities(_registry, fullRegistry);
        }

        buildSpriteMapsFromRegistry(fullRegistry);
        removeEntitiesOfType(component::entity_type::ENEMY);
        removeEntitiesOfType(component::entity_type::OBSTACLE);
        load_entity_textures();
        index_existing_entities();
        _hasLevelData = true;
        return true;
    }

    void GameScene::load_music() {
        _raylib.initAudioDevice();

        auto &audios = _registry.get_components<component::audio>();

        for (std::size_t i = 0; i < audios.size(); ++i) {
            if (!audios[i]) continue;

            const auto &audio = *audios[i];
            if (!std::filesystem::exists(audio.sound_path))
                continue;
            _music = _raylib.loadMusicStream(audio.sound_path);
            _music.looping = audio.loop;
            _raylib.setMusicVolume(_music, audio.volume);
            if (_game.isSoundEnabled()) {
                _raylib.playMusicStream(_music);
            }
            _raylib.updateMusicStream(_music);
            break;
        }
         _shootSound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/shoot.wav");
        _raylib.setSoundVolume(_shootSound, 0.8f);
        _victorySound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/victory.wav");
        _raylib.setSoundVolume(_victorySound, 0.8f);
        _defeatSound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/defeat.wav");
        _raylib.setSoundVolume(_defeatSound, 0.8f);
    }

    Color GameScene::get_color_for_id(uint32_t id) {
        static Color palette[] = {
            RAYWHITE, BLUE, GREEN, YELLOW, ORANGE,
            PURPLE, PINK, GOLD, LIME, SKYBLUE
        };
        return palette[id % (sizeof(palette)/sizeof(palette[0]))];
    }

    void GameScene::handleEvents() {
        update();
        float deltaTime = _raylib.getFrameTime();
        _chat.update(deltaTime);
        int globalScore = 0;
        uint32_t myClientId = 0;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            globalScore = _game.getGameClient().globalScore;
            myClientId = _game.getGameClient().clientId;
        }
        const auto &weaponDef = weapon::getDefinition(_game.getSelectedWeaponId());
        float progress = std::clamp(globalScore / 150.0f, 0.0f, 1.0f);
        float SHOOT_COOLDOWN = weaponDef.fireCooldown - progress * (weaponDef.fireCooldown - weaponDef.minCooldown);
        SHOOT_COOLDOWN = std::clamp(SHOOT_COOLDOWN, weaponDef.minCooldown, weaponDef.fireCooldown);

        bool upPressed = _raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP);
        bool downPressed = _raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN);
        bool leftPressed = _raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT);
        bool rightPressed = _raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT);

        int keyPressed = _raylib.getKeyPressed();

        bool shootRequested = _raylib.isKeyDown(KEY_SPACE);

        switch (keyPressed) {
            case KEY_F1:
                _chat.toggleFocus();
                break;
            case KEY_F11:
                toggleFullScreen();
                break;
            default:
                break;
        }

        if (_chat.isFocused()) {
            if (keyPressed == KEY_ENTER) {
                if (auto submitted = _chat.submitMessage()) {
                    _game.getGameClient().sendChatMessage(*submitted);
                }
            } else if (keyPressed == KEY_BACKSPACE) {
                _chat.removeLastCharacter();
            } else if (keyPressed == KEY_ESCAPE) {
                _chat.toggleFocus();
                dispatch_input_events(false, false, false, false);
                if (myClientId != 0) {
                    moovePlayer[myClientId] = 0.0f;
                }
                return;
            }

            int codepoint = 0;
            while ((codepoint = _raylib.getCharPressed()) != 0) {
                _chat.appendCharacter(codepoint);
            }

            dispatch_input_events(false, false, false, false);
            if (myClientId != 0) {
                moovePlayer[myClientId] = 0.0f;
            }
            return;
        }

        if (_raylib.isGamepadAvailable(0)) {
            float leftStickX = _raylib.getGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            float leftStickY = _raylib.getGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

            if (leftStickY < -0.2f)
                upPressed = true;
            else if (leftStickY > 0.2f)
                downPressed = true;

            if (leftStickX < -0.2f)
                leftPressed = true;
            else if (leftStickX > 0.2f)
                rightPressed = true;

            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
                downPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
                upPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                rightPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
                leftPressed = true;

            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                shootRequested = true;
        }

        if (shootRequested && !_stopShoot && !_chat.isFocused()) {
            handle_shoot(weaponDef, SHOOT_COOLDOWN);
        }

        dispatch_input_events(upPressed, downPressed, leftPressed, rightPressed);

        float input_x = 0.f;
        float input_y = 0.f;
        if (leftPressed != rightPressed)
            input_x = leftPressed ? -1.f : 1.f;
        if (upPressed != downPressed)
            input_y = upPressed ? -1.f : 1.f;

        if (myClientId != 0) {
            if (input_y < 0.f)
                moovePlayer[myClientId] = 33.0f;
            else if (input_y > 0.f)
                moovePlayer[myClientId] = -33.0f;
            else
                moovePlayer[myClientId] = 0.0f;
        }

        handle_input(input_x, input_y);
    }

    void GameScene::handle_shoot(const weapon::WeaponDefinition &weaponDef, float cooldown) {
        const std::string weaponId = !weaponDef.id.empty() ? weaponDef.id : _game.getSelectedWeaponId();
        auto &state = _weaponUsage[weaponId];
        float currentTime = _raylib.getTime();
        bool usesBurst = weaponDef.burstDuration > 0.f && weaponDef.burstCooldown > 0.f;

        if (!weaponDef.infiniteAmmo && state.remainingAmmo < 0) {
            state.remainingAmmo = weaponDef.ammoCapacity;
        }

        if (usesBurst) {
            if (state.inBurst) {
                float elapsed = currentTime - state.burstStartTime;
                if (elapsed >= weaponDef.burstDuration) {
                    state.inBurst = false;
                    state.lastBurstEndTime = currentTime;
                }
            }

            if (!state.inBurst) {
                if (state.lastBurstEndTime >= 0.f) {
                    float cooldownElapsed = currentTime - state.lastBurstEndTime;
                    if (cooldownElapsed < weaponDef.burstCooldown) {
                        return;
                    }
                }
                state.inBurst = true;
                state.burstStartTime = currentTime;
                if (!weaponDef.infiniteAmmo) {
                    state.remainingAmmo = weaponDef.ammoCapacity;
                }
            }
        } else {
            state.inBurst = false;
        }

        if (currentTime - state.lastShotTime < cooldown) {
            return;
        }

        if (!weaponDef.infiniteAmmo) {
            if (state.remainingAmmo <= 0) {
                if (usesBurst) {
                    state.inBurst = false;
                    state.lastBurstEndTime = currentTime;
                }
                return;
            }
            state.remainingAmmo--;
        }

        _game.getGameClient().sendShoot();
        state.lastShotTime = currentTime;
        if (usesBurst && !weaponDef.infiniteAmmo && state.remainingAmmo <= 0) {
            state.inBurst = false;
            state.lastBurstEndTime = currentTime;
        }
        if (_game.isSoundEnabled()) {
            _raylib.playSound(_shootSound);
        }
    }

    void GameScene::handle_input(float input_x, float input_y) {
        if (_isDead)
            return;
        auto &positions = _registry.get_components<component::position>();
        auto &controls  = _registry.get_components<component::controllable>();
        auto &hitboxes  = _registry.get_components<component::collision_box>();
        uint32_t myClientId = _game.getGameClient().clientId;
        auto myPlayerIt = _playerEntities.find(myClientId);
        if (myPlayerIt == _playerEntities.end())
            return;

        ecs::entity_t myPlayer = myPlayerIt->second;

        if (myPlayer.value() < positions.size() && positions[myPlayer.value()] &&
            myPlayer.value() < controls.size() && controls[myPlayer.value()]) {
            float speed = controls[myPlayer.value()]->speed;
            float dt = 0.016f;

            ecs::entity_t playerHitbox = collision::find_player_hitbox(*this);
            if (playerHitbox.value() < hitboxes.size() && hitboxes[playerHitbox.value()]) {
                auto &playerPos = *positions[myPlayer.value()];
                auto &playerBox = *hitboxes[playerHitbox.value()];
                float ix = input_x;
                float iy = input_y;
                float testX = playerPos.x + ix * speed * dt;
                float testY = playerPos.y + iy * speed * dt;

                if (collision::is_blocked(*this, testX, playerPos.y, playerPos, playerBox))
                    ix = 0.f;
                if (collision::is_blocked(*this, playerPos.x, testY, playerPos, playerBox))
                    iy = 0.f;
                if (ix != 0.f || iy != 0.f) {
                    playerPos.x += ix * speed * dt;
                    playerPos.y += iy * speed * dt;
                }
            }
        }
    }

    void GameScene::dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed) {
        auto sendIfChanged = [this](InputCode code, bool newState, bool &cachedState) {
            if (newState == cachedState)
                return;
            cachedState = newState;
            _game.getGameClient().sendInputEvent(code, newState);
        };

        sendIfChanged(InputCode::Up, upPressed, _inputState.up);
        sendIfChanged(InputCode::Down, downPressed, _inputState.down);
        sendIfChanged(InputCode::Left, leftPressed, _inputState.left);
        sendIfChanged(InputCode::Right, rightPressed, _inputState.right);
    }

    void GameScene::setup_movement_system() {
        _registry.add_system<component::position, component::velocity>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::position> &pos,
               ecs::sparse_array<component::velocity> &vel) {
                float dt = 0.016f;
                for (std::size_t i = 0; i < pos.size() && i < vel.size(); ++i) {
                    if (pos[i] && vel[i]) {
                        pos[i]->x += vel[i]->vx * dt;
                        pos[i]->y += vel[i]->vy * dt;
                        pos[i]->z += vel[i]->vz * dt;
                    }
                }
            });
    }

    void GameScene::setup_render_system() {
        _registry.add_system<component::position, component::drawable>(
            [this](ecs::registry &reg,
                   ecs::sparse_array<component::position> &pos,
                   ecs::sparse_array<component::drawable> &drw) {
            });
    }

    void GameScene::setup_health_system() {
        _registry.add_system<component::health, component::type>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::health> &health,
               ecs::sparse_array<component::type> &type) {
                for (std::size_t i = 0; i < health.size() && i < type.size(); ++i) {
                    if (health[i] && type[i] && health[i]->current <= 0) {
                        if (type[i]->value == component::entity_type::PLAYER) {
                            reg.remove_component<component::drawable>(reg.entity_from_index(i));
                        } else {
                            ecs::entity_t entity = reg.entity_from_index(i);
                            reg.kill_entity(entity);
                        }
                    }
                }
            });
    }

    void GameScene::toggleFullScreen() {
        int const currentMonitor = _raylib.getCurrentMonitor();

        if (_raylib.isWindowFullscreen()) {
            int posX = (_raylib.getMonitorWidth(currentMonitor) - getWindowWidth()) / 2;
            int posY = (_raylib.getMonitorHeight(currentMonitor) - getWindowHeight()) / 2;
            _raylib.setWindowSize(getWindowWidth() - 1, getWindowHeight() - 1);
            _raylib.setWindowPosition(posX, posY);
        } else {
            _raylib.setWindowSize(getWindowWidth(), getWindowHeight());
        }
        _raylib.setWindowMonitor(currentMonitor);
        _raylib.toggleFullscreen();
    }

    void GameScene::onClose() {
        _raylib.unloadSound(_shootSound);
        _raylib.unloadSound(_victorySound);
        _raylib.unloadSound(_defeatSound);
        _raylib.stopMusicStream(_music);
        _raylib.unloadMusicStream(_music);
        _raylib.closeAudioDevice();
        _game_running = false;
        _ui.unload();
        unload_entity_textures();
        unload_projectile_textures();
    }
} // namespace game::scene
