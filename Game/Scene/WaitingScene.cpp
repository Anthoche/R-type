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
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <vector>

#include <format>
#include "Logger.hpp"
#include "WeaponDefinition.hpp"

namespace scene {
	WaitingScene::WaitingScene(Game &game)
		: AScene(960, 540, "R-Type - Waiting..."), _game(game) {
		_previewCenter = {static_cast<float>(_width) / 2.f, static_cast<float>(_height) / 2.f + 45.f};
		_previewBounds = {240.f, 150.f};
		_weaponPreviewCenter = {
			_previewCenter.x,
			_previewCenter.y + (_previewBounds.y * 0.5f) + (_weaponPreviewBounds.y * 0.5f) + 78.f
		};
		float maxWeaponCenterY = static_cast<float>(_height) - (_weaponPreviewBounds.y * 0.5f) - 36.f;
		if (_weaponPreviewCenter.y > maxWeaponCenterY) {
			_weaponPreviewCenter.y = maxWeaponCenterY;
		}
	}

	void WaitingScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(Color{6, 16, 28, 255});

		drawSkinPreview();
		drawWeaponPreview();

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

		if (_ignoreInitialClick && !_raylib.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			_ignoreInitialClick = false;
		}

		int key = _raylib.getKeyPressed();
		switch (key) {
			case KEY_LEFT:
			case KEY_A:
				selectPreviousSkin();
				break;
			case KEY_RIGHT:
			case KEY_D:
				selectNextSkin();
				break;
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
				if (!_ignoreInitialClick && _raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT) && clickable[i]->enabled) {
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
		_ignoreInitialClick = true;

		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		loadSkinOptions();
		loadWeaponOptions();

		bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
		bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

		std::string statusText =
			isFrench ? "En attente des joueurs..." :
			isItalian ? "In attesa dei giocatori..." :
			"Waiting for players...";
		int statusFontSize = 18;
		game::entities::create_text(_registry, {40.f, 36.f}, statusText,
									Color{170, 220, 255, 255}, -0.5f, statusFontSize, _font);

		std::string heading =
			isFrench ? "Hangar de personnalisation" :
			isItalian ? "Hangar di personalizzazione" :
			"Customization Hangar";
		int headingFontSize = 28;
		Vector2 headingSize = _raylib.measureTextEx(_font, heading.c_str(), static_cast<float>(headingFontSize), -0.5f);
		Vector2 headingPos = {_previewCenter.x - headingSize.x / 2.f, 108.f};
		game::entities::create_text(_registry, headingPos, heading,
									RAYWHITE, -0.5f, headingFontSize, _font);

		std::string instructions =
			isFrench ? "Choisis ton vaisseau avant de rejoindre la partie." :
			isItalian ? "Scegli la tua nave prima di entrare in partita." :
			"Pick your ship before entering the match.";
		int instructionsFontSize = 14;
		Vector2 instructionsSize = _raylib.measureTextEx(_font, instructions.c_str(), static_cast<float>(instructionsFontSize), -0.5f);
		Vector2 instructionsPos = {_previewCenter.x - instructionsSize.x / 2.f, headingPos.y + headingSize.y + 22.f};
		game::entities::create_text(_registry, instructionsPos, instructions,
									Color{180, 200, 220, 255}, -0.5f, instructionsFontSize, _font);

		std::string joinText =
			isFrench ? "Rejoindre" :
			isItalian ? "Unisciti" :
			"Join";
		std::string quitText =
			isFrench ? "Quitter" :
			isItalian ? "Uscire" :
			"Quit";

		int buttonFontSize = 18;
		Vector2 joinTextSize = _raylib.measureTextEx(_font, joinText.c_str(), static_cast<float>(buttonFontSize), -0.5f);
		Vector2 quitTextSize = _raylib.measureTextEx(_font, quitText.c_str(), static_cast<float>(buttonFontSize), -0.5f);

		float paddingX = 80.f;
		float paddingY = 30.f;
		Vector2 joinButtonSize = {joinTextSize.x + paddingX, joinTextSize.y + paddingY};
		Vector2 quitButtonSize = {quitTextSize.x + paddingX, quitTextSize.y + paddingY};

		float buttonSpacing = 28.f;
		float margin = 42.f;
		float stackHeight = joinButtonSize.y + quitButtonSize.y + buttonSpacing;
		float baseX = static_cast<float>(_width) - std::max(joinButtonSize.x, quitButtonSize.x) - margin;
		float baseY = static_cast<float>(_height) - stackHeight - margin;

		Vector2 joinButtonPos = {baseX, baseY};
		Vector2 quitButtonPos = {baseX, baseY + joinButtonSize.y + buttonSpacing};

		Color accentColor = Color{26, 170, 177, 255};

		game::entities::create_button(_registry, "button_join", joinText,
									 joinButtonPos.x, joinButtonPos.y, 0.f,
									 joinButtonSize.x, joinButtonSize.y,
									 accentColor, RAYWHITE, buttonFontSize);
		game::entities::create_button(_registry, "button_quit", quitText,
									 quitButtonPos.x, quitButtonPos.y, 0.f,
									 quitButtonSize.x, quitButtonSize.y,
									 RED, RAYWHITE, buttonFontSize);

		float arrowButtonSize = 54.f;
		float arrowSpacing = 36.f;
		Vector2 prevButtonPos = {_previewCenter.x - (_previewBounds.x / 2.f) - arrowSpacing - arrowButtonSize,
								 _previewCenter.y - arrowButtonSize / 2.f};
		Vector2 nextButtonPos = {_previewCenter.x + (_previewBounds.x / 2.f) + arrowSpacing,
								 _previewCenter.y - arrowButtonSize / 2.f};

		Color secondaryColor = Color{32, 104, 147, 255};
		Color arrowTextColor = Color{220, 245, 255, 255};

		game::entities::create_button(_registry, "button_skin_prev", "<",
									 prevButtonPos.x, prevButtonPos.y, 0.f,
									 arrowButtonSize, arrowButtonSize,
									 secondaryColor, arrowTextColor, 30);
		game::entities::create_button(_registry, "button_skin_next", ">",
									 nextButtonPos.x, nextButtonPos.y, 0.f,
									 arrowButtonSize, arrowButtonSize,
									 secondaryColor, arrowTextColor, 30);

		float weaponArrowButtonSize = 32.f;
		float weaponArrowSpacing = 18.f;
		Vector2 weaponPrevPos = {
			_weaponPreviewCenter.x - (_weaponPreviewBounds.x / 2.f) - weaponArrowSpacing - weaponArrowButtonSize,
			_weaponPreviewCenter.y - weaponArrowButtonSize / 2.f
		};
		Vector2 weaponNextPos = {
			_weaponPreviewCenter.x + (_weaponPreviewBounds.x / 2.f) + weaponArrowSpacing,
			_weaponPreviewCenter.y - weaponArrowButtonSize / 2.f
		};

		game::entities::create_button(_registry, "button_weapon_prev", "<",
									  weaponPrevPos.x, weaponPrevPos.y, 0.f,
									  weaponArrowButtonSize, weaponArrowButtonSize,
									  secondaryColor, arrowTextColor, 24);
		game::entities::create_button(_registry, "button_weapon_next", ">",
									  weaponNextPos.x, weaponNextPos.y, 0.f,
									  weaponArrowButtonSize, weaponArrowButtonSize,
									  secondaryColor, arrowTextColor, 24);
	}

