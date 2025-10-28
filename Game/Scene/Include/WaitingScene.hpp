/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** WaitingScene
*/

#ifndef RTYPE_WAITINGSCENE_HPP
	#define RTYPE_WAITINGSCENE_HPP

    #include "../../../Engine/Rendering/scene/Include/AScene.hpp"
    #include "../Game.hpp"
    #include <vector>
    #include <string>

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

        struct SkinOption {
            std::string name;
            std::string path;
            std::string filename;
            Texture2D texture{};
            Rectangle source{0.f, 0.f, 0.f, 0.f};
        };

        struct WeaponOption {
            std::string name;
            std::string path;
            Texture2D texture{};
            Rectangle source{0.f, 0.f, 0.f, 0.f};
        };

        std::vector<SkinOption> _skinOptions;
        std::size_t _currentSkinIndex{0};
        std::vector<WeaponOption> _weaponOptions;
        std::size_t _currentWeaponIndex{0};
        Vector2 _previewCenter{0.f, 0.f};
        Vector2 _previewBounds{220.f, 140.f};
        Vector2 _weaponPreviewCenter{0.f, 0.f};
        Vector2 _weaponPreviewBounds{120.f, 70.f};
        bool _ignoreInitialClick{false};

        void resetButtonStates();
        void handleButtonClick(std::string const &id);
        void loadSkinOptions();
        void loadWeaponOptions();
        void unloadSkinTextures();
        void unloadWeaponTextures();
        void selectSkin(std::size_t index);
        void selectNextSkin();
        void selectPreviousSkin();
        void selectWeapon(std::size_t index);
        void selectNextWeapon();
        void selectPreviousWeapon();
        void drawSkinPreview();
        void drawWeaponPreview();
        bool canJoin();
};

} // namespace scene


#endif //RTYPE_WAITINGSCENE_HPP
