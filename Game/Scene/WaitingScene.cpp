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

namespace scene {
	WaitingScene::WaitingScene(Game &game)
		: AScene(960, 540, "R-Type - Waiting..."), _game(game) {}

	void WaitingScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

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

		switch (_raylib.getKeyPressed()) {
			default:
				break;
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
				if (_raylib.isMouseButtonDown(MOUSE_BUTTON_LEFT) && clickable[i]->enabled) {
					clickable[i]->isClicked = true;
					handleButtonClick(clickable[i]->id);
				}
			}
		}
	}

	void WaitingScene::init() {
		_isOpen = true;
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);

		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
		bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);
		std::string title = 
			isFrench ? "En attente des joueurs..." : 
			isItalian ? "In attesa dei giocatori..." :
			"Waiting for players...";
		int titleFontSize = 38;
		Vector2 titleSize = _raylib.measureTextEx(_font, title.c_str(), titleFontSize, -0.5f);
		float titleCenterY = getElementCenter(_height, titleSize.y) - 100;
		float titleCenterX = _width / 2 - titleSize.x / 2;

		std::string joinText = 
			isFrench ? "Rejoindre" : 
			isItalian ? "Guintura" :
			"Join";
		std::string quitText = 
			isFrench ? "Quitter" :
			isItalian ? "Uscire" :
			"Quit";
		Vector2 joinTextSize = _raylib.measureTextEx(_font, joinText.c_str(), 23, -0.5f);
		Vector2 quitTextSize = _raylib.measureTextEx(_font, quitText.c_str(), 23, -0.5f);

		float paddingX = 120.f;
		float paddingY = 40.f;
		Vector2 joinButtonSize = {joinTextSize.x + paddingX, joinTextSize.y + paddingY};
		Vector2 quitButtonSize = {quitTextSize.x + paddingX, quitTextSize.y + paddingY};

		Vector2 buttonBasePos = {getElementCenter(_width, joinButtonSize.x),
								getElementCenter(_height, joinButtonSize.y) + 50};
		Vector2 joinButtonPos = buttonBasePos;
		Vector2 quitButtonPos = buttonBasePos;
		joinButtonPos.x -= (joinButtonSize.x / 2 + 30);
		quitButtonPos.x += (quitButtonSize.x / 2 + 30);

		Color accentColor = Color{26, 170, 177, 255};

		game::entities::create_text(_registry, {titleCenterX, titleCenterY}, title,
									RAYWHITE, -0.5f, titleFontSize, _font);
		game::entities::create_button(_registry, "button_join", joinText,
                             joinButtonPos.x, joinButtonPos.y, 0.f,
                             joinButtonSize.x, joinButtonSize.y,
                             accentColor, RAYWHITE, 23);
		game::entities::create_button(_registry, "button_quit", quitText,
									quitButtonPos.x, quitButtonPos.y, 0.f,
									quitButtonSize.x, quitButtonSize.y,
									RED, RAYWHITE, 23);

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

			if (clickable[i]->id == "button_join") {
				if (_game.getGameStatus() == GameStatus::PENDING_START ||
					_game.getGameStatus() == GameStatus::RUNNING) {
					clickable[i]->enabled = true;
				} else {
					clickable[i]->enabled = false;
				}
			}
		}
	}

	void WaitingScene::handleButtonClick(std::string const &id) {
		if (id == "button_join") {
			_game.getSceneHandler().open("game");
		} else if (id == "button_quit") {
			close();
		}
	}
} // namespace scene
