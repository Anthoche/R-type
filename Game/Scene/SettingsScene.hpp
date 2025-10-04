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

class SettingsScene : public AScene {
public:
    SettingsScene(Game &game);
    ~SettingsScene() override = default;

    void init() override;
    void render() override;
    void handleEvents() override;
    void onClose();

private:
    Game &_game;

    Font _font{};
    std::string _sceneTitle;

    int const _titleSize = 90;
    Color const _accentColor{26, 170, 177, 255};
    Vector2 const _buttonSize{300.f, 50.f};
    int const _buttonTextSize = 23;
    int const _buttonSpacing = 20;

    std::vector<std::string> _buttons{"1. Difficulty", "2. Lives", "3. Sound"};
    std::vector<std::string> _values{"Medium", "3", "On"};

    std::vector<std::string> _levels{"Easy", "Medium", "Hard"};
    std::size_t _currentLevelIndex{0};

    std::vector<std::string> _lives{"3", "5", "7"};
    std::size_t _currentLivesIndex{0};

    bool _soundOn = true;

    Vector2 _buttonPosition{230.f, 200.f};

    void resetButtonStates();
    void handleButtonClick(std::string const &id);
    void drawButton(Vector2 position, Vector2 size, std::string const &content,
                    int fontSize, float spacing, Color color, Color textColor,
                    bool isHovered, bool isClicked);

    int getCenterY(int screenHeight, int elementHeight) const;
    int getButtonsCenterY(int screenHeight, int numberOfButtons,
                          int buttonHeight, int buttonSpacing) const;
};

} // namespace scene

#endif // SETTINGSSCENE_HPP
