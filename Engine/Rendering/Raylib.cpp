/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Raylib
*/

#include "Raylib.hpp"
#include <raylib.h>
#include "rlgl.h"

// rcore
void Raylib::setResizableFlag(unsigned int flags) {
	SetConfigFlags(flags);
}

void Raylib::setExitKey(int const key) {
	SetExitKey(key);
}

void Raylib::initWindow(int width, int height, const std::string &title) {
	InitWindow(width, height, title.c_str());
}

void Raylib::closeWindow() {
	CloseWindow();
}

int Raylib::getCurrentMonitor() {
	return GetCurrentMonitor();
}

int Raylib::getMonitorWidth(int monitor) {
	return GetMonitorWidth(monitor);
}

int Raylib::getMonitorHeight(int monitor) {
	return GetMonitorHeight(monitor);
}

int Raylib::getMonitorRefreshRate(int monitor) {
	return GetMonitorRefreshRate(monitor);
}

void Raylib::setWindowSize(int width, int height) {
	SetWindowSize(width, height);
}

void Raylib::setWindowTitle(const std::string &title) {
	SetWindowTitle(title.c_str());
}

void Raylib::setWindowPosition(int x, int y) {
	SetWindowPosition(x, y);
}

void Raylib::setWindowMonitor(int monitor) {
	SetWindowMonitor(monitor);
}

bool Raylib::isFullscreen() {
	return IsWindowFullscreen();
}

bool Raylib::isWindowFocused() {
	return IsWindowFocused();
}

void Raylib::toggleFullscreen() {
	ToggleFullscreen();
}

bool Raylib::shouldClose() {
	return WindowShouldClose();
}

void Raylib::enableCursor() {
	EnableCursor();
}

void Raylib::disableCursor() {
	DisableCursor();
}

void Raylib::setTargetFPS(int fps) {
	SetTargetFPS(fps);
}

float Raylib::getFrameTime() {
	return GetFrameTime();
}

int Raylib::getFPS() {
	return GetFPS();
}

int Raylib::getRenderWidth() {
	return GetRenderWidth();
}

int Raylib::getRenderHeight() {
	return GetRenderHeight();
}

void Raylib::beginDrawing() {
	BeginDrawing();
}

void Raylib::endDrawing() {
	EndDrawing();
}

void Raylib::clearBackground(Color color) {
	ClearBackground(color);
}

int Raylib::getKeyPressed() {
	return GetKeyPressed();
}

bool Raylib::isWindowFullscreen() {
	return IsWindowFullscreen();
}

bool Raylib::windowShouldClose() {
	return WindowShouldClose();
}

void Raylib::beginMode3D(Camera3D camera) {
	BeginMode3D(camera);
}

void Raylib::endMode3D() {
	EndMode3D();
}

void Raylib::updateCamera(Camera *camera, int mode) {
	UpdateCamera(camera, mode);
}

double Raylib::getTime() {
	return GetTime();
}

// rshapes
void Raylib::drawRectangle(int posX, int posY, int width, int height, Color color) {
	DrawRectangle(posX, posY, width, height, color);
}

void Raylib::drawRectangleRec(Rectangle rec, Color color) {
	DrawRectangleRec(rec, color);
}

void Raylib::drawRectanglePro(Rectangle rec, Vector2 origin, float rotation, Color color) {
	DrawRectanglePro(rec, origin, rotation, color);
}

bool Raylib::checkCollisionPointRec(Vector2 point, Rectangle rec) {
	return CheckCollisionPointRec(point, rec);
}

void Raylib::drawRectangleLines(int posX, int posY, int width, int height, Color color) {
	DrawRectangleLines(posX, posY, width, height, color);
}

void Raylib::drawRectangleRounded(Rectangle rec, float roundness, int segments, Color color) {
	DrawRectangleRounded(rec, roundness, segments, color);
}

void Raylib::drawLine(int startX, int startY, int endX, int endY, Color color) {
	DrawLine(startX, startY, endX, endY, color);
}

void Raylib::drawCircle(int centerX, int centerY, float radius, Color color) {
	DrawCircle(centerX, centerY, radius, color);
}

void Raylib::drawPlane(Vector3 centerPos, Vector2 size, Color color) {
	DrawPlane(centerPos, size, color);
}

// rtext

Font Raylib::loadFont(std::string const &fileName) {
	return LoadFont(fileName.c_str());
}

void Raylib::unloadFont(Font font) {
	UnloadFont(font);
}

int Raylib::measureText(std::string const &text, int fontSize) {
	return MeasureText(text.c_str(), fontSize);
}

Vector2 Raylib::measureTextEx(Font font, std::string const &text, float fontSize, float spacing) {
	return MeasureTextEx(font, text.c_str(), fontSize, spacing);
}

