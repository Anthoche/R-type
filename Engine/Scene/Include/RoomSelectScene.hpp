/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomSelectScene
*/

#ifndef ROOMSELECTSCENE_HPP
    #define ROOMSELECTSCENE_HPP

    #include "../../Rendering/scene/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

    struct RoomButton {
        Rectangle rect;
        bool isHovered;
        bool isClicked;
    };

    struct RoomDisplay {
        Rectangle background;
        std::string name;
        std::string playersCount;
        RoomButton button;
    };

/**
 * @class RoomSelectScene
 * @brief Represents the room select scene of the game.
 *
 * The menu scene handles:
 * - Initialization of menu UI and resources.
 * - Rendering menu elements.
 * - Handling user input/events (e.g., navigating the menu).
 * - Cleaning up when the scene is closed.
 */
class RoomSelectScene: public AScene {
    public:
        /**
         * @brief Construct a RoomSelectScene with a reference to the game.
         * @param game The game instance.
         */
        RoomSelectScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~RoomSelectScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initialize the menu scene (UI, buttons, resources).
         */
        void init() override;

        /**
         * @brief Render the menu scene (draw UI elements).
         */
        void render() override;

        /**
         * @brief Handle input and events in the menu.
         */
        void handleEvents() override;

        /**
         * @brief Called when the menu scene is closed (cleanup).
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the game instance.

        Font _font{};
        Vector2 _margin{25, 25};

        // Rooms
        int const _roomSpacing = 30;
        int const _innerMargin = 15;
        int const _roomNameSize = 30;
        int const _playerCountSize = 15;
        int const _roomButtonTextSize = 18;
        Vector2 _roomSize{550, 70};
        Vector2 _baseRoomPosition{0, 90};
        Vector2 _roomJoinButtonSize{95, 40};
        int _currentRoomPosY = 0;
        Color const _roomBackgroundColor{33, 33, 33, 255};
        std::map<uint32_t, RoomDisplay> _rooms;

        // Title
        int _titleSize = 46;

        // Buttons
        int _selectedButtonIndex = -1;
        Color const _accentColor{26, 170, 177, 255};
        Vector2 const _buttonSize{160.f, 50.f};
        int const _buttonTextSize = 23;

        void refreshRooms();
        void displayRooms();
        void createRoom(uint32_t id, game::serializer::RoomData roomData);
        void resetButtonStates();
        void handleButtonClick(std::string const &id);
        void handleRoomJoinButton(uint32_t id);
};

} // namespace scene

#endif //ROOMSELECTSCENE_HPP
