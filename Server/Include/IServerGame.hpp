/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** IServerGame
*/

#pragma once

#include "connexion.hpp"
#include "../../Shared/protocol.hpp"
#include "../../Engine/Utils/Include/serializer.hpp"
#include "../../Engine/Core/Include/registry.hpp"
#include "../../Engine/Utils/Include/entity_storage.hpp"
#include <cstdint>
#include <vector>

class IServerGame {
    public:
        virtual ~IServerGame() = default;

        virtual void run(int roomId) = 0;
        virtual void enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) = 0;
        virtual void setInitialClients(const std::vector<uint32_t> &clients) = 0;
};
