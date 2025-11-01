/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerWaitScene
*/

#include <format>
#include "Include/ServerWaitScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"

namespace scene {
	ServerWaitScene::ServerWaitScene(Game &game)
		: AScene(960, 540, "R-Type - Waiting for server..."), _game(game) {
		_startTime = 0;
		_endTime = 0;
		_attemptsCount = 0;
	}

	void ServerWaitScene::init() {
		_registry.clear();
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
			isFrench ? "En attente de la connexion au serveur..." :
			isItalian ? "In attesa della connessione al server..." :
						"Waiting for server connection...";

		int titleFontSize = 30;
		Vector2 titleSize = _raylib.measureTextEx(_font, title.c_str(), titleFontSize, -0.75f);
		float titleCenterY = getElementCenter(_height, titleSize.y) - 100;
		float titleCenterX = getElementCenter(_width, titleSize.x);

		std::string subtitle =
			isFrench ? std::format("Nouvelle tentative dans {:.1f} seconde(s)", 10.f) :
			isItalian ? std::format("Prossimo tentativo tra {:.1f} secondo(i)", 10.f) :
						std::format("Next attempt in {:.1f} second(s)", 10.f);
		int subtitleFontSize = 18;
		Vector2 subtitleSize = _raylib.measureTextEx(_font, subtitle.c_str(), subtitleFontSize, -0.5f);
		float subtitleCenterY = getElementCenter(_height, subtitleSize.y) - 50;
		float subtitleCenterX = getElementCenter(_width, subtitleSize.x);

		Vector2 buttonSize = {180.f, 70.f};
		Vector2 buttonBasePos = {getElementCenter(_width, buttonSize.x), getElementCenter(_height, buttonSize.y) + 70};
		Vector2 playButtonPos = buttonBasePos;
		Vector2 quitButtonPos = buttonBasePos;
		playButtonPos.x -= 110;
		quitButtonPos.x += 110;

		Color accentColor = Color{26, 170, 177, 255};

		game::entities::create_text(_registry, {titleCenterX, titleCenterY}, title,
			RAYWHITE, -0.75f, titleFontSize, _font);
		game::entities::create_text(_registry, {subtitleCenterX, subtitleCenterY}, subtitle,
			RAYWHITE, -0.5f, subtitleFontSize, _font);

		game::entities::create_button(_registry, "button_play", isFrench ? "Jouer" : isItalian ? "Gioca" : "Play",
			playButtonPos.x, playButtonPos.y, 0.f, buttonSize.x, buttonSize.y, accentColor, RAYWHITE);

		game::entities::create_button(_registry, "button_quit", isFrench ? "Quitter" : isItalian ? "Uscire" : "Quit",
			quitButtonPos.x, quitButtonPos.y, 0.f, buttonSize.x, buttonSize.y, RED, RAYWHITE);
	}


	void ServerWaitScene::render() {
		double remainingTime = getRemainingTime();
		bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
		bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

		if (_attemptsCount < 3 && remainingTime <= 0.1) {
			_attemptsCount += 1;
			resetTimer();
		}

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
				if (text[i]->font_size == 18) {
					if (_attemptsCount >= 3) {
						text[i]->content = 
						isFrench ? "Connexion au serveur impossible" :
						isItalian ? "Impossible connettersi al server" :
						 "Unable to connect to server";
						text[i]->color = RED;
					} else {
						text[i]->content = 
						isFrench ? std::format("Nouvelle tentative dans {:.1f} seconde(s)", remainingTime) :
						isItalian ? std::format("Riprova tra {:.1f} secondo(i))", remainingTime) :
						std::format("Next attempt in {:.1f} second(s)", remainingTime);
					}
					Vector2 size = _raylib.measureTextEx(_font, text[i]->content.c_str(), text[i]->font_size, text[i]->spacing);
					pos.x = getElementCenter(_width, size.x);
				}
				_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
			}
		}

		_raylib.endDrawing();
	}

	void ServerWaitScene::handleEvents() {
		resetButtonStates();

		Vector2 mousePos = _raylib.getMousePosition();
		auto &positions = _registry.get_components<component::position>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i])
				continue;

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

	void ServerWaitScene::onClose() {
		_raylib.unloadFont(_font);
	}

	double ServerWaitScene::getRemainingTime() {
		double currentTime = _raylib.getTime();
		return _endTime - currentTime;
	}

	void ServerWaitScene::resetTimer() {
		_startTime = _raylib.getTime();
		_endTime = _startTime + 10.0f;
	}

	void ServerWaitScene::resetButtonStates() {
		auto &positions = _registry.get_components<component::position>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i]) continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;
			clickable[i]->enabled = true;
		}
	}

	void ServerWaitScene::handleButtonClick(std::string const &id) {
		if (id == "button_play") {
			_game.getSceneHandler().open("game");
		} else if (id == "button_quit") {
			close();
		}
	}
} // namespace scene
