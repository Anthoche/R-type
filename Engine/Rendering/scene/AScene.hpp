/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AScene
*/

#ifndef AWINDOW_HPP
    #define AWINDOW_HPP

    #include "IScene.hpp"

/**
 * @class AScene
 * @brief Abstract base class implementing common window functionality.
 *
 * This class partially implements the IWindow interface,
 * providing common properties such as width, height, title, and open state,
 * while leaving init, render, and handleEvents as pure virtual methods
 * for derived classes to implement.
 */
class AScene : public IScene {
    protected:
        /**
         * @brief The width of the window.
         */
        int _width;

        /**
         * @brief The height of the window.
         */
        int _height;

        /**
         * @brief The title of the window.
         */
        std::string _title;

        /**
         * @brief Flag indicating if the window is open.
         */
        bool _isOpen;

    public:
        /**
         * @brief Constructs an AWindow with given dimensions and title.
         * 
         * @param width The window width in pixels.
         * @param height The window height in pixels.
         * @param title The window title string.
         */
        AScene(int const &width, int const &height, std::string const &title);

        /**
         * @brief Default destructor.
         */
        ~AScene() override = default;

        /**
         * @brief Initializes the window.
         * Must be implemented by derived classes.
         */
        virtual void init() = 0;

        /**
         * @brief Renders the window content.
         * Must be implemented by derived classes.
         */
        virtual void render() = 0;

        /**
         * @brief Handles events related to the window.
         * Must be implemented by derived classes.
         */
        virtual void handleEvents() = 0;

        /**
         * @brief Closes the window and updates the open state.
         */
        void close() override;

        /**
         * @brief Gets the window width.
         * 
         * @return The width in pixels.
         */
        int getWindowWidth() const override;

        /**
         * @brief Gets the window height.
         * 
         * @return The height in pixels.
         */
        int getWindowHeight() const override;

        /**
         * @brief Gets the window title.
         * 
         * @return The title string.
         */
        std::string const &getWindowTitle() const override;

        /**
         * @brief Checks if the window is open.
         * 
         * @return true if open, false otherwise.
         */
        bool isOpen() const override;
};

#endif //AWINDOW_HPP
