/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomCreationScene
*/

#include "Include/RoomCreationScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"
#include "UITheme.hpp"

#include <algorithm>
#include <cctype>

namespace scene {

RoomCreationScene::RoomCreationScene(Game &game)
    : AScene(960, 540, "R-Type - Create Room"), _game(game) {}

void RoomCreationScene::init() {
    _registry.clear();
    _isOpen = true;
    _raylib.enableCursor();
    _raylib.setTargetFPS(60);
    _font = _raylib.loadFont(ASSETS_PATH "/fonts/Steelar-j9Vnj.otf");
    registerComponents();
    createLayout();
}

void RoomCreationScene::registerComponents() {
    _registry.register_component<component::position>();
    _registry.register_component<component::drawable>();
    _registry.register_component<component::text>();
    _registry.register_component<component::clickable>();
    _registry.register_component<component::hoverable>();
    _registry.register_component<component::type>();
}

RoomCreationScene::LocaleText RoomCreationScene::buildLocaleText() const {
    RoomCreationScene::LocaleText text{};
    switch (_game.getLanguage()) {
        case Game::Language::FRENCH:
            text = {"Créer une salle","Choisis le nom, le mode et la taille de ta salle.","Nom de salle","Mode de jeu","Joueurs min","Joueurs max","Créer","< Retour"};
            break;
        case Game::Language::ITALIAN:
            text = {"Crea una stanza","Scegli nome, modalità e dimensione della stanza.","Nome stanza","Modalità di gioco","Giocatori min","Giocatori max","Crea","< Indietro"};
            break;
        default:
            text = {"Create Room","Choose the room name, mode and player limits.","Room name","Game mode","Min players","Max players","Create room","< Back"};
            break;
    }
    return text;
}

void RoomCreationScene::createLayout() {
    LocaleText locale = buildLocaleText();
    float baseY = placeHeader(locale);
    buildSelectors(locale, baseY);
    buildActionButtons(locale, baseY);
}

float RoomCreationScene::placeHeader(const LocaleText &text) {
    Vector2 titlePos{_labelColumnX, 42.f};
    game::entities::create_text(_registry, titlePos, text.title, ui::theme::AccentText, -0.5f, 46, _font);
    game::entities::create_text(_registry, {_labelColumnX, titlePos.y + 48.f}, text.helper, ui::theme::SecondaryText, -0.5f, 16, _font);
    _controlsCenterX = _width * 0.68f;
    _roomName.clear();
    _nameInputActive = false;
    return titlePos.y + 110.f;
}

void RoomCreationScene::buildSelectors(const LocaleText &text, float baseY) {
    createNameRow(text.roomLabel, baseY);
    createModeSelector(text.modeLabel, baseY + 70.f);
    createValueRow(text.minLabel, baseY + 150.f, "button_min_decrease", "button_min_increase", _minValueButtonId, _minPlayers);
    createValueRow(text.maxLabel, baseY + 210.f, "button_max_decrease", "button_max_increase", _maxValueButtonId, _maxPlayers);
}

void RoomCreationScene::buildActionButtons(const LocaleText &text, float baseY) {
    float createWidth = 220.f;
    float createX = (_width - createWidth) / 2.f;
    makeButton("button_create_room_confirm", text.confirmLabel, createX, baseY + 280.f, createWidth, 48.f, _accentColor, ui::theme::AccentText, 24);
    makeButton("button_back", text.backLabel, _labelColumnX, baseY + 280.f, 130.f, 44.f, ui::theme::Secondary, ui::theme::SecondaryText, 20);
}

void RoomCreationScene::addLabel(const std::string &label, float y, int fontSize) {
    game::entities::create_text(_registry, {_labelColumnX, y + 6.f}, label, ui::theme::AccentText, -0.5f, fontSize, _font);
}

ecs::entity_t RoomCreationScene::makeButton(const std::string &id, const std::string &label, float x, float y, float width, float height, Color color, Color textColor, int fontSize) {
    return game::entities::create_button(_registry, id, label, x, y, 0.f, width, height, color, textColor, fontSize);
}

void RoomCreationScene::disableButton(ecs::entity_t entity) {
    auto &clickables = _registry.get_components<component::clickable>();
    if (entity < clickables.size() && clickables[entity])
        clickables[entity]->enabled = false;
}

void RoomCreationScene::createValueRow(const std::string &label, float y, const std::string &minusId,
                                       const std::string &plusId, const std::string &valueId, int value) {
    addLabel(label, y);
    float buttonWidth = 45.f;
    float buttonHeight = 42.f;
    float spacing = 14.f;
    float valueWidth = 110.f;
    float valueX = _controlsCenterX - valueWidth / 2.f;
    makeButton(minusId, "<", valueX - buttonWidth - spacing, y, buttonWidth, buttonHeight, ui::theme::Secondary, ui::theme::SecondaryText, 28);
    ecs::entity_t valueEntity = makeButton(valueId, std::to_string(value), valueX, y, valueWidth, buttonHeight, ui::theme::Neutral, ui::theme::NeutralText, 24);
    disableButton(valueEntity);
    makeButton(plusId, ">", valueX + valueWidth + spacing, y, buttonWidth, buttonHeight, ui::theme::Secondary, ui::theme::SecondaryText, 28);
}

void RoomCreationScene::createNameRow(const std::string &label, float y) {
    addLabel(label, y);
    float valueWidth = 200.f;
    float valueX = _controlsCenterX - valueWidth / 2.f;
    makeButton(_roomNameButtonId, "", valueX, y, valueWidth, 44.f, ui::theme::Neutral, ui::theme::NeutralText, 22);
    updateRoomNameDisplay();
}

void RoomCreationScene::createModeSelector(const std::string &label, float y) {
    _modeOptions = {
        {"Story levels", "Niveaux classiques", "Livelli classici", GameModeOption::LEVELS, true},
        {"Endless run", "Mode infini", "Modalità infinita", GameModeOption::ENDLESS, true},
        {"Custom map", "Créer un niveau", "Crea un livello", GameModeOption::CUSTOM, false}
    };
    _modeIndex = 0;
    _selectedMode = _modeOptions[_modeIndex].value;
    createModeRow(label, y);
    updateValueDisplay(_modeValueButtonId, 0);
}

void RoomCreationScene::createModeRow(const std::string &label, float y) {
    addLabel(label, y);
    float buttonWidth = 45.f;
    float buttonHeight = 42.f;
    float spacing = 14.f;
    float valueWidth = 230.f;
    float valueX = _controlsCenterX - valueWidth / 2.f;
    makeButton("button_mode_prev", "<", valueX - buttonWidth - spacing, y, buttonWidth, buttonHeight, ui::theme::Secondary, ui::theme::SecondaryText, 26);
    ecs::entity_t valueEntity = makeButton(_modeValueButtonId, getLocalizedModeLabel(_modeOptions[_modeIndex]), valueX, y, valueWidth, buttonHeight, ui::theme::Neutral, ui::theme::NeutralText, 22);
    disableButton(valueEntity);
    auto &texts = _registry.get_components<component::text>();
    if (valueEntity < texts.size() && texts[valueEntity])
        texts[valueEntity]->color = ui::theme::AccentText;
    makeButton("button_mode_next", ">", valueX + valueWidth + spacing, y, buttonWidth, buttonHeight, ui::theme::Secondary, ui::theme::SecondaryText, 26);
}

std::string RoomCreationScene::getLocalizedModeLabel(const ModeOptionData &option) const {
    switch (_game.getLanguage()) {
        case Game::Language::FRENCH: return option.labelFr;
        case Game::Language::ITALIAN: return option.labelIt;
        default: return option.labelEn;
    }
}

std::string RoomCreationScene::getRoomNamePlaceholder() const {
    switch (_game.getLanguage()) {
        case Game::Language::FRENCH: return "Nom (8 max)";
        case Game::Language::ITALIAN: return "Nome (8 max)";
        default: return "Name (8 max)";
    }
}

void RoomCreationScene::updateRoomNameDisplay() {
    auto &clickables = _registry.get_components<component::clickable>();
    auto &texts = _registry.get_components<component::text>();
    auto &drawables = _registry.get_components<component::drawable>();
    for (std::size_t i = 0; i < clickables.size() && i < texts.size() && i < drawables.size(); ++i) {
        if (!clickables[i] || clickables[i]->id != _roomNameButtonId)
            continue;
        if (texts[i]) {
            texts[i]->content = _roomName.empty() ? getRoomNamePlaceholder() : _roomName;
            texts[i]->color = _nameInputActive ? ui::theme::AccentText : ui::theme::NeutralText;
        }
        if (drawables[i])
            drawables[i]->color = _nameInputActive ? _accentColor : ui::theme::Neutral;
        break;
    }
}

void RoomCreationScene::handleNameInput() {
    bool updated = false;
    int codepoint = 0;
    while ((codepoint = _raylib.getCharPressed()) != 0) {
        if (_roomName.size() >= _roomNameMaxLength)
            continue;
        if (codepoint >= 32 && codepoint <= 126) {
            char c = static_cast<char>(codepoint);
            if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ' || c == '-' || c == '_') {
                _roomName.push_back(c);
                updated = true;
            }
        }
    }
    if (_raylib.isKeyPressed(KEY_BACKSPACE) && !_roomName.empty()) {
        _roomName.pop_back();
        updated = true;
    }
    if (_raylib.isKeyPressed(KEY_ENTER) || _raylib.isKeyPressed(KEY_KP_ENTER)) {
        _nameInputActive = false;
        updated = true;
    }
    if (updated)
        updateRoomNameDisplay();
}

