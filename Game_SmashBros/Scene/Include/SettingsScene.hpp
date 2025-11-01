/**
 * @file SettingsScene.hpp
 * @brief Game settings and configuration scene
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#pragma once

#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"

namespace scene {

/**
 * @class SettingsScene
 * @brief Scene for managing game settings and preferences
 *
 * Provides a user interface for configuring:
 * - Game difficulty (Easy, Medium, Hard)
 * - Number of lives (1-7)
 * - Endless mode toggle
 * - Sound on/off
 * - Language selection (English/French/Italian)
 * 
 * Handles initialization, rendering of UI elements, user input processing,
 * real-time language translation, and proper resource cleanup.
 */
class SettingsScene : public AScene {
    public:
        /**
         * @brief Constructs the settings scene
         * @param game Reference to the main game instance
         */
        SettingsScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~SettingsScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initializes the settings scene
         * 
         * Loads fonts, creates UI buttons and text elements,
         * and sets up initial configuration values.
         */
        void init() override;

        /**
         * @brief Renders all settings UI elements
         * 
         * Draws the title, option buttons, current values,
         * and navigation elements to the screen.
         */
        void render() override;

        /**
         * @brief Processes user input and interactions
         * 
         * Handles mouse clicks, hover states, and keyboard input
         * for navigating and modifying settings.
         */
        void handleEvents() override;

        /**
         * @brief Cleans up resources when the scene closes
         * 
         * Unloads fonts and releases allocated resources.
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the main game instance

        Font _font{};                        ///< Font used for all text rendering
        std::string _sceneTitle = "R-Type";  ///< Title displayed at the top of the scene

        // --- UI configuration ---
        int const _titleSize = 90;                        ///< Font size for the scene title
        Color const _accentColor{46, 204, 113, 255};     ///< Accent color for UI elements (green)
        Vector2 const _buttonSize{300.f, 50.f};          ///< Standard dimensions for buttons
        int const _buttonTextSize = 23;                   ///< Font size for button text
        int const _buttonSpacing = 20;                    ///< Vertical spacing between buttons
        Vector2 _buttonPosition{230.f, 150.f};           ///< Starting position for button layout

        // --- Language management ---
        Game::Language _currentLanguage = Game::Language::ENGLISH; ///< Currently selected language

        // --- Button labels and values ---
        std::vector<std::string> _buttons{
            "1. Endless", "2. Lives", "3. Sound", "4. Language"
        }; ///< Text labels for option buttons
        
        std::vector<std::string> _values{
            "Off", "3", "On", "English"
        }; ///< Current values displayed for each option

        // --- Option data ---
        bool _endlessMode = false; ///< Whether endless mode is enabled

        std::vector<std::string> _lives{"1", "2", "3", "4", "5", "6", "7"}; ///< Available life count options
        std::size_t _currentLivesIndex{2}; ///< Index of currently selected life count (default: 3)

        bool _soundOn = true; ///< Whether sound is enabled

        // --- Setup functions ---
        /**
         * @brief Prepares all scene elements for rendering
         * 
         * Orchestrates the creation of title, buttons, and other UI components.
         */
        void setupScene();

        /**
         * @brief Registers ECS components required by this scene
         * 
         * Ensures all necessary component types are available in the registry.
         */
        void registerComponents();

        /**
         * @brief Creates the scene title entity
         * 
         * Instantiates and positions the main title text at the top of the screen.
         */
        void createTitle();

        /**
         * @brief Creates all main option buttons
         * 
         * Generates buttons for endless mode, lives, sound, and language settings.
         */
        void createButtons();

        /**
         * @brief Creates the back navigation button
         * 
         * Adds a "< Back" button to return to the previous scene.
         */
        void createBackButton();

        // --- Button creation handlers ---
        using ButtonCreator = void (SettingsScene::*)(Vector2 pos, std::size_t i); ///< Function pointer type for button creation methods

