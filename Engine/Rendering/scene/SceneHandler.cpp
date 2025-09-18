/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowHandler
*/

#include "SceneHandler.hpp"
#include <iostream>

SceneHandler::SceneHandler() {
	_raylib = Raylib();
	setupScenes();
}

void SceneHandler::addScene(const std::string &name, const std::shared_ptr<IScene> &window) {
	_scenes.insert_or_assign(name, std::make_pair(window, false));
}

void SceneHandler::setupScenes() {
	// Setup windows here
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

	std::shared_ptr<IScene> window = _scenes[name].first;

	int currentMonitor = _raylib.getCurrentMonitor();
	int posX = (_raylib.getMonitorWidth(currentMonitor) - window->getWindowWidth()) / 2;
	int posY = (_raylib.getMonitorHeight(currentMonitor) - window->getWindowHeight()) / 2;

	_raylib.setWindowSize(window->getWindowWidth(), window->getWindowHeight());
	_raylib.setWindowTitle(window->getWindowTitle());
	_raylib.setWindowPosition(posX, posY);

	if (IsWindowFullscreen())
		_raylib.toggleFullscreen();

	window->init();
	_scenes[name].second = true;

	while (window->isOpen() && !WindowShouldClose()) {
		window->handleEvents();
		window->render();

		//Handle user inputs here
	}

	window->onClose();
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
