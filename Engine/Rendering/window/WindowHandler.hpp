/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowHandler
*/

#ifndef WINDOWHANDLER_HPP
    #define WINDOWHANDLER_HPP

    #include <map>
    #include <memory>
    #include "IWindow.hpp"
    #include "../Raylib.hpp"

/**
 * @class WindowHandler
 * @brief Manages multiple windows within the GUI.
 *
 * This class handles the creation, opening, and closing of windows,
 * maintaining a map of window names to their objects and visibility states.
 */
class WindowHandler {
    private:
        /**
         * @brief Raylib wrapper instance
         */
        Raylib _raylib; // Temporaire

        /**
         * @brief Map associating window names to pairs of window objects and their open state.
         *
         * The bool indicates whether the window is currently open (true) or closed (false).
         */
        std::map<std::string, std::pair<std::shared_ptr<IWindow>, bool>> _windows;

        /**
         * @brief Adds a window to the internal window map.
         *
         * @param name The name of the window.
         * @param window Shared pointer to the window instance.
         */
        void addWindow(std::string const &name, std::shared_ptr<IWindow> const &window);

        /**
         * @brief Closes all currently opened windows.
         */
        void closeOpened();

        /**
         * @brief Initializes and sets up the windows.
         */
        void setupWindows();

        /**
         * @brief Gets the index of a window by name.
         *
         * @param name The name of the window.
         * @return The index of the window, or -1 if not found.
         */
        int getWindowIndex(std::string const &name) const;

    public:
        /**
         * @brief Constructs a new WindowHandler object.
         */
        WindowHandler();

        /**
         * @brief Default destructor.
         */
        ~WindowHandler() = default;

        /**
         * @brief Opens a window by name.
         *
         * @param name The name of the window to open.
         */
        void open(std::string const &name);

        /**
         * @brief Opens the main menu window.
         */
        void openMenu();

        /**
         * @brief Gets the current window.
         *
         * @return The currently opened window.
         */
        std::shared_ptr<IWindow> getWindow(std::string const &name);

};

#endif //WINDOWHANDLER_HPP
