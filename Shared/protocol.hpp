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
    GameStart
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