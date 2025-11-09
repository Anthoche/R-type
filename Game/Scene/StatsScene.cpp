/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** StatsScene
*/

#include "Include/StatsScene.hpp"
#include "UITheme.hpp"
#include "RenderUtils.hpp"
#include <algorithm>
#include <format>
#include <mutex>

namespace scene {

    StatsScene::StatsScene(Game &game)
        : AScene(960, 540, "R-Type - Stats"), _game(game) {}

    void StatsScene::init() {
        _registry.clear();
        _isOpen = true;
        _raylib.enableCursor();
        _raylib.setTargetFPS(60);
        _font = _raylib.loadFont(ASSETS_PATH "/fonts/Steelar-j9Vnj.otf");
        _fontLoaded = true;
    }

    void StatsScene::render() {
        Labels labels = buildLabels();
        int32_t globalScore = 0;
        std::vector<ScoreRow> rows = collectScoreRows(globalScore);

        std::string outcomeText;
        switch (_game.getLastMatchOutcome()) {
            case Game::MatchOutcome::Victory:
                outcomeText = labels.victory;
                break;
            case Game::MatchOutcome::Defeat:
                outcomeText = labels.defeat;
                break;
            default:
                outcomeText.clear();
                break;
        }

        _raylib.beginDrawing();
        drawSceneBackground(_raylib, ui::theme::BackgroundTop, ui::theme::BackgroundBottom);
        drawHeader(labels, outcomeText, globalScore);
        drawScoreTable(labels, rows, 210.f);
        drawCenteredText(labels.hint, _height - 60.f, 20.f, Fade(RAYWHITE, 0.7f));
        _raylib.endDrawing();
    }

    void StatsScene::handleEvents() {
        if (_raylib.isKeyPressed(KEY_ENTER) ||
            _raylib.isKeyPressed(KEY_SPACE) ||
            _raylib.isKeyPressed(KEY_ESCAPE)) {
            _game.setLastMatchOutcome(Game::MatchOutcome::None);
            _game.setGameStatus(GameStatus::WAITING_PLAYERS);
            _game.getSceneHandler().open("menu");
        }
    }

    void StatsScene::onClose() {
        if (_fontLoaded) {
            _raylib.unloadFont(_font);
            _fontLoaded = false;
        }
    }

    StatsScene::Labels StatsScene::buildLabels() const {
        using Lang = Game::Language;
        Labels labels;

        switch (_game.getLanguage()) {
            case Lang::FRENCH:
                labels.title = "Statistiques de partie";
                labels.victory = "Victoire !";
                labels.defeat = "Défaite...";
                labels.globalScore = "Score global";
                labels.playerColumn = "Joueur";
                labels.scoreColumn = "Score";
                labels.hint = "Appuyez sur ENTREE pour revenir au menu";
                labels.noData = "Aucune donnée disponible";
                labels.youLabel = "Vous";
                break;
            case Lang::ITALIAN:
                labels.title = "Statistiche partita";
                labels.victory = "Vittoria!";
                labels.defeat = "Sconfitta...";
                labels.globalScore = "Punteggio globale";
                labels.playerColumn = "Giocatore";
                labels.scoreColumn = "Punteggio";
                labels.hint = "Premi INVIO per tornare al menu";
                labels.noData = "Nessun punteggio disponibile";
                labels.youLabel = "Tu";
                break;
            default:
                labels.title = "Match Stats";
                labels.victory = "Victory!";
                labels.defeat = "Defeat...";
                labels.globalScore = "Team score";
                labels.playerColumn = "Player";
                labels.scoreColumn = "Score";
                labels.hint = "Press ENTER to return to the menu";
                labels.noData = "No score data available";
                labels.youLabel = "You";
                break;
        }

        return labels;
    }

    std::vector<StatsScene::ScoreRow> StatsScene::collectScoreRows(int32_t &globalScore) {
        std::vector<ScoreRow> rows;
        uint32_t myId = 0;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            globalScore = _game.getGameClient().globalScore;
            myId = _game.getGameClient().clientId;
            for (const auto &kv : _game.getGameClient().playerIndividualScores) {
                rows.push_back(ScoreRow{kv.first, kv.second, kv.first == myId});
            }
        }

