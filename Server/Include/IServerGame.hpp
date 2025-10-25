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

class IServerGame {
    public:
        virtual ~IServerGame() = default;

        virtual void run(int roomId) = 0;
};
