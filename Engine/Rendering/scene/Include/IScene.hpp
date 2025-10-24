/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** IScene
*/

#pragma once

#include <string>

/**
 * @class IScene
 * @brief Interface for scene objects.
 *
 * Defines the essential methods every scene class should implement,
 * including initialization, rendering, event handling, and lifecycle management.
 */
class IScene {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IScene() = default;

        /**
         * @brief Initializes the window.
         */
        virtual void init() = 0;

        /**
         * @brief Renders the window contents.
         */
        virtual void render() = 0;

        /**
         * @brief Handles user or system events for the window.
         */
        virtual void handleEvents() = 0;

        /**
         * @brief Initiates window closing procedure.
         */
        virtual void close() = 0;

        /**
         * @brief Called after the window has closed for cleanup or additional logic.
         */
        virtual void onClose() = 0;

        /**
         * @brief Returns the window's width.
         * 
         * @return The width in pixels.
         */
        virtual int getWindowWidth() const = 0;

        /**
         * @brief Returns the window's height.
         * 
         * @return The height in pixels.
         */
        virtual int getWindowHeight() const = 0;

        /**
         * @brief Returns the window's title.
         * 
         * @return The window title as a const reference to a string.
         */
        virtual std::string const &getWindowTitle() const = 0;

        /**
         * @brief Checks if the window is currently open.
         * 
         * @return true if the window is open, false otherwise.
         */
        virtual bool isOpen() const = 0;
};
