/*
** EPITECH PROJECT, 2025
** R-Type Engine
** File description:
** ASceneHandler
*/

#pragma once

#include <map>
#include <memory>
#include <string>
#include "ISceneHandler.hpp"
#include "IScene.hpp"

/**
 * @class ASceneHandler
 * @brief Abstract base class providing generic scene handling behavior.
 *
 * Stores and manages scene objects, leaving rendering and game-specific setup
 * to the concrete implementation.
 */
class ASceneHandler : public ISceneHandler {
    protected:
        /**
         * @brief Map associating scene names to pairs of scene objects and their open state.
         */
        std::map<std::string, std::pair<std::shared_ptr<IScene>, bool>> _scenes;

        /**
         * @brief Adds a scene to the internal scene map.
         *
         * @param name The name of the scene.
         * @param scene Shared pointer to the scene instance.
         */
        void addScene(const std::string &name, const std::shared_ptr<IScene> &scene);

    public:
        virtual ~ASceneHandler() = default;

        /**
         * @brief Retrieves a scene instance by name.
         *
         * @param name The name of the scene to retrieve
         * @return Shared pointer to the scene, or nullptr if not found
         */
        std::shared_ptr<IScene> getScene(const std::string &name) override;

        /**
         * @brief Closes all currently opened scenes
         */
        void closeOpened() override;

        /**
         * @brief Pure virtual: opens a specific scene by name
         */
        virtual void open(const std::string &name) override = 0;

        /**
         * @brief Pure virtual: opens the default scene
         */
        virtual void openDefault() override = 0;
};
