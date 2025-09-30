/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AScene
*/

#include <raylib.h>
#include "AScene.hpp"

AScene::AScene(int const &width, int const &height, std::string const &title) {
	_width = width;
	_height = height;
	_title = title;
	_isOpen = false;
}

int AScene::getWindowWidth() const {
	return _width;
}

int AScene::getWindowHeight() const {
	return _height;
}

std::string const &AScene::getWindowTitle() const {
	return _title;
}

bool AScene::isOpen() const {
	return _isOpen;
}

void AScene::close() {
	_isOpen = false;
}
