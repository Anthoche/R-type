/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#ifndef MENUSCENE_HPP
    #define MENUSCENE_HPP

    #include "../../../Engine/Rendering/scene/Include/AScene.hpp"
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

        Font _font{}; ///< Font used for rendering text.

        // Title
        int const _titleSize = 90; ///< Font size for the title.
        float _titleCenterY; ///< Y position to center the title vertically.

        // Buttons
        Color const _accentColor{10, 34, 64, 255}; ///< Accent color for buttons.
        Vector2 const _buttonSize{280.f, 70.f}; ///< Size of buttons.
        int const _buttonTextSize = 35; ///< Font size for button text.
        int const _buttonSpacing = 35; ///< Vertical spacing between buttons.
        float _buttonCenterY{}; ///< Y position to center buttons vertically.
        Vector2 _buttonPosition{}; ///< Position of the current button.
        int _selectedButtonIndex = -1; ///< Index of the currently selected button.

        /**
         * @brief Reset the hover and click states of all menu buttons.
         */
        void resetButtonStates();

        /**
         * @brief Handle the click event for a specific button.
         * @param id The ID of the clicked button as a string.
         */
        void handleButtonClick(std::string const &id);

        /**
         * @brief Draw a button on the screen.
         * @param position Position of the button.
         * @param size Size of the button.
         * @param content Text displayed on the button.
         * @param fontSize Size of the text font.
         * @param spacing Spacing for button elements.
         * @param color Background color of the button.
         * @param textColor Color of the text.
         * @param isHovered True if the mouse is hovering over the button.
         * @param isClicked True if the button is being clicked.
         */
        void drawButton(Vector2 position, Vector2 size, std::string const &content, int fontSize, float spacing, Color color, Color textColor, bool isHovered, bool isClicked);
};

} // namespace scene

#endif //MENUSCENE_HPP
