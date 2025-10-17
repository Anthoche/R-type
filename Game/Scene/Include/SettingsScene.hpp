/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/


#pragma once

#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"

namespace scene {

/**
 * @class SettingsScene
 * @brief Represents the settings menu of the game.
 *
 * The settings scene allows the player to:
 * - Change game difficulty (Easy, Medium, Hard).
 * - Choose number of lives (3, 5, 7).
 * - Toggle sound (On/Off).
 * - Change game language (English/French).
 * - Navigate back to the main menu.
 *
 * It handles initialization, rendering of UI, user input,
 * language translation, and proper cleanup of loaded resources.
 */
class SettingsScene : public AScene {
    public:
        /**
        * @brief Construct a SettingsScene with a reference to the Game instance.
        * @param game The game instance.
        */
        SettingsScene(Game &game);

        /**
        * @brief Default destructor.
        */
        ~SettingsScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
        * @brief Initialize the settings scene (load fonts, create buttons and texts).
        */
        void init() override;

        /**
        * @brief Render the settings scene (draw buttons, texts, title, etc.).
        */
        void render() override;

        /**
        * @brief Handle user input and interactions (clicks, hovers, etc.).
        */
        void handleEvents() override;

        /**
        * @brief Called when the scene is closed (cleanup and unload resources).
        */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the main game instance.

        Font _font{};
        std::string _sceneTitle = "R-Type";

        // --- UI configuration ---
        int const _titleSize = 90; 
        Color const _accentColor{26, 170, 177, 255};
        Vector2 const _buttonSize{300.f, 50.f};
        int const _buttonTextSize = 23; 
        int const _buttonSpacing = 20;
        Vector2 _buttonPosition{230.f, 150.f};

        // --- Language management ---
        Game::Language _currentLanguage = Game::Language::ENGLISH;

        // --- Buttons and values ---
        std::vector<std::string> _buttons{
            "1. Difficulty", "2. Lives", "3. Sound", "4. Language"
        };
        std::vector<std::string> _values{
            "Medium", "3", "On", "English"
        };

        // --- Option data ---
        std::vector<std::string> _levels{"Easy", "Medium", "Hard"};
        std::size_t _currentLevelIndex{1};

        std::vector<std::string> _lives{"3", "5", "7"};
        std::size_t _currentLivesIndex{0};

        bool _soundOn = true;

        // --- Setup functions ---
        /**
        * @brief Prepare all scene elements (title, buttons, etc.).
        */
        void setupScene();

        /**
        * @brief Register ECS components used in this scene.
        */
        void registerComponents();

        /**
        * @brief Create the title entity.
        */
        void createTitle();

        /**
        * @brief Create the main option buttons.
        */
        void createButtons();

        /**
        * @brief Create the "< Back" button.
        */
        void createBackButton();

        // --- Button creation handlers ---
        using ButtonCreator = void (SettingsScene::*)(Vector2 pos, std::size_t i);

        /**
        * @brief Create the difficulty button.
        */
        void createDifficultyButton(Vector2 pos, std::size_t i);

        /**
        * @brief Create the lives button.
        */
        void createLivesButton(Vector2 pos, std::size_t i);

        /**
        * @brief Create the sound toggle button.
        */
        void createSoundButton(Vector2 pos, std::size_t i);

        /**
        * @brief Create the language toggle button.
        */
        void createLanguageButton(Vector2 pos, std::size_t i);

        /**
        * @brief Create a default text entity (used for button labels).
        */
        void createDefaultText(Vector2 pos, std::size_t i);

        // --- Logic and event handling ---
        /**
        * @brief Reset button hover and click states.
        */
        void resetButtonStates();

        /**
        * @brief Handle a click event for a button by its ID.
        * @param id The ID of the clicked button.
        */
        void handleButtonClick(std::string const &id);

        /**
        * @brief Toggle sound on/off.
        */
        void toggleSound();

        /**
        * @brief Cycle through available lives options.
        */
        void cycleLives();

        /**
        * @brief Cycle through available difficulty levels.
        */
        void cycleDifficulty();

        /**
        * @brief Toggle between English and French languages.
        */
        void toggleLanguage();

        // --- Localization ---
        /**
        * @brief Apply French translations to all texts.
        */
        void translateToFrench();

        /**
        * @brief Apply English translations to all texts.
        */
        void translateToEnglish();

        /**
        * @brief Update all displayed texts (button labels, etc.).
        */
        void updateAllTexts();

        /**
        * @brief Update displayed values on all buttons.
        */
        void updateAllButtonValues();

        /**
        * @brief Update the text of a specific button.
        * @param buttonId The button entity ID.
        * @param newText The new text to display.
        */
        void updateButtonText(const std::string &buttonId, const std::string &newText);

        // --- Drawing helpers ---
        /**
        * @brief Draw a button with specified properties.
        */
        void drawButton(Vector2 position, Vector2 size, std::string const &content,
                        int fontSize, float spacing, Color color, Color textColor,
                        bool isHovered, bool isClicked);

        // --- UI positioning helpers ---
        /**
        * @brief Get vertical center position for an element.
        */
        int getCenterY(int screenHeight, int elementHeight) const;

        /**
        * @brief Get vertical center position for a group of buttons.
        */
        int getButtonsCenterY(int screenHeight, int numberOfButtons,
                            int buttonHeight, int buttonSpacing) const;
    };

} // namespace scene
