/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ServerWaitScene
*/

#ifndef RTYPE_SERVERWAITSCENE_HPP
#define RTYPE_SERVERWAITSCENE_HPP

#include "../../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../../Engine/Game.hpp"

namespace scene {

/**
 * @class ServerWaitScene
 * @brief Represents the waiting scene for server connection and player join.
 *
 * This scene handles:
 * - Waiting for the server to be ready.
 * - Waiting for player connections.
 * - Managing UI and timeouts.
 */
class ServerWaitScene : public AScene {
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
         * @brief Initialize the waiting scene (UI, resources, timers).
         */
        void init() override;

        /**
         * @brief Render the waiting scene (draw UI elements).
         */
        void render() override;

        /**
         * @brief Handle input and events in the waiting scene.
         */
        void handleEvents() override;

        /**
         * @brief Called when the waiting scene is closed (cleanup).
         */
        void onClose() override;

    private:
        Game &_game; ///< Reference to the game instance.
        Font _font{}; ///< Font used in the scene.

        double _startTime; ///< Time when the waiting started.
        double _endTime; ///< Time limit before timeout.
        int _attemptsCount; ///< Number of retry attempts.

        // --- Private methods ---

        /**
         * @brief Calculate the remaining time before timeout.
         * @return Remaining time in seconds.
         */
        double getRemainingTime();

        /**
         * @brief Reset the waiting timer and attempt counter.
         */
        void resetTimer();

        /**
         * @brief Reset the states of all UI buttons.
         */
        void resetButtonStates();

        /**
         * @brief Handle a button click by its ID.
         * @param id ID of the clicked button.
         */
        void handleButtonClick(std::string const &id);
};

} // namespace scene

#endif // RTYPE_SERVERWAITSCENE_HPP
