/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#ifndef MENUSCENE_HPP
    #define MENUSCENE_HPP

    #include "../../Engine/Rendering/scene/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

/**
 * @class MenuScene
 * @brief Represents the main menu scene of the game.
 *
 * The menu scene handles:
 * - Initialization of menu UI and resources.
 * - Rendering menu elements.
 * - Handling user input/events (e.g., navigating the menu).
 * - Cleaning up when the scene is closed.
 */
class MenuScene: public AScene {
    public:
        /**
         * @brief Construct a MenuScene with a reference to the SceneHandler.
         * @param game The game instance.
         */
        MenuScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~MenuScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initialize the menu scene (UI, buttons, resources).
         */
        void init() override;

        /**
         * @brief Render the menu scene (draw UI elements).
         */
        void render() override;

        /**
         * @brief Handle input and events in the menu.
         */
        void handleEvents() override;

        /**
         * @brief Called when the menu scene is closed (cleanup).
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the game instance.
};

} // namespace scene

#endif //MENUSCENE_HPP