void RoomCreationScene::render() {
    _raylib.beginDrawing();
    drawSceneBackground(_raylib, ui::theme::BackgroundTop, ui::theme::BackgroundBottom);
    drawButtons();
    drawTexts();
    _raylib.endDrawing();
}

void RoomCreationScene::drawButtons() {
    auto &positions = _registry.get_components<component::position>();
    auto &drawables = _registry.get_components<component::drawable>();
    auto &types = _registry.get_components<component::type>();
    auto &text = _registry.get_components<component::text>();
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (!positions[i] || i >= drawables.size() || i >= types.size())
            continue;
        if (!drawables[i] || !types[i] || types[i]->value != component::entity_type::BUTTON)
            continue;
        Vector2 pos{positions[i]->x, positions[i]->y};
        Vector2 size{drawables[i]->width, drawables[i]->height};
        bool hovered = (i < hoverable.size() && hoverable[i]) ? hoverable[i]->isHovered : false;
        bool clicked = (i < clickable.size() && clickable[i]) ? clickable[i]->isClicked : false;
        bool enabled = (i < clickable.size() && clickable[i]) ? clickable[i]->enabled : true;
        drawButton(_raylib, pos, size, text[i]->content, _font, text[i]->font_size, text[i]->spacing, drawables[i]->color, text[i]->color, hovered, clicked, enabled);
    }
}

