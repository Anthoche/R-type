/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#ifndef MENUSCENE_HPP
    #define MENUSCENE_HPP

    #include "../../Rendering/scene/AScene.hpp"
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
         * @brief Construct a MenuScene with a reference to the game.
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

        /**
        * @brief Update the menu buttons' text based on the selected language.
        * @param isFrench True if French, false if English.
        */
        void updateLanguage(bool isFrench);

    private:
        Game &_game; ///< Reference to the game instance.

        Font _font{};

        //Title
        int const _titleSize = 90;
        float _titleCenterY;

        //Buttons
        Color const _accentColor{26, 170, 177, 255};
        Vector2 const _buttonSize{280.f, 70.f};
        int const _buttonTextSize = 35;
        int const _buttonSpacing = 35;
        float _buttonCenterY{};
        Vector2 _buttonPosition{};

        void resetButtonStates();
        void handleButtonClick(std::string const &id);
        void drawButton(Vector2 position, Vector2 size, std::string const &content, int fontSize, float spacing, Color color, Color textColor, bool isHovered, bool isClicked);
};

} // namespace scene

#endif //MENUSCENE_HPP
