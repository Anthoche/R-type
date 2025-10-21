/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomSelectScene
*/

#ifndef ROOMSELECTSCENE_HPP
    #define ROOMSELECTSCENE_HPP

    #include "../../Rendering/scene/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

/**
 * @class RoomSelectScene
 * @brief Represents the room select scene of the game.
 *
 * The menu scene handles:
 * - Initialization of menu UI and resources.
 * - Rendering menu elements.
 * - Handling user input/events (e.g., navigating the menu).
 * - Cleaning up when the scene is closed.
 */
class RoomSelectScene: public AScene {
    public:
        /**
         * @brief Construct a RoomSelectScene with a reference to the game.
         * @param game The game instance.
         */
        RoomSelectScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~RoomSelectScene() override = default;

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

        Font _font{};

        //Title

        //Buttons
        int _selectedButtonIndex = -1;

        void resetButtonStates();
        void handleButtonClick(std::string const &id);
        void drawButton(Vector2 position, Vector2 size, std::string const &content, int fontSize, float spacing, Color color, Color textColor, bool isHovered, bool isClicked);
};

} // namespace scene

#endif //ROOMSELECTSCENE_HPP