void RoomCreationScene::drawTexts() {
    auto &positions = _registry.get_components<component::position>();
    auto &types = _registry.get_components<component::type>();
    auto &text = _registry.get_components<component::text>();
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (!positions[i] || i >= types.size() || i >= text.size())
            continue;
        if (!types[i] || types[i]->value != component::entity_type::TEXT || !text[i])
            continue;
        Vector2 pos{positions[i]->x, positions[i]->y};
        _raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
    }
}

void RoomCreationScene::handleEvents() {
    resetButtonStates();
    processMouseInteractions();
    if (_nameInputActive)
        handleNameInput();
}

void RoomCreationScene::processMouseInteractions() {
    Vector2 mousePos = _raylib.getMousePosition();
    auto &positions = _registry.get_components<component::position>();
    auto &drawables = _registry.get_components<component::drawable>();
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (!positions[i] || i >= drawables.size() || i >= clickable.size() || i >= hoverable.size())
            continue;
        if (!drawables[i] || !clickable[i] || !hoverable[i] || !clickable[i]->enabled)
            continue;
        Vector2 size{drawables[i]->width, drawables[i]->height};
        bool inside = mousePos.x > positions[i]->x && mousePos.x < positions[i]->x + size.x && mousePos.y > positions[i]->y && mousePos.y < positions[i]->y + size.y;
        if (!inside)
            continue;
        hoverable[i]->isHovered = true;
        if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clickable[i]->isClicked = true;
            handleButtonClick(clickable[i]->id);
        }
    }
}

void RoomCreationScene::handleButtonClick(const std::string &id) {
    if (id == _roomNameButtonId) {
        _nameInputActive = true;
        updateRoomNameDisplay();
        return;
    }
    if (_nameInputActive) {
        _nameInputActive = false;
        updateRoomNameDisplay();
    }
    if (id == "button_back") {
        _game.getSceneHandler().open("room_select");
        return;
    }
    if (id == "button_create_room_confirm") {
        submitRoom();
        return;
    }
    if (handlePlayerCountButton(id))
        return;
    handleModeButton(id);
}