        /**
         * @brief Creates the lives selection button
         * @param pos Position where the button should be placed
         * @param i Button index in the layout
         */
        void createLivesButton(Vector2 pos, std::size_t i);

        /**
         * @brief Creates the endless mode toggle button
         * @param pos Position where the button should be placed
         * @param i Button index in the layout
         */
        void createEndlessButton(Vector2 pos, std::size_t i);

        /**
         * @brief Creates the sound toggle button
         * @param pos Position where the button should be placed
         * @param i Button index in the layout
         */
        void createSoundButton(Vector2 pos, std::size_t i);

        /**
         * @brief Creates the language selection button
         * @param pos Position where the button should be placed
         * @param i Button index in the layout
         */
        void createLanguageButton(Vector2 pos, std::size_t i);

        /**
         * @brief Creates a default text entity for button labels
         * @param pos Position where the text should be placed
         * @param i Text index in the layout
         */
        void createDefaultText(Vector2 pos, std::size_t i);

        // --- Logic and event handling ---
        /**
         * @brief Resets all button interaction states to default
         * 
         * Clears hover and click states, typically called at the
         * start of event processing each frame.
         */
        void resetButtonStates();

        /**
         * @brief Handles click events for buttons
         * @param id Unique identifier of the clicked button
         * 
         * Executes the appropriate action based on which button was clicked.
         */
        void handleButtonClick(std::string const &id);

        /**
         * @brief Toggles sound on or off
         * 
         * Updates the sound setting and refreshes the displayed value.
         */
        void toggleSound();

        /**
         * @brief Cycles through available life count options
         * 
         * Moves to the next value in the lives list (wraps around at end).
         */
        void cycleLives();

        /**
         * @brief Toggles endless mode on or off
         * 
         * Updates the endless mode setting and refreshes the displayed value.
         */
        void toggleEndless();

        /**
         * @brief Cycles through available language options
         * 
         * Switches between English, French, and Italian, updating all UI text.
         */
        void toggleLanguage();

        // --- Localization ---
        /**
         * @brief Applies French translations to all text elements
         * 
         * Updates button labels and values to their French equivalents.
         */
        void translateToFrench();

        /**
         * @brief Applies Italian translations to all text elements
         * 
         * Updates button labels and values to their Italian equivalents.
         */
        void translateToItalian();
        
        /**
         * @brief Applies English translations to all text elements
         * 
         * Updates button labels and values to their English equivalents.
         */
        void translateToEnglish();

        /**
         * @brief Updates all displayed text based on current language
         * 
         * Refreshes button labels and UI text after language change.
         */
        void updateAllTexts();

        /**
         * @brief Updates all button value displays
         * 
         * Refreshes the current value shown for each setting option.
         */
        void updateAllButtonValues();

        /**
         * @brief Updates the text content of a specific button
         * @param buttonId Unique identifier of the button entity
         * @param newText New text content to display
         */
        void updateButtonText(const std::string &buttonId, const std::string &newText);

        // --- Drawing helpers ---
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
         */
        void drawButton(Vector2 position, Vector2 size, std::string const &content,
                        int fontSize, float spacing, Color color, Color textColor,
                        bool isHovered, bool isClicked);

        // --- UI positioning helpers ---
        /**
         * @brief Calculates vertical center position for an element
         * @param screenHeight Total screen height
         * @param elementHeight Height of the element to center
         * @return Y coordinate for centered positioning
         */
        int getCenterY(int screenHeight, int elementHeight) const;

        /**
         * @brief Calculates vertical center position for a button group
         * @param screenHeight Total screen height
         * @param numberOfButtons Number of buttons in the group
         * @param buttonHeight Height of each button
         * @param buttonSpacing Vertical spacing between buttons
         * @return Y coordinate for the first button to center the group
         */
        int getButtonsCenterY(int screenHeight, int numberOfButtons,
                            int buttonHeight, int buttonSpacing) const;
    };

} // namespace scene