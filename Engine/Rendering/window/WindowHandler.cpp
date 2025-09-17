/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WindowHandler
*/

#include "WindowHandler.hpp"
#include <iostream>

WindowHandler::WindowHandler() {
	_raylib = Raylib();
	setupWindows();
}

void WindowHandler::addWindow(const std::string &name, const std::shared_ptr<IWindow> &window) {
	_windows.insert_or_assign(name, std::make_pair(window, false));
}

void WindowHandler::setupWindows() {
	// Setup windows here
}

void WindowHandler::openMenu() {
	_raylib.setResizableFlag(FLAG_WINDOW_RESIZABLE);
	_raylib.initWindow(100, 100, "R-Type");
	open("menu");
	_raylib.closeWindow();
}

void WindowHandler::open(const std::string &name) {
	if (_windows.empty() || _windows[name].first == nullptr)
		return;

	closeOpened();

	std::shared_ptr<IWindow> window = _windows[name].first;

	int currentMonitor = _raylib.getCurrentMonitor();
	int posX = (_raylib.getMonitorWidth(currentMonitor) - window->getWidth()) / 2;
	int posY = (_raylib.getMonitorHeight(currentMonitor) - window->getHeight()) / 2;

	_raylib.setWindowSize(window->getWidth(), window->getHeight());
	_raylib.setWindowTitle(window->getTitle());
	_raylib.setWindowPosition(posX, posY);

	if (IsWindowFullscreen())
		_raylib.toggleFullscreen();

	window->init();
	_windows[name].second = true;

	while (window->isOpen() && !WindowShouldClose()) {
		window->handleEvents();
		window->render();

		//Handle user inputs here
	}

	window->onClose();
	_windows[name].second = false;
}

void WindowHandler::closeOpened() {
	for (auto &[name, pair]: _windows) {
		if (pair.second) {
			pair.second = false;
			pair.first->close();
		}
	}
}

int WindowHandler::getWindowIndex(const std::string &name) const {
	int index = 0;
	for (const auto &[key, _]: _windows) {
		if (key == name)
			return index;
		++index;
	}
	return -1;
}

std::shared_ptr<IWindow> WindowHandler::getWindow(std::string const &name) {
	auto it = _windows.find(name);
	if (it != _windows.end())
		return it->second.first;
	return nullptr;
}