bool RoomCreationScene::handlePlayerCountButton(const std::string &id) {
    if (id == "button_min_decrease") { changeMin(-1); return true; }
    if (id == "button_min_increase") { changeMin(1); return true; }
    if (id == "button_max_decrease") { changeMax(-1); return true; }
    if (id == "button_max_increase") { changeMax(1); return true; }
    return false;
}

bool RoomCreationScene::handleModeButton(const std::string &id) {
    if (id == "button_mode_prev") { changeMode(-1); return true; }
    if (id == "button_mode_next") { changeMode(1); return true; }
    return false;
}

void RoomCreationScene::changeMin(int delta) {
    _minPlayers = std::clamp(_minPlayers + delta, _minLimit, _maxPlayers);
    if (_minPlayers > _maxPlayers)
        _maxPlayers = _minPlayers;
    updateValueDisplay(_minValueButtonId, _minPlayers);
    updateValueDisplay(_maxValueButtonId, _maxPlayers);
}

void RoomCreationScene::changeMax(int delta) {
    _maxPlayers = std::clamp(_maxPlayers + delta, _minPlayers, _maxLimit);
    updateValueDisplay(_maxValueButtonId, _maxPlayers);
}

void RoomCreationScene::changeMode(int delta) {
    if (_modeOptions.empty())
        return;
    std::size_t next = nextEnabledModeIndex(delta);
    if (next == _modeIndex)
        return;
    _modeIndex = next;
    _selectedMode = _modeOptions[_modeIndex].value;
    updateValueDisplay(_modeValueButtonId, 0);
}

std::size_t RoomCreationScene::nextEnabledModeIndex(int delta) const {
    if (_modeOptions.empty())
        return 0;
    std::size_t modeCount = _modeOptions.size();
    int index = static_cast<int>(_modeIndex);
    for (std::size_t i = 0; i < modeCount; ++i) {
        index += delta;
        if (index < 0)
            index = static_cast<int>(modeCount) - 1;
        if (index >= static_cast<int>(modeCount))
            index = 0;
        if (_modeOptions[static_cast<std::size_t>(index)].enabled)
            return static_cast<std::size_t>(index);
    }
    return _modeIndex;
}

void RoomCreationScene::updateValueDisplay(const std::string &buttonId, int value) {
    auto &clickable = _registry.get_components<component::clickable>();
    auto &texts = _registry.get_components<component::text>();
    for (std::size_t i = 0; i < clickable.size() && i < texts.size(); ++i) {
        if (!clickable[i] || clickable[i]->id != buttonId || !texts[i])
            continue;
        if (buttonId == _modeValueButtonId) {
            texts[i]->content = getLocalizedModeLabel(_modeOptions[_modeIndex]);
            texts[i]->color = _modeOptions[_modeIndex].enabled ? ui::theme::AccentText : ui::theme::NeutralText;
        } else {
            texts[i]->content = std::to_string(value);
        }
        break;
    }
}

std::string RoomCreationScene::sanitizeRoomName() const {
    std::string finalName = _roomName;
    auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };
    finalName.erase(finalName.begin(), std::find_if_not(finalName.begin(), finalName.end(), isSpace));
    finalName.erase(std::find_if_not(finalName.rbegin(), finalName.rend(), isSpace).base(), finalName.end());
    if (finalName.empty()) {
        if (_game.getLanguage() == Game::Language::FRENCH)
            finalName = "Salle";
        else if (_game.getLanguage() == Game::Language::ITALIAN)
            finalName = "Stanza";
        else
            finalName = "Room";
    }
    if (finalName.size() > _roomNameMaxLength)
        finalName.resize(_roomNameMaxLength);
    return finalName;
}

void RoomCreationScene::submitRoom() {
    bool enableEndless = (_selectedMode == GameModeOption::ENDLESS);
    _game.setEndlessMode(enableEndless);
    _game.getGameClient().sendEndlessMode(enableEndless);
    _game.getGameClient().sendRoomCreate(static_cast<uint16_t>(_minPlayers), static_cast<uint16_t>(_maxPlayers), sanitizeRoomName());
    _game.getSceneHandler().open("room_select");
}

void RoomCreationScene::resetButtonStates() {
    auto &positions = _registry.get_components<component::position>();
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();
    for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
        if (!positions[i] || !clickable[i] || !hoverable[i])
            continue;
        clickable[i]->isClicked = false;
        hoverable[i]->isHovered = false;
    }
}

void RoomCreationScene::onClose() {
    _nameInputActive = false;
    _raylib.unloadFont(_font);
}

} // namespace scene
