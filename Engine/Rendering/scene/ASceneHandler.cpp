/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ASceneHandler
*/

#include "Include/ASceneHandler.hpp"

void ASceneHandler::addScene(const std::string &name, const std::shared_ptr<IScene> &scene) {
    _scenes.insert_or_assign(name, std::make_pair(scene, false));
}

std::shared_ptr<IScene> ASceneHandler::getScene(const std::string &name) {
    auto it = _scenes.find(name);
    if (it != _scenes.end())
        return it->second.first;
    return nullptr;
}

void ASceneHandler::closeOpened() {
    for (auto &[_, pair] : _scenes) {
        if (pair.second) {
            pair.second = false;
            pair.first->close();
        }
    }
}
