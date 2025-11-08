/**
 * @file MenuScene.hpp
 * @brief Main menu scene implementation for the game
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#ifndef MENUSCENE_HPP
    #define MENUSCENE_HPP

    #include "../../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

/**
 * @class MenuScene
 * @brief Main menu scene handling navigation and game entry
 *
 * Manages the main menu interface including:
 * - UI initialization and rendering
 * - Button interactions and navigation
 * - Language selection
 * - User input processing
 * - Resource management and cleanup
 */
class MenuScene: public AScene {
    public:
        /**
         * @brief Constructs the menu scene
         * @param game Reference to the main game instance
         */
        MenuScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~MenuScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initializes menu resources, UI elements, and buttons
         * 
         * Sets up fonts, calculates button positions, and prepares
         * all visual elements for rendering.
         */
        void init() override;

        /**
         * @brief Renders all menu UI elements to the screen
         * 
         * Draws the title, buttons, and any decorative elements.
         */
        void render() override;

        /**
         * @brief Processes user input and menu interactions
         * 
         * Handles mouse clicks, hover states, and keyboard navigation.
         */
        void handleEvents() override;

        /**
         * @brief Cleans up menu resources when the scene closes
         * 
         * Releases fonts, textures, and other allocated resources.
         */
        void onClose() override;

        /**
         * @brief Updates menu button text based on selected language
         * @param isFrench True for French language, false for English
         * 
         * Changes all UI text to match the selected language preference.
         */
        void updateLanguage(bool isFrench);

    private:
        Game &_game; ///< Reference to the main game instance

        Font _font{}; ///< Font used for menu text rendering

        // Title configuration
        int const _titleSize = 90;        ///< Font size for the menu title
        float _titleCenterY;              ///< Vertical center position for title

        // Button styling and layout
        Color const _accentColor{10, 34, 64, 255}; ///< Accent color for UI elements (green)
        Vector2 const _buttonSize{280.f, 70.f};      ///< Standard button dimensions
        int const _buttonTextSize = 35;              ///< Font size for button text
        int const _buttonSpacing = 35;               ///< Vertical spacing between buttons
        float _buttonCenterY{};                      ///< Vertical center position for buttons
        Vector2 _buttonPosition{};                   ///< Current button rendering position
        int _selectedButtonIndex = -1;               ///< Index of currently selected button (-1 if none)

        /**
         * @brief Resets all button states to default
         * 
         * Clears hover and click states for all buttons, typically
         * called at the start of event processing.
         */
        void resetButtonStates();
        
        /**
         * @brief Handles button click actions
         * @param id Unique identifier of the clicked button
         * 
         * Executes the appropriate action based on which button was clicked
         * (e.g., start game, open settings, quit).
         */
        void handleButtonClick(std::string const &id);
        
        /**
         * @brief Renders a button with visual feedback
         * @param position Top-left position of the button
         * @param size Width and height of the button
         * @param content Text to display on the button
         * @param fontSize Size of the button text
         * @param spacing Additional spacing around text
         * @param color Background color of the button
         * @param textColor Color of the button text
         * @param isHovered Whether the mouse is hovering over the button
         * @param isClicked Whether the button is currently pressed
         * 
         * Draws a button with appropriate styling based on its interaction state.
         */
        void drawButton(Vector2 position, Vector2 size, std::string const &content, 
                       int fontSize, float spacing, Color color, Color textColor, 
                       bool isHovered, bool isClicked);
};

} // namespace scene

#endif //MENUSCENE_HPP