void Raylib::drawText(std::string const &text, int posX, int posY, int fontSize, Color color) {
	DrawText(text.c_str(), posX, posY, fontSize, color);
}

void Raylib::drawTextEx(Font font, std::string const &text, Vector2 position, float fontSize, float spacing, Color tint) {
	DrawTextEx(font, text.c_str(), position, fontSize, spacing, tint);
}

void Raylib::imageDrawText(Image *dst, std::string const &text, int posX, int posY, int fontSize, Color color) {
	ImageDrawText(dst, text.c_str(), posX, posY, fontSize, color);
}

Color Raylib::fade(Color color, float alpha) {
	return Fade(color, alpha);
}

std::string Raylib::textFormat(std::string format, ...) {
	//TODO: Do something
	return format;
}

// rmodels
void Raylib::drawModel(Model model, Vector3 position, float scale, Color tint) {
	DrawModel(model, position, scale, tint);
}

void Raylib::drawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint) {
	DrawModelEx(model, position, rotationAxis, rotationAngle, scale, tint);
}

void Raylib::drawCube(Vector3 position, float width, float height, float length, Color color) {
	DrawCube(position, width, height, length, color);
}

void Raylib::drawCubeWires(Vector3 position, float width, float height, float length, Color color) {
	DrawCubeWires(position, width, height, length, color);
}

void Raylib::drawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color) {
	DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
}

void Raylib::drawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color) {
	DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
}

Model Raylib::loadModel(std::string const &fileName) {
	return LoadModel(fileName.c_str());
}

void Raylib::unloadModel(Model const &model) {
	UnloadModel(model);
}

Music Raylib::loadMusicStream(std::string const &fileName) {
	return LoadMusicStream(fileName.c_str());
}

void Raylib::initAudioDevice() {
	InitAudioDevice();
}

void Raylib::playMusicStream(Music const &music) {
	PlayMusicStream(music);
}

void Raylib::updateMusicStream(Music const &music) {
	UpdateMusicStream(music);
}

void Raylib::stopMusicStream(Music const &music) {
	StopMusicStream(music);
}

void Raylib::unloadMusicStream(Music const &music) {
	UnloadMusicStream(music);
}

void Raylib::setMusicVolume(Music const &music, float volume) {
	SetMusicVolume(music, volume);
}

Shader Raylib::loadShader(std::string const &vsFileName, std::string const &fsFileName) {
	return LoadShader(vsFileName.c_str(), fsFileName.c_str());
}

void Raylib::unloadShader(Shader const &shader) {
	UnloadShader(shader);
}

void Raylib::setShaderValue(Shader const &shader, int locIndex, const Vector4 &value, int uniformType) {
	SetShaderValue(shader, locIndex, &value, uniformType);
}

void Raylib::setShaderValue(Shader const &shader, int locIndex, float value, int uniformType) {
	SetShaderValue(shader, locIndex, &value, uniformType);
}

void Raylib::setShaderValue(Shader const &shader, int locIndex, int value, int uniformType) {
	SetShaderValue(shader, locIndex, &value, uniformType);
}

int Raylib::getShaderLocation(Shader const &shader, std::string const &uniformName) {
	return GetShaderLocation(shader, uniformName.c_str());
}

void Raylib::closeAudioDevice() {
	CloseAudioDevice();
}

Vector2 Raylib::getMousePosition() {
	return GetMousePosition();
}

bool Raylib::isMouseButtonDown(int button) {
	return IsMouseButtonDown(button);
}

bool Raylib::isMouseButtonReleased(int button) {
	return IsMouseButtonReleased(button);
}

// Keyboard input

bool Raylib::isKeyDown(int key) {
	return IsKeyDown(key);
}

bool Raylib::isKeyPressed(int key) {
	return IsKeyPressed(key);
}

bool Raylib::isKeyReleased(int key) {
	return IsKeyReleased(key);
}

// RLGL

void Raylib::pushMatrix() {
	rlPushMatrix();
}

void Raylib::translatef(float x, float y, float z) {
	rlTranslatef(x, y, z);
}

void Raylib::rotatef(float angle, float x, float y, float z) {
	rlRotatef(angle, x, y, z);
}

void Raylib::popMatrix() {
	rlPopMatrix();
}


//rtextures
Texture2D Raylib::loadTexture(const std::string &fileName) {
    return LoadTexture(fileName.c_str());
}

void Raylib::unloadTexture(Texture2D texture) {
    UnloadTexture(texture);
}

void Raylib::drawTexture(Texture2D texture, int posX, int posY, Color tint) {
    DrawTexture(texture, posX, posY, tint);
}

void Raylib::drawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint) {
    DrawTextureEx(texture, position, rotation, scale, tint);
}
