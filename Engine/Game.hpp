/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/
#pragma once
#include <string>
#include <thread>
#include "../Shared/GameStatus.hpp"
#include "../Client/client.hpp"
#include "../Game/Scene/Include/SceneHandler.hpp"

/**
 * @class Game
 * @brief Main client-side game class that orchestrates gameplay, networking, and scene management.
 * 
 * The Game class serves as the central controller for the R-Type client application.
 * It manages the game loop, handles networking through GameClient, controls scene transitions
 * via SceneHandler, and maintains game state including language settings, sound preferences,
 * and player customization options.
 */
class Game {
  public:
      /**
       * @brief Constructs a Game instance and initializes networking.
       *
       * Sets up the game client connection to the server, initializes the scene handler,
       * and prepares the networking thread for asynchronous communication.
       *
       * @param serverIp IP address of the game server to connect to.
       * @param serverPort Port number of the game server.
       * @param clientName Display name for this player/client.
       */
      Game(const std::string &serverIp, const std::string &serverPort, const std::string &clientName);
      
      /**
       * @brief Destructor for Game.
       *
       * Cleans up resources and ensures proper shutdown of networking threads.
       */
      ~Game() = default;
      
      /**
       * @brief Runs the main game loop.
       *
       * Starts the game's main loop which handles rendering, input processing,
       * scene updates, and network synchronization. This function blocks until
       * the game is closed.
       */
      void run();
      
      /**
       * @brief Gets the scene handler that manages scene transitions and rendering.
       *
       * @return Reference to the SceneHandler instance.
       */
      SceneHandler &getSceneHandler();
      
      /**
       * @brief Gets the game client that handles network communication.
       *
       * @return Reference to the GameClient instance.
       */
      GameClient &getGameClient();
      
      /**
       * @brief Gets the current game status.
       *
       * @return The current GameStatus (e.g., waiting, playing, finished).
       */
      GameStatus getGameStatus() const;
      
      /**
       * @brief Sets a new game status.
       *
       * Updates the game's current status, which affects game logic and scene behavior.
       *
       * @param status The new GameStatus to set.
       */
      void setGameStatus(const GameStatus &status);
      
      /**
       * @enum Language
       * @brief Supported languages for game localization.
       *
       * Defines the available language options for UI text and in-game messages.
       */
      enum class Language { 
          ENGLISH,  ///< English language.
          FRENCH,   ///< French language.
          ITALIAN   ///< Italian language.
      };
      
      /**
       * @brief Describes the result of the last finished match.
       */
      enum class MatchOutcome {
          None,    ///< No result yet or match still running.
          Victory, ///< Team cleared the campaign.
          Defeat   ///< All players have been defeated.
      };

      /**
       * @brief Sets the current game language.
       *
       * Changes the active language for all UI elements and text displays.
       *
       * @param lang The language to activate (ENGLISH, FRENCH, or ITALIAN).
       */
      void setLanguage(Language lang) { _language = lang; }
      
      /**
       * @brief Gets the current game language.
       *
       * @return The currently active language setting.
       */
      Language getLanguage() const { return _language; }
      
      /**
       * @brief Enables or disables game sound.
       *
       * @param enabled true to enable sound effects and music, false to mute.
       */
      void setSoundEnabled(bool enabled) { _soundEnabled = enabled; }
      
      /**
       * @brief Checks if game sound is currently enabled.
       *
       * @return true if sound is enabled, false if muted.
       */
      bool isSoundEnabled() const { return _soundEnabled; }

      /**
       * @brief Sets the selected player skin texture path.
       *
       * Updates the visual appearance of the local player character.
       *
       * @param path Absolute file path to the skin sprite texture.
       */
      void setSelectedSkinPath(const std::string &path) { _selectedSkinPath = path; }

      /**
       * @brief Gets the currently selected player skin texture path.
       *
       * @return Absolute path to the sprite used for the local player.
       */
      const std::string &getSelectedSkinPath() const { return _selectedSkinPath; }

      /**
       * @brief Sets the selected weapon texture path.
       *
       * Updates the visual appearance of the local player's weapon.
       *
       * @param path Absolute file path to the weapon sprite texture.
       */
      void setSelectedWeaponPath(const std::string &path) { _selectedWeaponPath = path; }

      /**
       * @brief Gets the currently selected weapon texture path.
       *
       * @return Absolute path to the sprite used for the local player's weapon.
       */
      const std::string &getSelectedWeaponPath() const { return _selectedWeaponPath; }

      /**
       * @brief Sets the selected weapon identifier.
       *
       * Associates the player's weapon with a specific game logic identifier.
       *
       * @param id Unique identifier string for the weapon type.
       */
      void setSelectedWeaponId(const std::string &id) { _selectedWeaponId = id; }

      /**
       * @brief Gets the identifier of the currently selected weapon.
       *
       * @return Identifier string for the active weapon.
       */
      const std::string &getSelectedWeaponId() const { return _selectedWeaponId; }

      /**
       * @brief Enables or disables endless game mode.
       *
       * @param endless true to enable endless mode, false for standard mode.
       */
      void setEndlessMode(bool endless) { _endlessMode = endless; }
    
      /**
       * @brief Checks if endless mode is enabled.
       *
       * @return true if endless mode is active, false otherwise.
       */
      bool isEndlessModeEnabled() const { return _endlessMode; }

      /**
       * @brief Stores the outcome of the most recently finished match.
       */
      void setLastMatchOutcome(MatchOutcome outcome) { _lastOutcome = outcome; }

      /**
       * @brief Returns the recorded outcome of the last finished match.
       */
      MatchOutcome getLastMatchOutcome() const { return _lastOutcome; }
      
      /**
       * @brief Sets the player's current health value.
       *
       * @param health Health value in hit points (25 HP = 1 life).
       */
      void setHealth(size_t health) { _health = health; }

      /**
       * @brief Gets the player's current health value.
       *
       * @return Current health in hit points.
       */
      size_t getHealth() const { return _health; }
      
      /**
       * @brief Calculates the player's current life index based on health.
       *
       * Converts raw health points to a discrete life count for display purposes.
       *
       * @return Life index (0 = dead, 1 = 1 life, 2 = 2 lives, etc.).
       */
      size_t getHealthIndex() const {
          if (_health <= 0) return 0;
          return (_health / 25) - 1;
      }

  private:
      GameStatus _status; ///< Current game status (waiting, playing, etc.).
      SceneHandler _sceneHandler; ///< Manages scene rendering and transitions.
      GameClient _client; ///< Handles network communication with the server.
      std::thread _networkThread; ///< Dedicated thread for asynchronous networking.
      Language _language = Language::ENGLISH; ///< Current language setting (default: English).
      bool _soundEnabled = true; ///< Whether sound effects and music are enabled.
      bool _endlessMode = false; ///< Whether endless mode is active.
      size_t _health = 75; ///< Current player health (25 HP = 1 life, 50 HP = 2 lives, etc.).
      std::string _selectedSkinPath; ///< File path to the selected player skin sprite.
      std::string _selectedWeaponPath; ///< File path to the selected weapon sprite.
      std::string _selectedWeaponId; ///< Identifier for the currently selected weapon type.
      MatchOutcome _lastOutcome = MatchOutcome::None; ///< Result of the last finished run.
};
