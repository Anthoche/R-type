/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** WaitingScene
*/

#ifndef RTYPE_WAITINGSCENE_HPP
	#define RTYPE_WAITINGSCENE_HPP

    #include "../../Rendering/scene/AScene.hpp"
    #include "../Game.hpp"

namespace scene {

/**
 * @class WaitingScene
 * @brief Represents the waiting scene of the game.
 *
 * The waiting scene handles:
 * - Server await
 * - Player connections await
 */
class WaitingScene: public AScene {
    public:
        /**
         * @brief Construct a WaitingScene with a reference to the game.
         * @param game The game instance.
         */
        WaitingScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~WaitingScene() override = default;

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

        void resetButtonStates();
        void handleButtonClick(std::string const &id);
        void drawButton(Vector2 position, Vector2 size, std::string const &content, int fontSize, float spacing, Color color, Color textColor, bool isHovered, bool isClicked, bool isEnabled);
};

} // namespace scene


#endif //RTYPE_WAITINGSCENE_HPP
