/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomCreationScene
*/

#pragma once

#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace scene {

enum class GameModeOption {
	LEVELS,
	ENDLESS,
	CUSTOM
};

/**
 * @class RoomCreationScene
 * @brief UI scene that lets the player define custom room limits.
 *
 * Provides a minimal form with controls to adjust the minimum and maximum
 * number of players before sending a creation request to the server.
 */
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
        Color const _accentColor{10, 34, 64, 255};
        Vector2 _margin{25.f, 25.f};
        float const _labelColumnX = 80.f;

        int _minPlayers = 2;
        int _maxPlayers = 4;
        int const _minLimit = 1;
        int const _maxLimit = 8;

        std::string _roomNameButtonId{"value_room_name"};
        std::string _minValueButtonId{"value_min_players"};
        std::string _maxValueButtonId{"value_max_players"};
        std::string _modeValueButtonId{"value_game_mode"};
        float _controlsCenterX = 0.f;
        std::string _roomName;
        bool _nameInputActive = false;
        static constexpr std::size_t _roomNameMaxLength = 8;
        struct ModeOptionData {
            std::string labelEn;
            std::string labelFr;
            std::string labelIt;
            GameModeOption value;
            bool enabled;
        };

        struct LocaleText {
            std::string title;
            std::string helper;
            std::string roomLabel;
            std::string modeLabel;
            std::string minLabel;
            std::string maxLabel;
            std::string confirmLabel;
            std::string backLabel;
        };

        std::vector<ModeOptionData> _modeOptions;
        std::size_t _modeIndex = 0;
        GameModeOption _selectedMode = GameModeOption::LEVELS;

        void registerComponents();
        void createLayout();
        LocaleText buildLocaleText() const;
        float placeHeader(const LocaleText &text);
        void buildSelectors(const LocaleText &text, float baseY);
        void buildActionButtons(const LocaleText &text, float baseY);
        void addLabel(const std::string &label, float y, int fontSize = 20);
        ecs::entity_t makeButton(const std::string &id, const std::string &label, float x, float y, float width, float height, Color color, Color textColor, int fontSize);
        void disableButton(ecs::entity_t entity);
        void createValueRow(const std::string &label, float y, const std::string &minusId,
                            const std::string &plusId, const std::string &valueId, int value);
        void createNameRow(const std::string &label, float y);
        void createModeSelector(const std::string &label, float y);
        void createModeRow(const std::string &label, float y);
        std::string getLocalizedModeLabel(const ModeOptionData &option) const;
        std::string getRoomNamePlaceholder() const;
        void updateRoomNameDisplay();
        void handleNameInput();
        void drawButtons();
        void drawTexts();
        void processMouseInteractions();
        void resetButtonStates();
        void handleButtonClick(const std::string &id);
        bool handlePlayerCountButton(const std::string &id);
        bool handleModeButton(const std::string &id);
        void changeMin(int delta);
        void changeMax(int delta);
        void changeMode(int delta);
        std::size_t nextEnabledModeIndex(int delta) const;
        void updateValueDisplay(const std::string &buttonId, int value);
        std::string sanitizeRoomName() const;
        void submitRoom();
};

} // namespace scene
