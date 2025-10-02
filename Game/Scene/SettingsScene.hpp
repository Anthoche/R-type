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
};
}

#endif // SETTINGSSCENE_HPP
