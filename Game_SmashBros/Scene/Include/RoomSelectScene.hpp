/**
 * @file RoomSelectScene.hpp
 * @brief Room selection scene for multiplayer game lobbies
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#ifndef ROOMSELECTSCENE_HPP
    #define ROOMSELECTSCENE_HPP

    #include "../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../Game.hpp"
    #include <map>

namespace scene {

    /**
     * @struct RoomButton
     * @brief Represents an interactive button for a room
     */
    struct RoomButton {
        Rectangle rect;     ///< Button bounds and position
        bool isHovered;     ///< Whether mouse is hovering over the button
        bool isClicked;     ///< Whether the button is currently pressed
    };

    /**
     * @struct RoomDisplay
     * @brief Contains all visual elements for displaying a game room
     */
    struct RoomDisplay {
        Rectangle background;       ///< Background rectangle for the room card
        std::string name;          ///< Display name of the room
        std::string playersCount;  ///< Current player count text (e.g., "2/4")
        RoomButton button;         ///< Join button for the room
    };

/**
 * @class RoomSelectScene
 * @brief Scene for browsing and joining multiplayer game rooms
 *
 * Manages the room selection interface including:
 * - Displaying available game rooms with player counts
 * - Refreshing room list from server
 * - Handling room creation and joining
 * - User input for navigation and selection
 * - UI rendering and interaction feedback
 */
class RoomSelectScene: public AScene {
    public:
        /**
         * @brief Constructs the room selection scene
         * @param game Reference to the main game instance
         */
        RoomSelectScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~RoomSelectScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initializes room selection UI and resources
         * 
         * Sets up fonts, calculates layout positions, and requests
         * initial room list from the server.
         */
        void init() override;

        /**
         * @brief Renders the room selection interface
         * 
         * Draws the title, room list with player counts, join buttons,
         * and navigation elements.
         */
        void render() override;

        /**
         * @brief Processes user input and interactions
         * 
         * Handles mouse clicks on room buttons, hover states,
         * and keyboard navigation.
         */
        void handleEvents() override;

        /**
         * @brief Cleans up resources when the scene closes
         * 
         * Releases fonts, clears room data, and performs cleanup.
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the main game instance

        Font _font{};                ///< Font used for text rendering
        Vector2 _margin{25, 25};     ///< Screen margins for layout

        // Room display configuration
        int const _roomSpacing = 30;          ///< Vertical spacing between room cards
        int const _innerMargin = 15;          ///< Internal padding within room cards
        int const _roomNameSize = 30;         ///< Font size for room names
        int const _playerCountSize = 15;      ///< Font size for player count text
        int const _roomButtonTextSize = 18;   ///< Font size for join button text
        Vector2 _roomSize{550, 70};           ///< Dimensions of each room card
        Vector2 _baseRoomPosition{0, 90};     ///< Starting position for room list
        Vector2 _roomJoinButtonSize{95, 40};  ///< Dimensions of join buttons
        int _currentRoomPosY = 0;             ///< Current vertical scroll position
        Color const _roomBackgroundColor{33, 33, 33, 255}; ///< Background color for room cards
        std::map<uint32_t, RoomDisplay> _rooms; ///< Map of room IDs to display data

        // Title configuration
        int _titleSize = 46; ///< Font size for scene title

        // Button configuration
        int _selectedButtonIndex = -1;                ///< Index of currently selected button (-1 if none)
        Color const _accentColor{46, 204, 113, 255}; ///< Accent color for UI highlights (green)
        Vector2 const _buttonSize{160.f, 50.f};      ///< Standard button dimensions
        int const _buttonTextSize = 23;              ///< Font size for button text

        /**
         * @brief Requests and updates the room list from the server
         * 
         * Fetches current room data including names, player counts,
         * and availability status.
         */
        void refreshRooms();
        
        /**
         * @brief Renders all available rooms to the screen
         * 
         * Iterates through the room list and draws each room card
         * with its associated information and join button.
         */
        void displayRooms();
        
        /**
         * @brief Creates and adds a new room to the display list
         * @param id Unique identifier for the room
         * @param roomData Room information including name and player count
         * 
         * Constructs a RoomDisplay object and adds it to the internal
         * room map for rendering.
         */
        void createRoom(uint32_t id, game::serializer::RoomData roomData);
        
        /**
         * @brief Resets all button interaction states to default
         * 
         * Clears hover and click states for all buttons and rooms,
         * typically called at the start of event processing.
         */
        void resetButtonStates();
        
        /**
         * @brief Handles navigation button click actions
         * @param id Unique identifier of the clicked button
         * 
         * Executes actions for navigation buttons such as back,
         * refresh, or create new room.
         */
        void handleButtonClick(std::string const &id);
        
        /**
         * @brief Handles join button clicks for a specific room
         * @param id Room identifier to join
         * 
         * Initiates connection to the selected room and transitions
         * to the game scene if successful.
         */
        void handleRoomJoinButton(uint32_t id);
};

} // namespace scene

#endif //ROOMSELECTSCENE_HPP