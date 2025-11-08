/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomSelectScene
*/

#ifndef ROOMSELECTSCENE_HPP
    #define ROOMSELECTSCENE_HPP

    #include "../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../Game.hpp"
    #include <map>

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

        Font _font{}; ///< Font used for rendering text.
        Vector2 _margin{25, 25}; ///< General margin for UI elements.

        // Rooms
        int const _roomSpacing = 30; ///< Vertical spacing between rooms.
        int const _innerMargin = 15; ///< Inner margin inside room elements.
        int const _roomNameSize = 30; ///< Font size for room names.
        int const _playerCountSize = 15; ///< Font size for player count text.
        int const _roomButtonTextSize = 18; ///< Font size for room button text.
        Vector2 _roomSize{550, 70}; ///< Size of each room display box.
        Vector2 _baseRoomPosition{0, 90}; ///< Starting position for room display.
        Vector2 _roomJoinButtonSize{95, 40}; ///< Size of the join button.
        int _currentRoomPosY = 0; ///< Current Y position for the next room to render.
        Color const _roomBackgroundColor{37, 52, 86, 235}; ///< Background color of rooms.
        std::map<uint32_t, RoomDisplay> _rooms; ///< Map of rooms by their ID.

        // Title
        int _titleSize = 46; ///< Font size for the scene title.

        // Buttons
        int _selectedButtonIndex = -1; ///< Currently selected button index.
        int _selectedRoomIndex = -1; ///< Currently selected room index.
        bool _selectingRooms = false; ///< Indicates if user is selecting rooms or not
        Color const _accentColor{10, 34, 64, 255}; ///< Accent color for buttons.
        Vector2 const _buttonSize{160.f, 50.f}; ///< Size of general buttons.
        int const _buttonTextSize = 23; ///< Font size for general buttons.

        /**
         * @brief Refresh the list of rooms (update room data and UI).
         */
        void refreshRooms();

        /**
         * @brief Render all rooms on the scene.
         */
        void displayRooms();

        /**
         * @brief Create a room display entry.
         * @param id Unique identifier of the room.
         * @param roomData Data structure containing room information.
         */
        void createRoom(uint32_t id, game::serializer::RoomData roomData);

        /**
         * @brief Reset the hover and click states of all room buttons.
         */
        void resetButtonStates();

        /**
         * @brief Handle the click event for a specific button.
         * @param id The ID of the clicked button as a string.
         */
        void handleButtonClick(std::string const &id);

        /**
         * @brief Handle the action of joining a room.
         * @param id Unique identifier of the room to join.
         */
        void handleRoomJoinButton(uint32_t id);
};

} // namespace scene

#endif //ROOMSELECTSCENE_HPP
