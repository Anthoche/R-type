/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/

#ifndef SETTINGSSCENE_HPP
#define SETTINGSSCENE_HPP

#include "../../Engine/Rendering/scene/AScene.hpp"
#include "../Game.hpp"
#include "components.hpp"

namespace scene {

/**
 * @class SettingsScene
 * @brief Represents the settings menu of the game.
 *
 * The settings scene allows the player to:
 * - Change game difficulty (Easy, Medium, Hard).
 * - Choose number of lives (3, 5, 7).
 * - Toggle sound (On/Off).
 * - Navigate back to the main menu.
 *
 * It handles initialization, rendering of UI, input events,
 * and proper cleanup of allocated resources (fonts, entities).
 */
class SettingsScene : public AScene {
public:
    /**
     * @brief Construct a SettingsScene with reference to the Game instance.
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
     * @brief Render the settings scene (draw buttons, text, etc.).
     */
    void render() override;

    /**
     * @brief Handle input and user interactions (clicks, hovers).
     */
    void handleEvents() override;

    /**
     * @brief Called when the scene is closed (cleanup).
     */
    void onClose() override;

private:
    Game &_game;

    Font _font{};
    std::string _sceneTitle;

    // --- UI config ---
    int const _titleSize = 90;
    Color const _accentColor{26, 170, 177, 255};
    Vector2 const _buttonSize{300.f, 50.f};
    int const _buttonTextSize = 23;
    int const _buttonSpacing = 20;

    // --- Buttons and options ---
    std::vector<std::string> _buttons{"1. Difficulty", "2. Lives", "3. Sound"};
    std::vector<std::string> _values{"Medium", "3", "On"};

    std::vector<std::string> _levels{"Easy", "Medium", "Hard"};
    std::size_t _currentLevelIndex{0};

    std::vector<std::string> _lives{"3", "5", "7"};
    std::size_t _currentLivesIndex{0};

    bool _soundOn = true;

    // --- Layout ---
    Vector2 _buttonPosition{230.f, 200.f};

    // --- Helpers ---
    /**
     * @brief Reset all button states (hovered, clicked).
     */
    void resetButtonStates();

    /**
     * @brief Handle button clicks by id (difficulty, lives, sound, back).
     * @param id Identifier of the clicked button.
     */
    void handleButtonClick(std::string const &id);

    /**
     * @brief Draw a button with text.
     */
    void drawButton(Vector2 position, Vector2 size, std::string const &content,
                    int fontSize, float spacing, Color color, Color textColor,
                    bool isHovered, bool isClicked);

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

#endif // SETTINGSSCENE_HPP
