/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "client.hpp"
#include "Logger.hpp"
#include "../Engine/Game.hpp"
#include "../Engine/Utils/Include/serializer.hpp"
#include <asio.hpp>
#include <cstring>

GameClient::GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name)
    : socket(), clientName(name), _game(game), serverIpStr(serverIp) {
    try {
        serverEndpoint = asio::ip::udp::endpoint(
            asio::ip::address::from_string(serverIp), 
            serverPort
        );
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid server IP: " + std::string(e.what()));
    }
}

GameClient::~GameClient() {
    if (running)
        disconnect();
}

void GameClient::run() {
    running = true;
    rxThread = std::thread(&GameClient::recvLoop, this);
    while (_game.getGameStatus() != GameStatus::RUNNING) {
        if (!running)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void GameClient::disconnect() {
    running = false;
    if (rxThread.joinable()) rxThread.join();
}


void GameClient::sendHello() {
    ClientHelloMessage msg;
    msg.type = MessageType::ClientHello;
    msg.clientId = htonl(0);
    strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
    msg.clientName[sizeof(msg.clientName) - 1] = '\0';
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendRoomAsk(uint32_t room_id) {
    ClientRoomIdAskMessage msg;
    msg.type = MessageType::ClientRoomIdAsk;
    msg.clientId = clientId;
    msg.roomId = room_id;
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendRoomsFetch() {
    ClientFetchRoomsMessage msg;
    msg.type = MessageType::ClientFetchRooms;
    msg.clientId = clientId;
    {
        std::lock_guard<std::mutex> lock(roomsMutex);
        rooms.clear();
        roomsUpdated = false;
    }
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendRoomCreate(uint16_t minPlayers, uint16_t maxPlayers, const std::string &roomName) {
    ClientRoomCreateMessage msg{};
    msg.type = MessageType::ClientRoomCreate;
    msg.clientId = clientId;
    msg.minPlayers = htons(minPlayers);
    msg.maxPlayers = htons(maxPlayers);
    std::memset(msg.roomName, 0, sizeof(msg.roomName));
    std::string trimmed = roomName;
    if (trimmed.size() > sizeof(msg.roomName) - 1)
        trimmed.resize(sizeof(msg.roomName) - 1);
    std::strncpy(msg.roomName, trimmed.c_str(), sizeof(msg.roomName) - 1);
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

bool GameClient::waitForRooms(std::chrono::milliseconds timeout,
                              std::map<int, game::serializer::RoomData> &outRooms) {
    std::unique_lock<std::mutex> lock(roomsMutex);
    if (!roomsCv.wait_for(lock, timeout, [this]() { return roomsUpdated; }))
        return false;
    outRooms = rooms;
    return true;
}

void GameClient::sendConfirmStart() {
    ClientConfirmStartMessage msg;
    msg.type = MessageType::ClientConfirmStart;
    msg.clientId = clientId;
    msg.roomId = roomId;
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendClientLeaveRoom() {
    ClientLeaveRoomMessage msg;
    msg.type = MessageType::ClientLeaveRoom;
    msg.clientId = clientId;
    msg.roomId = roomId;
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::initTcpConnection() {
    if (clientId == 0) return;

    uint16_t tcpPort = 5000 + clientId;
    tcpClient = std::make_unique<TCP_socket>();

    if (!tcpClient->connectToServer(serverIpStr, tcpPort)) {
        return;
    }
}

void GameClient::recvLoop() {
    while (running) {
        std::vector<uint8_t> buffer;
        asio::ip::udp::endpoint fromEndpoint;
        if (socket.try_receive(buffer, fromEndpoint)) {
            if (buffer.size() < sizeof(MessageType)) {
                continue;
            }
            MessageType type = *reinterpret_cast<MessageType *>(buffer.data());
            handleMessage(type, buffer);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void GameClient::sendInputEvent(InputCode code, bool pressed) {
    if (clientId == 0)
        return;
    ClientInputMessage m{};
    m.type = MessageType::ClientInput;
    m.clientId = htonl(clientId);
    m.inputCode = static_cast<uint8_t>(code);
    m.isPressed = pressed ? 1 : 0;
    socket.sendTo(&m, sizeof(m), serverEndpoint);
}

void GameClient::sendSceneState(SceneState scene, ecs::registry* registry) {
    SceneStateMessage msg{};
    msg.type = MessageType::SceneState;
    msg.clientId = htonl(clientId);
    msg.scene = htonl(static_cast<uint32_t>(scene));
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);

    if (scene == SceneState::GAME) {
        if (!tcpClient) {
            initTcpConnection();
            if (!tcpClient) {
                return;
            }
        }
        nlohmann::json fullRegistry = tcpClient->receiveJson();
        if (fullRegistry.is_null()) {
            return;
        }
        storeFullRegistry(fullRegistry, true);
        if (registry) {
            game::serializer::deserialize_entities(*registry, fullRegistry);
        }
    }
}

void GameClient::sendShoot() {
    if (clientId == 0)
        return;
    ClientShootMessage msg;
    msg.type = MessageType::ClientShoot;
    msg.clientId = htonl(clientId);
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendHealth(int health) {
    if (clientId == 0)
        return;
    InitialHealthMessage msg;
    msg.type = MessageType::InitialHealth;
    msg.clientId = htonl(clientId);
    msg.initialHealth = htons(static_cast<int16_t>(health));
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendEndlessMode(bool isEndless) {
    EndlessModeMessage msg;
    msg.type = MessageType::EndlessMode;
    msg.clientId = htonl(clientId);
    msg.isEndless = isEndless ? 1 : 0;
    
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::sendChatMessage(const std::string &message) {
    if (clientId == 0 || message.empty())
        return;
    ChatMessagePacket packet{};
    packet.type = MessageType::ChatMessage;
    packet.senderId = htonl(clientId);
    std::memset(packet.senderName, 0, sizeof(packet.senderName));
    std::memset(packet.message, 0, sizeof(packet.message));
    std::strncpy(packet.senderName, clientName.c_str(), sizeof(packet.senderName) - 1);
    std::strncpy(packet.message, message.c_str(), sizeof(packet.message) - 1);
    socket.sendTo(&packet, sizeof(packet), serverEndpoint);
}

bool GameClient::hasConnectionFailed() const {
    return connectionFailed;
}

void GameClient::sendSkinSelection(const std::string &skinFilename) {
    if (skinFilename.empty()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        pendingSkinSelection = skinFilename;
        if (clientId != 0) {
            playerSkins[clientId] = skinFilename;
        }
    }

    if (clientId == 0) {
        return;
    }

    PlayerSkinMessage msg{};
    msg.type = MessageType::PlayerSkinUpdate;
    msg.clientId = htonl(clientId);
    std::memset(msg.skinFilename, 0, sizeof(msg.skinFilename));
    std::strncpy(msg.skinFilename, skinFilename.c_str(), sizeof(msg.skinFilename) - 1);
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (pendingSkinSelection == skinFilename) {
            pendingSkinSelection.clear();
        }
    }
}

void GameClient::sendWeaponSelection(const std::string &weaponId) {
    if (weaponId.empty()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        pendingWeaponSelection = weaponId;
        if (clientId != 0) {
            playerWeapons[clientId] = weaponId;
        }
    }

    if (clientId == 0) {
        return;
    }

    PlayerWeaponMessage msg{};
    msg.type = MessageType::PlayerWeaponUpdate;
    msg.clientId = htonl(clientId);
    std::memset(msg.weaponId, 0, sizeof(msg.weaponId));
    std::strncpy(msg.weaponId, weaponId.c_str(), sizeof(msg.weaponId) - 1);
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (pendingWeaponSelection == weaponId) {
            pendingWeaponSelection.clear();
        }
    }
}

void GameClient::storeFullRegistry(const nlohmann::json &registryJson, bool markPending) {
    std::lock_guard<std::mutex> lock(registryMutex);
    latestFullRegistry = registryJson;
    if (markPending) {
        hasPendingFullRegistry.store(true, std::memory_order_release);
    }
}

std::optional<nlohmann::json> GameClient::consumeFullRegistry() {
    std::lock_guard<std::mutex> lock(registryMutex);
    if (!hasPendingFullRegistry.load(std::memory_order_acquire)) {
        return std::nullopt;
    }
    hasPendingFullRegistry.store(false, std::memory_order_release);
    return latestFullRegistry;
}

void GameClient::fetchFullRegistryAsync() {
    if (!tcpClient || !tcpClient->isConnected()) {
        return;
    }

    bool expected = false;
    if (!fullRegistryFetchInFlight.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return;
    }

    std::thread([this]() {
        nlohmann::json fullRegistry = tcpClient->receiveJson();
        if (!fullRegistry.is_null()) {
            storeFullRegistry(fullRegistry, true);
        }
        fullRegistryFetchInFlight.store(false, std::memory_order_release);
    }).detach();
}

const std::string &GameClient::getClientName() const {
    return clientName;
}

std::vector<std::pair<std::string, std::string>> GameClient::consumeChatMessages() {
    std::vector<std::pair<std::string, std::string>> messages;
    std::lock_guard<std::mutex> lock(stateMutex);
    while (!_chatQueue.empty()) {
        messages.emplace_back(std::move(_chatQueue.front()));
        _chatQueue.pop_front();
    }
    return messages;
}
