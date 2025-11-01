/**
 * @file ServerWaitScene.hpp
 * @brief Server connection waiting scene for multiplayer synchronization
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#ifndef RTYPE_SERVERWAITSCENE_HPP
	#define RTYPE_SERVERWAITSCENE_HPP

    #include "../../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../../../Engine/Game.hpp"

namespace scene {

/**
 * @class ServerWaitScene
 * @brief Waiting scene displayed while connecting to server or awaiting players
 *
 * Manages the waiting state during multiplayer setup including:
 * - Server connection attempts with timeout handling
 * - Player synchronization and ready state monitoring
 * - Retry mechanism with attempt counting
 * - Visual feedback and timeout display
 * - User cancellation and navigation options
 */
class ServerWaitScene: public AScene {
    public:
        /**
         * @brief Constructs the server waiting scene
         * @param game Reference to the main game instance
         */
        ServerWaitScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~ServerWaitScene() override = default;

        // --- Overridden lifecycle methods ---

        /**
         * @brief Initializes waiting scene UI and connection timer
         * 
         * Sets up fonts, initializes connection attempt counter,
         * and starts the timeout timer.
         */
        void init() override;

        /**
         * @brief Renders the waiting interface with timer and status
         * 
         * Draws loading indicators, remaining time, connection status,
         * and cancel/retry buttons.
         */
        void render() override;

        /**
         * @brief Processes user input and connection state changes
         * 
         * Handles cancel button clicks, checks for timeout conditions,
         * and monitors server response.
         */
        void handleEvents() override;

        /**
         * @brief Cleans up resources when the scene closes
         * 
         * Releases fonts, cancels pending connections, and performs cleanup.
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the main game instance

        Font _font{}; ///< Font used for text rendering

		double _startTime;      ///< Timestamp when the connection attempt started
		double _endTime;        ///< Timestamp when timeout will occur
		int _attemptsCount;     ///< Current number of connection retry attempts

		/**
		 * @brief Calculates remaining time before connection timeout
		 * @return Remaining time in seconds (0 if timeout reached)
		 * 
		 * Computes the difference between current time and timeout deadline,
		 * used for displaying countdown and triggering retry logic.
		 */
		double getRemainingTime();

		/**
		 * @brief Resets the connection timer and attempt counter
		 * 
		 * Called when retrying connection or starting a new connection attempt.
		 * Resets both the timer and increments the attempt counter.
		 */
		void resetTimer();

		/**
		 * @brief Resets all button interaction states to default
		 * 
		 * Clears hover and click states for all interactive buttons,
		 * typically called at the start of event processing.
		 */
        void resetButtonStates();
        
		/**
		 * @brief Handles action button click events
		 * @param id Unique identifier of the clicked button
		 * 
		 * Executes appropriate actions for buttons such as cancel,
		 * retry connection, or return to previous scene.
		 */
        void handleButtonClick(std::string const &id);
};

} // namespace scene


#endif //RTYPE_SERVERWAITSCENE_HPP