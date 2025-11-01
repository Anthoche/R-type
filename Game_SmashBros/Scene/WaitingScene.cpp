/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** WaitingScene
*/

#include "Include/WaitingScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"
#include <raylib.h>

namespace scene {
	WaitingScene::WaitingScene(Game &game)
		: AScene(960, 540, "R-Type - Waiting..."), _game(game) {}

	void WaitingScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(Color{6, 16, 28, 255});

		auto &drawables = _registry.get_components<component::drawable>();
		auto &positions = _registry.get_components<component::position>();
		auto &types = _registry.get_components<component::type>();
		auto &text = _registry.get_components<component::text>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
			if (!positions[i] || !drawables[i] || !types[i]) continue;

			Vector2 pos = {positions[i]->x, positions[i]->y};
			Vector2 size = {drawables[i]->width, drawables[i]->height};

			if (types[i]->value == component::entity_type::BUTTON) {
				std::string content = text[i]->content;
				int fontSize = text[i]->font_size;
				float spacing = text[i]->spacing;
				Color color = drawables[i]->color;
				Color textColor = text[i]->color;
				drawButton(_raylib, pos, size, content, _font, fontSize, spacing,
						color, textColor, hoverable[i]->isHovered,
						clickable[i]->isClicked, clickable[i]->enabled);
			}

			if (types[i]->value == component::entity_type::TEXT) {
				_raylib.drawTextEx(text[i]->font, text[i]->content, pos,
								text[i]->font_size, text[i]->spacing, text[i]->color);
			}
		}

		_raylib.endDrawing();
	}

	void WaitingScene::handleEvents() {
		resetButtonStates();
		if (_game.getGameStatus() == GameStatus::RUNNING) {
			_game.getSceneHandler().open("game");
			return;
		}

		if (_ignoreInitialClick && !_raylib.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			_ignoreInitialClick = false;
		}

		Vector2 mousePos = _raylib.getMousePosition();
		auto &positions = _registry.get_components<component::position>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i]) continue;

			if (mousePos.x > positions[i]->x && mousePos.x < positions[i]->x + drawables[i]->width &&
				mousePos.y > positions[i]->y && mousePos.y < positions[i]->y + drawables[i]->height) {
				hoverable[i]->isHovered = true;
				if (!_ignoreInitialClick && _raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && clickable[i]->enabled) {
					clickable[i]->isClicked = true;
					handleButtonClick(clickable[i]->id);
				}
			}
		}
	}

	void WaitingScene::init() {
		_registry.clear();
		_isOpen = true;
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);
		_ignoreInitialClick = true;

		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
		bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

		const std::string joinText =
			isFrench ? "Pret" :
			isItalian ? "Pronto" :
			"Ready";
		const std::string quitText =
			isFrench ? "Quitter" :
			isItalian ? "Uscire" :
			"Quit";

		const int buttonFontSize = 18;
		Vector2 joinTextSize = _raylib.measureTextEx(_font, joinText.c_str(), static_cast<float>(buttonFontSize), -0.5f);
		Vector2 quitTextSize = _raylib.measureTextEx(_font, quitText.c_str(), static_cast<float>(buttonFontSize), -0.5f);

		const float paddingX = 80.f;
		const float paddingY = 30.f;
		Vector2 joinButtonSize = {joinTextSize.x + paddingX, joinTextSize.y + paddingY};
		Vector2 quitButtonSize = {quitTextSize.x + paddingX, quitTextSize.y + paddingY};

		const float buttonSpacing = 28.f;
		const float stackHeight = joinButtonSize.y + quitButtonSize.y + buttonSpacing;
		const float originY = (static_cast<float>(_height) - stackHeight) / 2.f;
		const float centerX = static_cast<float>(_width) / 2.f;

		const float headingFontSize = 24.f;
		const std::string statusText =
			isFrench ? "En attente des joueurs..." :
			isItalian ? "In attesa dei giocatori..." :
			"Waiting for players...";
		Vector2 statusSize = _raylib.measureTextEx(_font, statusText.c_str(), headingFontSize, -0.5f);
		Vector2 statusPos = {centerX - statusSize.x / 2.f, originY - statusSize.y - 24.f};

		game::entities::create_text(
			_registry,
			statusPos,
			statusText,
			Color{190, 225, 255, 255},
			-0.5f,
			static_cast<int>(headingFontSize),
			_font
		);

		Vector2 joinButtonPos = {centerX - (joinButtonSize.x / 2.f), originY};
		Vector2 quitButtonPos = {centerX - (quitButtonSize.x / 2.f), originY + joinButtonSize.y + buttonSpacing};

		Color accentColor = Color{26, 170, 177, 255};

		game::entities::create_button(_registry, "button_ready", joinText,
									 joinButtonPos.x, joinButtonPos.y, 0.f,
									 joinButtonSize.x, joinButtonSize.y,
									 accentColor, RAYWHITE, buttonFontSize);
		game::entities::create_button(_registry, "button_quit", quitText,
									 quitButtonPos.x, quitButtonPos.y, 0.f,
									 quitButtonSize.x, quitButtonSize.y,
									 RED, RAYWHITE, buttonFontSize);
	}

	void WaitingScene::onClose() {
		_raylib.unloadFont(_font);
	}

	void WaitingScene::resetButtonStates() {
		auto &positions = _registry.get_components<component::position>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i]) continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;

			if (clickable[i]->id == "button_ready") {
				clickable[i]->enabled = (!_hasConfirmedReady && canJoin());
			}
		}
	}

	void WaitingScene::handleButtonClick(std::string const &id) {
		if (id == "button_ready") {
			_hasConfirmedReady = true;
			_game.getGameClient().sendConfirmStart();
		} else if (id == "button_quit") {
			_game.getGameClient().sendClientLeaveRoom();
			_game.getSceneHandler().open("room_select");
		}
	}

	bool WaitingScene::canJoin() {
		if (_game.getGameStatus() == GameStatus::FINISHED)
			return false;
		if (_game.getGameStatus() == GameStatus::WAITING_PLAYERS)
			return false;
		return true;
	}
} // namespace scene
