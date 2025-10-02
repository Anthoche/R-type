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

namespace scene {

class SettingsScene: public AScene {
    
public:
    SettingsScene(Game &game);
    ~SettingsScene() override = default;

    void init() override;
    void render() override;
    void handleEvents() override;
    void onClose() override;

private:
    Game &_game;

    Font _font{};
    std::string _sceneTitle;
    int const _titleSize = 80;

    std::vector<std::string> _buttons;
    std::vector<std::string> _values;
    Color const _accentColor{26, 170, 177, 255};
    Vector2 const _buttonSize{300.f, 50.f};
    int const _buttonTextSize = 23;
    int const _buttonSpacing = 20;

    Vector2 _buttonPosition{};
    int getCenterY(int screenHeight, int elementHeight) const;
    int getButtonsCenterY(int screenHeight, int numberOfButtons, 
                        int buttonHeight, int buttonSpacing) const;


};
}

#endif // SETTINGSSCENE_HPP
