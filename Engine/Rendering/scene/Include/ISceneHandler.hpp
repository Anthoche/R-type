/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ISceneHandler
*/

/*
** EPITECH PROJECT, 2025
** R-Type Engine
** File description:
** ISceneHandler
*/

#pragma once

#include <string>
#include <memory>
#include "IScene.hpp"
#include "../../Raylib.hpp"

/**
 * @interface ISceneHandler
 * @brief Generic interface for scene management.
 *
 * Defines how scenes are opened, closed, and retrieved.
 * Each game (e.g., R-Type, TowerDefense) can implement its own scene handler.
 */
class ISceneHandler {
    public:
        /**
         * @brief Virtual destructor for interface safety.
         */
        virtual ~ISceneHandler() = default;

        /**
         * @brief Opens a scene by name.
         * 
         * @param name The name of the scene to open.
         */
        virtual void open(const std::string &name) = 0;

        /**
         * @brief Opens the default entry scene (e.g., main menu).
         */
        virtual void openDefault() = 0;

        /**
         * @brief Retrieves a scene instance by name.
         * 
         * @param name The name of the scene to retrieve.
         * @return Shared pointer to the scene, or nullptr if not found.
         */
        virtual std::shared_ptr<IScene> getScene(const std::string &name) = 0;

        /**
         * @brief Closes all currently opened scenes.
         */
        virtual void closeOpened() = 0;
};
