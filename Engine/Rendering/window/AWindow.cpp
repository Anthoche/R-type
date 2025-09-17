/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** AWindow
*/

#include <raylib.h>
#include "AWindow.hpp"

AWindow::AWindow(int const &width, int const &height, std::string const &title) {
	_width = width;
	_height = height;
	_title = title;
	_isOpen = false;
}

int AWindow::getWidth() const {
	return _width;
}

int AWindow::getHeight() const {
	return _height;
}

std::string const &AWindow::getTitle() const {
	return _title;
}

bool AWindow::isOpen() const {
	return _isOpen;
}

void AWindow::close() {
	_isOpen = false;
	onClose();
}
