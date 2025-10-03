/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerWaitScene
*/

#ifndef RTYPE_SERVERWAITSCENE_HPP
	#define RTYPE_SERVERWAITSCENE_HPP

    #include "../../Rendering/scene/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

/**
 * @class ServerWaitScene
 * @brief Represents the waiting scene of the game.
 *
 * The waiting scene handles:
 * - Server await
 * - Player connections await
 */
class ServerWaitScene: public AScene {
    public:
        /**
         * @brief Construct a ServerWaitScene with a reference to the game.
         * @param game The game instance.
         */
        ServerWaitScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~ServerWaitScene() override = default;

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

		double _startTime; ///< Time when the waiting started.
		double _endTime; ///< Time limit before timeout.
		int _attemptsCount;  ///< Number of retry attempts.

		/**
		* @brief Calculate how much time is remaining before timeout.
		* @return Remaining time in seconds.
		*/
		double getRemainingTime();

		/**
		* @brief Reset the waiting timer and attempt counter.
		*/
		void resetTimer();

        void resetButtonStates();
        void handleButtonClick(std::string const &id);
};

} // namespace scene


#endif //RTYPE_SERVERWAITSCENE_HPP
