/**
 * @file WaitingScene.hpp
 * @brief Multiplayer lobby waiting scene for player synchronization
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#ifndef RTYPE_WAITINGSCENE_HPP
	#define RTYPE_WAITINGSCENE_HPP

    #include "../../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../Game.hpp"
    #include <string>

namespace scene {

/**
 * @class WaitingScene
 * @brief Lobby waiting scene for multiplayer game preparation
 *
 * Manages the pre-game lobby state where players gather before match start:
 * - Displays connected players and their ready status
 * - Handles player ready/unready state confirmation
 * - Waits for all players to confirm ready before starting
 * - Provides real-time updates on player connections
 * - Allows players to leave the lobby
 * - Shows game start conditions and countdown
 */
class WaitingScene: public AScene {
    public:
        /**
         * @brief Constructs the waiting lobby scene
         * @param game Reference to the main game instance
         */
        WaitingScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~WaitingScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initializes the waiting lobby UI and network listeners
         * 
         * Sets up fonts, player list display, ready button,
         * and registers for player connection updates.
         */
        void init() override;

        /**
         * @brief Renders the lobby interface with player status
         * 
         * Draws the player list, ready states, current player count,
         * ready button, and any countdown or status messages.
         */
        void render() override;

        /**
         * @brief Processes user input and network updates
         * 
         * Handles ready button clicks, leave button interactions,
         * and processes incoming player status updates from server.
         */
        void handleEvents() override;

        /**
         * @brief Cleans up resources when leaving the lobby
         * 
         * Unregisters network listeners, releases fonts,
         * and notifies server of departure if needed.
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the main game instance

        Font _font{}; ///< Font used for text rendering

        bool _ignoreInitialClick{false};  ///< Flag to prevent accidental clicks during scene initialization
        bool _hasConfirmedReady{false};   ///< Whether the local player has confirmed ready status

        /**
         * @brief Resets all button interaction states to default
         * 
         * Clears hover and click states for all interactive buttons,
         * typically called at the start of event processing each frame.
         */
        void resetButtonStates();
        
        /**
         * @brief Handles action button click events
         * @param id Unique identifier of the clicked button
         * 
         * Processes clicks for ready/unready toggle, leave lobby,
         * or other lobby interaction buttons.
         */
        void handleButtonClick(std::string const &id);
        
        /**
         * @brief Checks if the game can start based on current conditions
         * @return True if all players are ready and minimum requirements are met
         * 
         * Validates that enough players are connected and all players
         * have confirmed their ready status before allowing game start.
         */
        bool canJoin();
};

} // namespace scene


#endif //RTYPE_WAITINGSCENE_HPP