	void WaitingScene::onClose() {
		unloadSkinTextures();
		unloadWeaponTextures();
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
				clickable[i]->enabled = canJoin();
			}
		}
		LOG_DEBUG(std::format("GameStatus: {}", static_cast<int>(_game.getGameStatus())));
	}

	void WaitingScene::handleButtonClick(std::string const &id) {
		if (id == "button_join") {
			_game.getSceneHandler().open("game");
		} else if (id == "button_quit") {
			close();
		} else if (id == "button_skin_prev") {
			selectPreviousSkin();
		} else if (id == "button_skin_next") {
			selectNextSkin();
		} else if (id == "button_weapon_prev") {
			selectPreviousWeapon();
		} else if (id == "button_weapon_next") {
			selectNextWeapon();
		}
	}

    void WaitingScene::loadSkinOptions() {
        unloadSkinTextures();
        _skinOptions.clear();

        const std::string previouslySelected = _game.getSelectedSkinPath();
        const std::filesystem::path playerDir{ASSETS_PATH "/sprites/player"};
        std::error_code ec;

        std::vector<std::filesystem::directory_entry> files;
        if (std::filesystem::exists(playerDir, ec)) {
            for (const auto &entry : std::filesystem::directory_iterator(playerDir, ec)) {
                if (!entry.is_regular_file())
                    continue;
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (ext != ".png")
                    continue;
                files.push_back(entry);
            }
        }

        std::sort(files.begin(), files.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.path().filename().string() < rhs.path().filename().string();
        });

        auto formatName = [](std::string stem) {
            if (stem.empty())
                return std::string("Unnamed");
            for (auto &ch : stem) {
                if (ch == '_' || ch == '-')
                    ch = ' ';
            }
            if (!stem.empty())
                stem[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(stem[0])));
            return stem;
        };

        for (const auto &entry : files) {
            const auto path = entry.path();
            const std::string stem = path.stem().string();
            const std::string fullPath = path.string();

            if (stem == "r-typesheet42" || stem == "r-typesheet2") {
                continue;
            }

            SkinOption option;
            option.name = formatName(stem);
            option.path = fullPath;
            option.filename = path.filename().string();

            option.texture = _raylib.loadTexture(option.path);
            if (option.texture.id == 0) {
                _skinOptions.push_back(std::move(option));
                continue;
            }

            option.source = {0.f, 0.f, static_cast<float>(option.texture.width), static_cast<float>(option.texture.height)};

            Image image = LoadImage(option.path.c_str());
            if (image.data != nullptr) {
                Rectangle alpha = GetImageAlphaBorder(image, 0.05f);
                if (alpha.width > 0.f && alpha.height > 0.f) {
                    option.source = alpha;
                    constexpr float desiredWidth = 33.f;
                    constexpr float desiredHeight = 16.f;

                    if (option.source.width > desiredWidth) {
                        float reduce = option.source.width - desiredWidth;
                        option.source.x += reduce * 0.5f;
                        option.source.width = desiredWidth;
                    }
                    if (option.source.height > desiredHeight) {
                        float reduce = option.source.height - desiredHeight;
                        option.source.y += reduce * 0.5f;
                        option.source.height = desiredHeight;
                    }

                    option.source.x = std::clamp(option.source.x, 0.f, static_cast<float>(option.texture.width) - option.source.width);
                    option.source.y = std::clamp(option.source.y, 0.f, static_cast<float>(option.texture.height) - option.source.height);
                }
                UnloadImage(image);
            }

            _skinOptions.push_back(std::move(option));
        }

        if (_skinOptions.empty()) {
            _skinOptions.push_back({"Default", "", "", {}, {0.f, 0.f, 0.f, 0.f}});
        }

        _currentSkinIndex = 0;
        if (!_skinOptions.empty() && !previouslySelected.empty()) {
            for (std::size_t idx = 0; idx < _skinOptions.size(); ++idx) {
                if (_skinOptions[idx].path == previouslySelected) {
                    _currentSkinIndex = idx;
                    break;
                }
            }
        }

        selectSkin(_currentSkinIndex);
    }

	void WaitingScene::loadWeaponOptions() {
		unloadWeaponTextures();
		_weaponOptions.clear();

		const std::string previouslySelectedPath = _game.getSelectedWeaponPath();
		const std::string previouslySelectedId = _game.getSelectedWeaponId();
		const std::filesystem::path weaponDir{ASSETS_PATH "/sprites/weapons"};
		std::error_code ec;

		std::vector<std::filesystem::directory_entry> files;
		if (std::filesystem::exists(weaponDir, ec)) {
			for (const auto &entry : std::filesystem::directory_iterator(weaponDir, ec)) {
				if (!entry.is_regular_file())
					continue;
				std::string ext = entry.path().extension().string();
				std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
					return static_cast<char>(std::tolower(c));
				});
				if (ext != ".png")
					continue;
				files.push_back(entry);
			}
		}

		std::sort(files.begin(), files.end(), [](const auto &lhs, const auto &rhs) {
			return lhs.path().filename().string() < rhs.path().filename().string();
		});

		auto formatName = [](std::string stem) {
			if (stem.empty())
				return std::string("Unnamed");
			for (auto &ch : stem) {
				if (ch == '_' || ch == '-')
					ch = ' ';
			}
			if (!stem.empty())
				stem[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(stem[0])));
			return stem;
		};

		auto hydrateTexture = [this](WeaponOption &option) {
			if (option.path.empty())
				return;
			option.texture = _raylib.loadTexture(option.path);
			if (option.texture.id == 0)
				return;
			option.source = {
				0.f, 0.f,
				static_cast<float>(option.texture.width),
				static_cast<float>(option.texture.height)
			};

			Image image = LoadImage(option.path.c_str());
			if (image.data != nullptr) {
				Rectangle alpha = GetImageAlphaBorder(image, 0.05f);
				if (alpha.width > 0.f && alpha.height > 0.f) {
					option.source = alpha;
					option.source.x = std::clamp(
						option.source.x, 0.f,
						static_cast<float>(option.texture.width) - option.source.width
					);
					option.source.y = std::clamp(
						option.source.y, 0.f,
						static_cast<float>(option.texture.height) - option.source.height
					);
				}
				UnloadImage(image);
			}
		};

		const std::string defaultWeaponId = "basic_shot";
		const std::filesystem::path defaultWeaponSprite = weaponDir / "guntmp.png";
		const weapon::WeaponDefinition &basicDef = weapon::getDefinition(defaultWeaponId);

		WeaponOption baseOption;
		baseOption.name = basicDef.displayName;
		baseOption.id = defaultWeaponId;
		if (std::filesystem::exists(defaultWeaponSprite, ec) &&
			std::filesystem::is_regular_file(defaultWeaponSprite)) {
			baseOption.path = defaultWeaponSprite.string();
			hydrateTexture(baseOption);
		}
		_weaponOptions.push_back(baseOption);

		for (const auto &entry : files) {
			const auto path = entry.path();
			const std::string stem = path.stem().string();
			const std::string fullPath = path.string();

			if (path == defaultWeaponSprite) {
				continue;
			}

			WeaponOption option;
			option.name = formatName(stem);
			option.path = fullPath;
			option.id = stem;
			if (weapon::hasDefinition(option.id)) {
				option.name = weapon::getDefinition(option.id).displayName;
			}
			hydrateTexture(option);
			_weaponOptions.push_back(std::move(option));
		}

		if (_weaponOptions.empty()) {
			WeaponOption option;
			option.name = "Default";
			option.id = defaultWeaponId;
			_weaponOptions.push_back(option);
		}

		if (_weaponOptions.size() > 1 && _weaponOptions.front().id != defaultWeaponId) {
			_weaponOptions.front().id = defaultWeaponId;
		}

		_currentWeaponIndex = 0;
		auto matchById = [&](const std::string &id) {
			if (id.empty())
				return false;
			for (std::size_t idx = 0; idx < _weaponOptions.size(); ++idx) {
				if (_weaponOptions[idx].id == id) {
					_currentWeaponIndex = idx;
					return true;
				}
			}
			return false;
		};
		auto matchByPath = [&](const std::string &pathValue) {
			if (pathValue.empty())
				return false;
			for (std::size_t idx = 0; idx < _weaponOptions.size(); ++idx) {
				if (_weaponOptions[idx].path == pathValue) {
					_currentWeaponIndex = idx;
					return true;
				}
			}
			return false;
		};

		if (!matchById(previouslySelectedId)) {
			matchByPath(previouslySelectedPath);
		}

		if (_weaponOptions.empty()) {
			_currentWeaponIndex = 0;
		} else {
			_currentWeaponIndex %= _weaponOptions.size();
		}

		selectWeapon(_currentWeaponIndex);
	}

	void WaitingScene::unloadSkinTextures() {
		for (auto &skin : _skinOptions) {
			if (skin.texture.id != 0) {
				_raylib.unloadTexture(skin.texture);
				skin.texture = {};
			}
		}
	}

	void WaitingScene::unloadWeaponTextures() {
		for (auto &weapon : _weaponOptions) {
			if (weapon.texture.id != 0) {
				_raylib.unloadTexture(weapon.texture);
				weapon.texture = {};
			}
		}
	}

	void WaitingScene::selectSkin(std::size_t index) {
		if (_skinOptions.empty()) {
			return;
		}

		_currentSkinIndex = index % _skinOptions.size();
		const auto &skin = _skinOptions[_currentSkinIndex];
		if (!skin.path.empty()) {
			_game.setSelectedSkinPath(skin.path);
			if (!skin.filename.empty()) {
				_game.getGameClient().sendSkinSelection(skin.filename);
			}
		}
	}

	void WaitingScene::selectWeapon(std::size_t index) {
		if (_weaponOptions.empty()) {
			_game.setSelectedWeaponPath("");
			_game.setSelectedWeaponId("basic_shot");
			_game.getGameClient().sendWeaponSelection("basic_shot");
			return;
		}

		_currentWeaponIndex = index % _weaponOptions.size();
		const auto &weapon = _weaponOptions[_currentWeaponIndex];
		_game.setSelectedWeaponPath(weapon.path);
		const std::string weaponId = !weapon.id.empty() ? weapon.id : "basic_shot";
		_game.setSelectedWeaponId(weaponId);
		_game.getGameClient().sendWeaponSelection(weaponId);
	}

	void WaitingScene::selectNextSkin() {
		if (_skinOptions.empty()) return;
		std::size_t next = (_currentSkinIndex + 1) % _skinOptions.size();
		selectSkin(next);
	}

	void WaitingScene::selectPreviousSkin() {
		if (_skinOptions.empty()) return;
		std::size_t prev = (_currentSkinIndex + _skinOptions.size() - 1) % _skinOptions.size();
		selectSkin(prev);
	}

	void WaitingScene::selectNextWeapon() {
		if (_weaponOptions.empty()) return;
		std::size_t next = (_currentWeaponIndex + 1) % _weaponOptions.size();
		selectWeapon(next);
	}

	void WaitingScene::selectPreviousWeapon() {
		if (_weaponOptions.empty()) return;
		std::size_t prev = (_currentWeaponIndex + _weaponOptions.size() - 1) % _weaponOptions.size();
		selectWeapon(prev);
	}

	void WaitingScene::drawSkinPreview() {
		Rectangle panel = {
			_previewCenter.x - (_previewBounds.x / 2.f) - 24.f,
			_previewCenter.y - (_previewBounds.y / 2.f) - 24.f,
			_previewBounds.x + 48.f,
			_previewBounds.y + 48.f
		};
		_raylib.drawRectangleRounded(panel, 0.18f, 12, Color{18, 42, 70, 255});

		Rectangle inner = {
			_previewCenter.x - (_previewBounds.x / 2.f) - 6.f,
			_previewCenter.y - (_previewBounds.y / 2.f) - 6.f,
			_previewBounds.x + 12.f,
			_previewBounds.y + 12.f
		};
		_raylib.drawRectangleRounded(inner, 0.12f, 12, Color{10, 24, 40, 255});

		if (_skinOptions.empty() || _skinOptions[_currentSkinIndex].texture.id == 0) {
			std::string fallback =
				(_game.getLanguage() == Game::Language::FRENCH) ? "Apercu indisponible" :
				(_game.getLanguage() == Game::Language::ITALIAN) ? "Anteprima non disponibile" :
				"Preview unavailable";
			Vector2 textSize = _raylib.measureTextEx(_font, fallback.c_str(), 18.f, -0.5f);
			Vector2 textPos = {_previewCenter.x - textSize.x / 2.f, _previewCenter.y - textSize.y / 2.f};
			_raylib.drawTextEx(_font, fallback.c_str(), textPos, 18.f, -0.5f, Color{180, 190, 200, 255});
			return;
		}

		SkinOption &current = _skinOptions[_currentSkinIndex];
		Texture2D &texture = current.texture;

		Rectangle source = current.source;
		if (source.width <= 0.f || source.height <= 0.f) {
			source = {0.f, 0.f, static_cast<float>(texture.width), static_cast<float>(texture.height)};
		}

		float scale = 1.f;
		if (source.width > 0.f && source.height > 0.f) {
			float fitW = _previewBounds.x / source.width;
			float fitH = _previewBounds.y / source.height;
			scale = std::min(fitW, fitH) * 0.92f;
		}

		Rectangle dest = {
			_previewCenter.x - (source.width * scale) / 2.f,
			_previewCenter.y - (source.height * scale) / 2.f,
			source.width * scale,
			source.height * scale
		};

		_raylib.drawTexturePro(texture, source, dest, {0.f, 0.f}, 0.f, WHITE);

		std::string const &skinName = current.name;
		if (!skinName.empty()) {
			float labelSize = 20.f;
			Vector2 textSize = _raylib.measureTextEx(_font, skinName.c_str(), labelSize, -0.5f);
			Vector2 textPos = {_previewCenter.x - textSize.x / 2.f, panel.y + panel.height + 12.f};
			_raylib.drawTextEx(_font, skinName.c_str(), textPos, labelSize, -0.5f, Color{210, 235, 255, 255});
		}
	}

	void WaitingScene::drawWeaponPreview() {
		Rectangle panel = {
			_weaponPreviewCenter.x - (_weaponPreviewBounds.x / 2.f) - 14.f,
			_weaponPreviewCenter.y - (_weaponPreviewBounds.y / 2.f) - 14.f,
			_weaponPreviewBounds.x + 28.f,
			_weaponPreviewBounds.y + 28.f
		};
		_raylib.drawRectangleRounded(panel, 0.16f, 10, Color{14, 34, 56, 255});

		Rectangle inner = {
			_weaponPreviewCenter.x - (_weaponPreviewBounds.x / 2.f) - 4.f,
			_weaponPreviewCenter.y - (_weaponPreviewBounds.y / 2.f) - 4.f,
			_weaponPreviewBounds.x + 8.f,
			_weaponPreviewBounds.y + 8.f
		};
		_raylib.drawRectangleRounded(inner, 0.12f, 10, Color{8, 20, 34, 255});

		if (_weaponOptions.empty() || _weaponOptions[_currentWeaponIndex].texture.id == 0) {
			std::string fallback =
				(_game.getLanguage() == Game::Language::FRENCH) ? "Arme indisponible" :
				(_game.getLanguage() == Game::Language::ITALIAN) ? "Arma non disponibile" :
				"Weapon unavailable";
			Vector2 textSize = _raylib.measureTextEx(_font, fallback.c_str(), 13.f, -0.5f);
			Vector2 textPos = {_weaponPreviewCenter.x - textSize.x / 2.f, _weaponPreviewCenter.y - textSize.y / 2.f};
			_raylib.drawTextEx(_font, fallback.c_str(), textPos, 13.f, -0.5f, Color{170, 190, 210, 255});
			return;
		}

		WeaponOption &current = _weaponOptions[_currentWeaponIndex];
		Texture2D &texture = current.texture;

		Rectangle source = current.source;
		if (source.width <= 0.f || source.height <= 0.f) {
			source = {0.f, 0.f, static_cast<float>(texture.width), static_cast<float>(texture.height)};
		}

		float scale = 1.f;
		if (source.width > 0.f && source.height > 0.f) {
			float fitW = _weaponPreviewBounds.x / source.width;
			float fitH = _weaponPreviewBounds.y / source.height;
			scale = std::min(fitW, fitH) * 0.95f;
		}

		Rectangle dest = {
			_weaponPreviewCenter.x - (source.width * scale) / 2.f,
			_weaponPreviewCenter.y - (source.height * scale) / 2.f,
			source.width * scale,
			source.height * scale
		};

		_raylib.drawTexturePro(texture, source, dest, {0.f, 0.f}, 0.f, WHITE);

		const weapon::WeaponDefinition &definition = weapon::getDefinition(!current.id.empty() ? current.id : "basic_shot");

		std::string displayName = !current.name.empty() ? current.name : definition.displayName;
		float labelSize = 14.f;
		Vector2 textSize = _raylib.measureTextEx(_font, displayName.c_str(), labelSize, -0.5f);
		Vector2 textPos = {_weaponPreviewCenter.x - textSize.x / 2.f, panel.y + panel.height + 6.f};
		_raylib.drawTextEx(_font, displayName.c_str(), textPos, labelSize, -0.5f, Color{200, 225, 240, 255});

		auto formatFloat = [](float value, int precision = 2) {
			return std::format("{:.{}f}", value, precision);
		};

		std::string ammoLine = definition.infiniteAmmo
			? "Ammo: INF"
			: std::format("Ammo: {}", definition.ammoCapacity);
		std::string rateLine = std::format("Cooldown: {}s", formatFloat(definition.fireCooldown, 2));
		std::string dmgLine = std::format("Damage: {}", formatFloat(definition.damage, 1));

		float detailsSize = 12.f;
		Vector2 ammoSize = _raylib.measureTextEx(_font, ammoLine.c_str(), detailsSize, -0.5f);
		Vector2 rateSize = _raylib.measureTextEx(_font, rateLine.c_str(), detailsSize, -0.5f);
		Vector2 dmgSize = _raylib.measureTextEx(_font, dmgLine.c_str(), detailsSize, -0.5f);

		float firstLineY = textPos.y + ammoSize.y + 4.f;
		Vector2 ammoPos = {_weaponPreviewCenter.x - ammoSize.x / 2.f, firstLineY};
		Vector2 ratePos = {_weaponPreviewCenter.x - rateSize.x / 2.f, ammoPos.y + rateSize.y + 2.f};
		Vector2 dmgPos = {_weaponPreviewCenter.x - dmgSize.x / 2.f, ratePos.y + dmgSize.y + 2.f};

		Color detailColor = Color{190, 215, 235, 255};
		_raylib.drawTextEx(_font, ammoLine.c_str(), ammoPos, detailsSize, -0.5f, detailColor);
		_raylib.drawTextEx(_font, rateLine.c_str(), ratePos, detailsSize, -0.5f, detailColor);
		_raylib.drawTextEx(_font, dmgLine.c_str(), dmgPos, detailsSize, -0.5f, detailColor);
	}

	bool WaitingScene::canJoin() {
		if (_game.getGameStatus() == GameStatus::FINISHED)
			return false;
		if (_game.getGameStatus() == GameStatus::WAITING_PLAYERS)
			return false;
		return true;
	}
} // namespace scene
