/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** protocol
*/

#pragma once

#include <cstdint>
#include <string>


enum class MessageType : uint8_t {
    ClientHello,
    ServerAssignId,
    GameStart,
    ClientInput,
    StateUpdate
};

struct ClientHelloMessage {
    MessageType type;
    uint32_t clientId;
    char clientName[32];
};

struct GameStartMessage {
    MessageType type;
    uint32_t clientCount;
};

struct ServerAssignIdMessage {
    MessageType type;
    uint32_t clientId; // network byte order
};

// Client -> Server: input vector for this frame (floats encoded as uint32 network order)
struct ClientInputMessage {
    MessageType type;         // MessageType::ClientInput
    uint32_t clientId;        // network byte order
    uint32_t inputXBits;      // network byte order (reinterpret cast of float)
    uint32_t inputYBits;      // network byte order (reinterpret cast of float)
};

// Server -> Client: authoritative state for the local player (extend later)
struct StateUpdateMessage {
    MessageType type;         // MessageType::StateUpdate
    uint32_t clientId;        // network byte order (the recipient id)
    uint32_t posXBits;        // network byte order (reinterpret cast of float)
    uint32_t posYBits;        // network byte order (reinterpret cast of float)
};