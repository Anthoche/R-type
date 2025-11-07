/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomCreationScene (Smash variant)
*/

#pragma once

#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"

namespace scene {

class RoomCreationScene : public AScene {
    public:
        RoomCreationScene(Game &game);
        ~RoomCreationScene() override = default;

        void init() override;
        void render() override;
        void handleEvents() override;
        void onClose() override;

    private:
        Game &_game;

        Font _font{};
        Color const _accentColor{46, 204, 113, 255};
        Vector2 _margin{25.f, 25.f};

        int _minPlayers = 2;
        int _maxPlayers = 4;
        int const _minLimit = 1;
        int const _maxLimit = 8;

        std::string _minValueButtonId{"value_min_players"};
        std::string _maxValueButtonId{"value_max_players"};

        void registerComponents();
        void createLayout();
        void createValueRow(const std::string &label, float y, const std::string &minusId,
                            const std::string &plusId, const std::string &valueId, int value);
        void resetButtonStates();
        void handleButtonClick(const std::string &id);
        void changeMin(int delta);
        void changeMax(int delta);
        void updateValueDisplay(const std::string &buttonId, int value);
        void submitRoom();
};

} // namespace scene
