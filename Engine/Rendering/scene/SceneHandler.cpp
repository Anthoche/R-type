/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowHandler
*/

#include "SceneHandler.hpp"

#include <iostream>
#include <ostream>

#include "../../../Game/Scene/MenuScene.hpp"
#include "../../../Game/Scene/GameScene.hpp"

SceneHandler::SceneHandler() {
	_raylib = Raylib();
	setupScenes();
}

void SceneHandler::addScene(const std::string &name, const std::shared_ptr<IScene> &scene) {
	_scenes.insert_or_assign(name, std::make_pair(scene, false));
}

void SceneHandler::setupScenes() {
	addScene("menu", std::make_shared<scene::MenuScene>(*this));
	addScene("game", std::make_shared<game::scene::GameScene>(*this));
}

void SceneHandler::openMenu() {
	_raylib.setResizableFlag(FLAG_WINDOW_RESIZABLE);
	_raylib.initWindow(100, 100, "R-Type");
	open("menu");
	_raylib.closeWindow();
}

void SceneHandler::open(const std::string &name) {
	if (_scenes.empty() || _scenes[name].first == nullptr)
		return;

	closeOpened();

	std::shared_ptr<IScene> scene = _scenes[name].first;

	int currentMonitor = _raylib.getCurrentMonitor();
	int posX = (_raylib.getMonitorWidth(currentMonitor) - scene->getWindowWidth()) / 2;
	int posY = (_raylib.getMonitorHeight(currentMonitor) - scene->getWindowHeight()) / 2;

	_raylib.setWindowSize(scene->getWindowWidth(), scene->getWindowHeight());
	_raylib.setWindowTitle(scene->getWindowTitle());
	_raylib.setWindowPosition(posX, posY);

	if (_raylib.isWindowFullscreen())
		_raylib.toggleFullscreen();

	scene->init();
	_scenes[name].second = true;

	while (scene->isOpen() && !_raylib.windowShouldClose()) {
		scene->handleEvents();
		scene->render();
	}

	scene->onClose();
	_scenes[name].second = false;
}

void SceneHandler::closeOpened() {
	for (auto &[name, pair]: _scenes) {
		if (pair.second) {
			pair.second = false;
			pair.first->close();
		}
	}
}

int SceneHandler::getSceneIndex(const std::string &name) const {
	int index = 0;
	for (const auto &[key, _]: _scenes) {
		if (key == name)
			return index;
		++index;
	}
	return -1;
}

std::shared_ptr<IScene> SceneHandler::getScene(std::string const &name) {
	auto it = _scenes.find(name);
	if (it != _scenes.end())
		return it->second.first;
	return nullptr;
}
