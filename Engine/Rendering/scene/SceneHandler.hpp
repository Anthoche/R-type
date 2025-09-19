/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SceneHandler
*/

#ifndef SCENEHANDLER_HPP
    #define SCENEHANDLER_HPP

    #include <map>
    #include <memory>
    #include "IScene.hpp"
    #include "../Raylib.hpp"

class Game;

/**
 * @class SceneHandler
 * @brief Manages multiple windows within the GUI.
 *
 * This class handles the creation, opening, and closing of windows,
 * maintaining a map of scene names to their objects and visibility states.
 */
class SceneHandler {
    private:
        /**
         * @brief Raylib wrapper instance
         */
        Raylib _raylib; // Temporaire

        /**
         * @brief Game instance
         */
        Game &_game;

        /**
         * @brief Map associating scene names to pairs of scene objects and their open state.
         *
         * The bool indicates whether the scene is currently open (true) or closed (false).
         */
        std::map<std::string, std::pair<std::shared_ptr<IScene>, bool>> _scenes;

        /**
         * @brief Adds a scene to the internal scene map.
         *
         * @param name The name of the scene.
         * @param scene Shared pointer to the scene instance.
         */
        void addScene(std::string const &name, std::shared_ptr<IScene> const &scene);

        /**
         * @brief Closes all currently opened windows.
         */
        void closeOpened();

        /**
         * @brief Initializes and sets up the windows.
         */
        void setupScenes();

        /**
         * @brief Gets the index of a scene by name.
         *
         * @param name The name of the scene.
         * @return The index of the scene, or -1 if not found.
         */
        int getSceneIndex(std::string const &name) const;

    public:
        /**
         * @brief Constructs a new WindowHandler object.
         */
        SceneHandler(Game &game);

        /**
         * @brief Default destructor.
         */
        ~SceneHandler() = default;

        /**
         * @brief Opens a scene by name.
         *
         * @param name The name of the scene to open.
         */
        void open(std::string const &name);

        /**
         * @brief Opens the main menu scene.
         */
        void openMenu();

        /**
         * @brief Gets the current scene.
         *
         * @return The currently opened scene.
         */
        std::shared_ptr<IScene> getScene(std::string const &name);

};

#endif //SCENEHANDLER_HPP
