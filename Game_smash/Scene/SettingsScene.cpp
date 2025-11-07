/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/

#include "Include/SettingsScene.hpp"
#include "components.hpp"
#include "background.hpp"
#include "button.hpp"
#include "text.hpp"

namespace scene {
	SettingsScene::SettingsScene(Game &game)
		: AScene(960, 540, "R-Type - Settings"), _game(game) {}

	void SettingsScene::init() {
		_registry.clear();
		_isOpen = true;
		setupScene();
		registerComponents();
		createTitle();
		createButtons();
		createBackButton();
	}

	void SettingsScene::setupScene() {
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);
		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");
	}

	void SettingsScene::registerComponents() {
		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();
	}

	void SettingsScene::createTitle() {
		Vector2 titleSize = _raylib.measureTextEx(_font, _sceneTitle.c_str(), _titleSize, -0.5f);
		Vector2 titlePos = {(_width - titleSize.x) / 2.0f, 50.0f};

		game::entities::create_text(_registry, titlePos, _sceneTitle, RAYWHITE, -0.5f, _titleSize, _font);
	}

	void SettingsScene::createButtons() {
		float y = 190.f;

		std::vector<ButtonCreator> creators = {
			&SettingsScene::createSoundButton,
			&SettingsScene::createLanguageButton
		};

		for (size_t i = 0; i < _buttons.size(); ++i) {
			game::entities::create_text(
				_registry,
				{_buttonPosition.x, y},
				_buttons[i],
				_accentColor,
				-1.0f,
				_buttonTextSize,
				_font
			);

			Vector2 pos = {_buttonPosition.x + 350.f, y - 10.f};
			if (i < creators.size())
				(this->*creators[i])(pos, i);
			else
				createDefaultText(pos, i);
			y += _buttonSize.y + _buttonSpacing;
		}
	}

	void SettingsScene::createSoundButton(Vector2 pos, std::size_t i) {
		float textWidth = _raylib.measureTextEx(_font, _values[i].c_str(), _buttonTextSize - 4, -1.0f).x;
		float padding = 40.f;
		float buttonWidth = std::max(100.f, textWidth + padding);

		game::entities::create_button(_registry, "button_sound", _values[i],
			pos.x, pos.y, 0.f, buttonWidth, 40.f, _accentColor, BLACK, _buttonTextSize - 4
		);
	}

	void SettingsScene::createLanguageButton(Vector2 pos, std::size_t i) {
		game::entities::create_button(_registry, "button_language", _values[i],
			pos.x, pos.y, 0.f, 150.f, 40.f, _accentColor, BLACK, _buttonTextSize - 4
		);
	}

	void SettingsScene::createDefaultText(Vector2 pos, std::size_t i) {
		game::entities::create_text(_registry, pos, _values[i], RAYWHITE, -1.0f, _buttonTextSize - 4, _font);
	}

	void SettingsScene::createBackButton() {
		std::string label = (
    		_currentLanguage == Game::Language::FRENCH ? "< Retour" :
    		_currentLanguage == Game::Language::ITALIAN ? "< Indietro" :
    		"< Back"
		);
		
		auto &texts = _registry.get_components<component::text>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &drawables = _registry.get_components<component::drawable>();

		bool found = false;
		for (std::size_t i = 0; i < clickable.size(); ++i) {
			if (clickable[i] && clickable[i]->id == "button_back") {
				texts[i]->content = label;

				float textWidth = _raylib.measureTextEx(_font, label.c_str(), texts[i]->font_size, texts[i]->spacing).x;
				float padding = 40.f;
				drawables[i]->width = std::max(150.f, textWidth + padding);
				drawables[i]->color = _accentColor;
				texts[i]->color = BLACK;

				found = true;
				break;
			}
		}

		if (!found) {
			float textWidth = _raylib.measureTextEx(_font, label.c_str(), 23, -1.0f).x;
			float padding = 40.f;
			float buttonWidth = std::max(150.f, textWidth + padding);

			Vector2 backPos = {40.f, static_cast<float>(_height - 80)};
			Vector2 backSize = {buttonWidth, 50.f};

			game::entities::create_button(_registry, "button_back", label, backPos.x, backPos.y, 0.f,
				buttonWidth, backSize.y, _accentColor, BLACK, 23);
		}
	}


	void SettingsScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

		auto &drawables = _registry.get_components<component::drawable>();
		auto &positions = _registry.get_components<component::position>();
		auto &types = _registry.get_components<component::type>();
		auto &text = _registry.get_components<component::text>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size(); ++i) {
			if (!positions[i] || !types[i]) continue;

			Vector2 pos = {positions[i]->x, positions[i]->y};
			if (types[i]->value == component::entity_type::BUTTON) {
				Vector2 size = {drawables[i]->width, drawables[i]->height};
				std::string content = text[i]->content;
				int fontSize = text[i]->font_size;
				float spacing = text[i]->spacing;
				Color textColor = text[i]->color;

				drawButton(pos, size, content, fontSize, spacing,
					_accentColor, textColor,
					hoverable[i]->isHovered, clickable[i]->isClicked
				);
			}

			if (types[i]->value == component::entity_type::TEXT) {
				_raylib.drawTextEx(
					text[i]->font,
					text[i]->content,
					pos,
					text[i]->font_size,
					text[i]->spacing,
					text[i]->color
				);
			}
		}
		_raylib.endDrawing();
	}

	void SettingsScene::handleEvents() {
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
				if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					clickable[i]->isClicked = true;
					handleButtonClick(clickable[i]->id);
				}
			}
		}

		if (_raylib.isGamepadAvailable(0)) {
			// Button action
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
				handleButtonClick("button_back");
			}
		}
	}

	void SettingsScene::handleButtonClick(std::string const &id) {
		if (id == "button_back") {
			_game.getSceneHandler().open("menu");
			return;
		}
		if (id == "button_sound") toggleSound();
		else if (id == "button_language") toggleLanguage();
	}
  
	void SettingsScene::toggleSound() {
		_soundOn = !_soundOn;
		_game.setSoundEnabled(_soundOn);

		if (_currentLanguage == Game::Language::ENGLISH)
			_values[0] = _soundOn ? "On" : "Off";
		else if (_currentLanguage == Game::Language::FRENCH)
			_values[0] = _soundOn ? "Active" : "Desactive";
		else
			_values[0] = _soundOn ? "Attivo" : "Disattivo";

		updateButtonText("button_sound", _values[0]);
	}

	void SettingsScene::toggleLanguage() {
		if (_currentLanguage == Game::Language::ENGLISH) {
			_currentLanguage = Game::Language::FRENCH;
			translateToFrench();
		} else if (_currentLanguage == Game::Language::FRENCH) {
			_currentLanguage = Game::Language::ITALIAN;
			translateToItalian();
		} else {
			_currentLanguage = Game::Language::ENGLISH;
			translateToEnglish();
		}

		updateAllTexts();
		updateAllButtonValues();
		createBackButton();
		_game.setLanguage(_currentLanguage);
	}
	
	void SettingsScene::translateToItalian() {
		_buttons = {"1. Suono", "2. Lingua"};
		_values[0] = _soundOn ? "Attivo" : "Disattivo";
		_values[1] = "Italiano";
	}

	void SettingsScene::translateToFrench() {
		_buttons = {"1. Son", "2. Langue"};
		_values[0] = _soundOn ? "Active" : "Desactive";
		_values[1] = "Francais";
	}

	void SettingsScene::translateToEnglish() {
		_buttons = {"1. Sound", "2. Language"};
		_values[0] = _soundOn ? "On" : "Off";
		_values[1] = "English";
	}

	void SettingsScene::updateAllButtonValues() {
		auto &clickable = _registry.get_components<component::clickable>();
		auto &texts = _registry.get_components<component::text>();
		auto &drawables = _registry.get_components<component::drawable>();

		std::vector<std::string> buttonIds = {"button_sound", "button_language"};

		for (size_t i = 0; i < buttonIds.size(); ++i) {
			for (size_t j = 0; j < clickable.size(); ++j) {
				if (clickable[j] && clickable[j]->id == buttonIds[i] && texts[j] && drawables[j]) {
					texts[j]->content = _values[i];

					float textWidth = _raylib.measureTextEx(_font, _values[i].c_str(), texts[j]->font_size, texts[j]->spacing).x;
					float padding = 40.f;
					drawables[j]->width = std::max(drawables[j]->width, textWidth + padding);
				}
			}
		}
	}


	void SettingsScene::updateAllTexts() {
		auto &texts = _registry.get_components<component::text>();
		auto &types = _registry.get_components<component::type>();
		auto &clickable = _registry.get_components<component::clickable>();

		std::size_t buttonIndex = 0;
		for (std::size_t i = 0; i < texts.size() && i < types.size(); ++i) {
			if (!texts[i] || !types[i])
				continue;

			if (i < clickable.size() && clickable[i])
				continue;

			if (types[i]->value == component::entity_type::TEXT && texts[i]->content != _sceneTitle) {
				if (buttonIndex < _buttons.size()) {
					texts[i]->content = _buttons[buttonIndex];
					++buttonIndex;
				}
			}
		}
	}

	void SettingsScene::updateButtonText(const std::string &buttonId, const std::string &newText) {
		auto &texts = _registry.get_components<component::text>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &drawables = _registry.get_components<component::drawable>();

		for (std::size_t i = 0; i < clickable.size(); ++i) {
			if (clickable[i] && clickable[i]->id == buttonId && texts[i] && drawables[i]) {
				texts[i]->content = newText;

				float textWidth = _raylib.measureTextEx(_font, newText.c_str(), texts[i]->font_size, texts[i]->spacing).x;
				float padding = 40.f;
				drawables[i]->width = std::max(100.f, textWidth + padding);
			}
		}
	}

	void SettingsScene::drawButton(Vector2 position, Vector2 size, std::string const &content,
								int fontSize, float spacing, Color color, Color textColor,
								bool isHovered, bool isClicked) {
		Vector2 textPos = {position.x + size.x / 2, position.y + size.y / 2};
		Vector2 textSize = _raylib.measureTextEx(_font, content, fontSize, spacing);
		textPos.x -= textSize.x / 2;
		textPos.y -= textSize.y / 2;

		Rectangle rect = {position.x, position.y, size.x, size.y};

		if (isHovered) {
			Color temp = textColor;
			textColor = color;
			color = temp;
		}
		if (isClicked) color.a -= 50;

		_raylib.drawRectangleRounded(rect, 0.5, 10, color);
		_raylib.drawTextEx(_font, content, textPos, fontSize, spacing, textColor);
	}

	void SettingsScene::resetButtonStates() {
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();
		auto &positions = _registry.get_components<component::position>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i])
				continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;
		}
	}

	void SettingsScene::onClose() {
		_raylib.unloadFont(_font); 
	}

	int SettingsScene::getCenterY(int screenHeight, int elementHeight) const {
		return (screenHeight / 2) - (elementHeight / 2);
	}

	int SettingsScene::getButtonsCenterY(int screenHeight, int numberOfButtons, 
										int buttonHeight, int buttonSpacing) const {
		int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
		
		return getCenterY(screenHeight, totalHeight);
	}
} // namespace scene
