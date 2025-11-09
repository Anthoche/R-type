/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** StatsScene
*/

#pragma once

#include <string>
#include <vector>
#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"

namespace scene {

class StatsScene : public AScene {
    public:
        explicit StatsScene(Game &game);
        ~StatsScene() override = default;

        void init() override;
        void render() override;
        void handleEvents() override;
        void onClose() override;

    private:
        struct ScoreRow {
            uint32_t playerId;
            uint32_t score;
            bool isLocal;
        };

        struct Labels {
            std::string title;
            std::string victory;
            std::string defeat;
            std::string globalScore;
            std::string playerColumn;
            std::string scoreColumn;
            std::string hint;
            std::string noData;
            std::string youLabel;
        };

        Labels buildLabels() const;
        std::vector<ScoreRow> collectScoreRows(int32_t &globalScore);
        void drawHeader(const Labels &labels, const std::string &outcomeText, int32_t globalScore);
        void drawScoreTable(const Labels &labels, const std::vector<ScoreRow> &rows, float startY);
        std::string formatPlayerLabel(const Labels &labels, const ScoreRow &row) const;

        void drawCenteredText(const std::string &text, float y, float fontSize, Color color);

        Game &_game;
        Font _font{};
        bool _fontLoaded{false};
};

} // namespace scene