        std::sort(rows.begin(), rows.end(), [](const ScoreRow &lhs, const ScoreRow &rhs) {
            if (lhs.score == rhs.score)
                return lhs.playerId < rhs.playerId;
            return lhs.score > rhs.score;
        });

        return rows;
    }

    void StatsScene::drawHeader(const Labels &labels, const std::string &outcomeText, int32_t globalScore) {
        drawCenteredText(labels.title, 60.f, 48.f, RAYWHITE);

        if (!outcomeText.empty()) {
            Color color = (_game.getLastMatchOutcome() == Game::MatchOutcome::Victory) ? GREEN : RED;
            drawCenteredText(outcomeText, 118.f, 32.f, color);
        }

        std::string scoreText = std::format("{}: {}", labels.globalScore, globalScore);
        drawCenteredText(scoreText, 165.f, 26.f, RAYWHITE);
    }

    void StatsScene::drawScoreTable(const Labels &labels, const std::vector<ScoreRow> &rows, float startY) {
        float tableLeft = 140.f;
        float tableRight = static_cast<float>(_width) - 140.f;
        float rankColumn = tableLeft - 60.f;
        float rowHeight = 44.f;

        _raylib.drawTextEx(_font, labels.playerColumn, {tableLeft, startY}, 24.f, -1.f, RAYWHITE);
        _raylib.drawTextEx(_font, labels.scoreColumn,
                           {tableRight - _raylib.measureTextEx(_font, labels.scoreColumn.c_str(), 24.f, -1.f).x, startY},
                           24.f, -1.f, RAYWHITE);
        _raylib.drawTextEx(_font, "#", {rankColumn, startY}, 24.f, -1.f, RAYWHITE);

        if (rows.empty()) {
            drawCenteredText(labels.noData, startY + rowHeight * 2.f, 24.f, Fade(RAYWHITE, 0.8f));
            return;
        }

        std::size_t maxRows = std::min<std::size_t>(rows.size(), 8);
        for (std::size_t i = 0; i < maxRows; ++i) {
            const ScoreRow &row = rows[i];
            float rowY = startY + rowHeight * (i + 1);

            Rectangle background{tableLeft - 20.f, rowY - 6.f, tableRight - tableLeft + 40.f, rowHeight - 4.f};
            if (row.isLocal) {
                _raylib.drawRectangleRounded(background, 0.25f, 8, Color{20, 170, 255, 120});
            } else {
                _raylib.drawRectangleRoundedLines(background, 0.25f, 8, 1, Color{255, 255, 255, 40});
            }

            std::string rankText = std::format("{}.", i + 1);
            _raylib.drawTextEx(_font, rankText, {rankColumn, rowY}, 20.f, -1.f, row.isLocal ? YELLOW : RAYWHITE);

            std::string playerText = formatPlayerLabel(labels, row);
            _raylib.drawTextEx(_font, playerText, {tableLeft, rowY}, 22.f, -1.f, RAYWHITE);

            std::string scoreText = std::format("{}", row.score);
            float scoreWidth = _raylib.measureTextEx(_font, scoreText.c_str(), 22.f, -1.f).x;
            _raylib.drawTextEx(_font, scoreText, {tableRight - scoreWidth, rowY}, 22.f, -1.f, RAYWHITE);
        }
    }

    std::string StatsScene::formatPlayerLabel(const Labels &labels, const ScoreRow &row) const {
        if (row.isLocal)
            return labels.youLabel;

        return std::format("{} {}", labels.playerColumn, row.playerId);
    }

    void StatsScene::drawCenteredText(const std::string &text, float y, float fontSize, Color color) {
        Vector2 size = _raylib.measureTextEx(_font, text.c_str(), fontSize, -1.f);
        Vector2 pos{(_width - size.x) / 2.f, y};
        _raylib.drawTextEx(_font, text.c_str(), pos, fontSize, -1.f, color);
    }

} // namespace scene
