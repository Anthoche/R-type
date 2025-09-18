/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ViewMode
*/

/**
 * @file ViewMode.hpp
 * @brief Defines the ViewMode enumeration used to control the camera view mode
 */

#ifndef VIEWMODE_HPP
#define VIEWMODE_HPP

/**
 * @enum ViewMode
 * @brief Represents the available camera view modes.
 *
 * Used to switch between different rendering perspectives in the 3D scene.
 */
enum ViewMode {
    PERSPECTIVE, /**< 3D perspective view mode. */
    FLAT         /**< 2D flat (orthographic) view mode. */
};

#endif //VIEWMODE_HPP
