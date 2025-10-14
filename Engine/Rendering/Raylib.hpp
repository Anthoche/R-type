/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Raylib wrapper class
*/

#ifndef RAYLIB_HPP
#define RAYLIB_HPP

#include <string>
#include <raylib.h>

// Type alias pour Rectangle de raylib
using RaylibRectangle = Rectangle;

/**
 * @class Raylib
 * @brief A C++ wrapper around selected functions from the raylib library.
 * 
 * This class provides a simplified interface for core raylib functionalities, 
 * including window management, drawing shapes and text, handling models
 * and input from mouse and keyboard.
 */
class Raylib {
    public:
        // === rcore module ===

        /**
         * @brief Set window configuration flags before initializing the window.
         * @param flags Configuration flags (e.g., FLAG_WINDOW_RESIZABLE).
         */
        void setResizableFlag(unsigned int flags);

		/**
		 * Sets key to exit the window
		 *
		 * @param key Key to set as an exit key
		 * Set to -1 to disable
		 */
		void setExitKey(int const key);

        /**
         * @brief Initialize window with specified dimensions and title.
         * @param width Window width.
         * @param height Window height.
         * @param title Window title.
         */
        void initWindow(int width, int height, const std::string &title);

        /**
         * @brief Close and unload the window.
         */
        void closeWindow();

        /**
         * @brief Check if the window should close.
         * @return True if the window should close.
         */
        bool shouldClose();

        /**
         * @brief Enable the mouse cursor.
         */
        void enableCursor();

		/**
		* @brief Disable the mouse cursor.
		*/
		void disableCursor();

        /**
         * @brief Set the target frames per second (FPS).
         * @param fps Frames per second.
         */
        void setTargetFPS(int fps);

		/**
		 * @brief Get time in seconds for last frame drawn (delta time)
		 * @return Time since last frame
		 */
		float getFrameTime();

		/**
		 * @brief Get the current FPS
		 * @return Current FPS
		 */
		int getFPS();

        /**
         * @brief Get the elapsed time since the program started.
         * @return Time in seconds.
         */
        double getTime();

        /**
         * @brief Get the key that was pressed.
         * @return Key code.
         */
        int getKeyPressed();

        /**
         * @brief Begin drawing frame.
         */
        void beginDrawing();

        /**
         * @brief End drawing frame.
         */
        void endDrawing();

        /**
         * @brief Clear the background with a specific color.
         * @param color Background color.
         */
        void clearBackground(Color color);

        /**
         * @brief Set the window title.
         * @param title New window title.
         */
        void setWindowTitle(const std::string &title);

        /**
         * @brief Set the window size.
         * @param width New width.
         * @param height New height.
         */
        void setWindowSize(int width, int height);

        /**
         * @brief Set the window position on screen.
         * @param x X coordinate.
         * @param y Y coordinate.
         */
        void setWindowPosition(int x, int y);

		/**
		 * Set window on specific monitor
		 * @param monitor The monitor to put the window on
		 */
		void setWindowMonitor(int monitor);

        /**
         * @brief Check if fullscreen mode is enabled.
         * @return True if fullscreen.
         */
        bool isFullscreen();

        /**
         * @brief Check if window is in fullscreen mode.
         * @return True if window is fullscreen.
         */
        bool isWindowFullscreen();

		/**
		 * @brief Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
		 * @return True if window should close.
		 */
		bool windowShouldClose();

		/**
		 * Checks if window is focused or not
		 * @return True if window is focused, false otherwise
		 */
		bool isWindowFocused();

        /**
         * @brief Toggle between fullscreen and windowed mode.
         */
        void toggleFullscreen();

        /**
         * @brief Get the currently active monitor index.
         * @return Monitor index.
         */
        int getCurrentMonitor();

        /**
         * @brief Get the width of a specified monitor.
         * @param monitor Monitor index.
         * @return Width in pixels.
         */
        int getMonitorWidth(int monitor);

        /**
         * @brief Get the height of a specified monitor.
         * @param monitor Monitor index.
         * @return Height in pixels.
         */
        int getMonitorHeight(int monitor);

		/**
		 * Get the refresh rate of a specified monitor
		 * @param monitor Monitor index
		 * @return The monitor's refresh rate
		 */
		int getMonitorRefreshRate(int monitor);

        /**
         * @brief Get the current render width.
         * @return Render width.
         */
        int getRenderWidth();

        /**
         * @brief Get the current render height.
         * @return Render height.
         */
        int getRenderHeight();

        /**
         * @brief Begin 3D mode with a specified camera.
         * @param camera Camera3D object.
         */
        void beginMode3D(Camera3D camera);

        /**
         * @brief End 3D mode.
         */
        void endMode3D();

        /**
         * @brief Update a camera with a given mode.
         * @param camera Pointer to Camera object.
         * @param mode Update mode.
         */
        void updateCamera(Camera *camera, int mode);

        // === rshapes module ===

		/**
		 * @brief Draws a rectangle
		 * @param posX Pos at x axis
		 * @param posY Pos at y axis
		 * @param width width of the rectangle
		 * @param height height of the rectangle
		 * @param color color of the rectangle
		 */
		void drawRectangle(int posX, int posY, int width, int height, Color color);

		/**
		 * Draws a rectangle on screen
		 * @param rec The rectangle to draw
		 * @param color The color of the rectangle
		 */
		void drawRectangleRec(RaylibRectangle rec, Color color);

        /**
         * @brief Draw a rotated rectangle using pro parameters.
         * @param rec Rectangle to draw.
         * @param origin Origin point for rotation.
         * @param rotation Rotation angle.
         * @param color Rectangle color.
         */
        void drawRectanglePro(RaylibRectangle rec, Vector2 origin, float rotation, Color color);

        /**
         * @brief Draw the outline of a rectangle.
         * @param posX X position.
         * @param posY Y position.
         * @param width Width of the rectangle.
         * @param height Height of the rectangle.
         * @param color Outline color.
         */
        void drawRectangleLines(int posX, int posY, int width, int height, Color color);

        /**
         * @brief Check if a point collides with a rectangle.
         * @param point Point position.
         * @param rec Rectangle to test against.
         * @return True if collision detected.
         */
        bool checkCollisionPointRec(Vector2 point, RaylibRectangle rec);

		/**
		 * @brief Draw rectangle with rounded edges
		 * @param rec The rectangle to draw
		 * @param roundness The rect roundness (0 to 1)
		 * @param segments ??
		 * @param color The rectangle fill color
		 */
		void drawRectangleRounded(Rectangle rec, float roundness, int segments, Color color);

        /**
         * @brief Draw a line between two points.
         * @param startX Start X.
         * @param startY Start Y.
         * @param endX End X.
         * @param endY End Y.
         * @param color Line color.
         */
        void drawLine(int startX, int startY, int endX, int endY, Color color);

        /**
         * @brief Draw a circle.
         * @param centerX Center X.
         * @param centerY Center Y.
         * @param radius Circle radius.
         * @param color Circle color.
         */
        void drawCircle(int centerX, int centerY, float radius, Color color);

		/**
		 * @brief Draws a plane.
		 * @param centerPos The center of the plane.
		 * @param size The size of the plane.
		 * @param color The color of the plane.
		 */
		void drawPlane(Vector3 centerPos, Vector2 size, Color color);

        // === rtext module ===

		/**
		 * @brief Load font from file into GPU memory (VRAM)
		 * @param fileName The file path of the font file
		 * @return The loaded font
		 */
		Font loadFont(std::string const &fileName);

		/**
		 * @brief Unload font from GPU memory (VRAM)
		 * @param font The font to unload
		 */
		void unloadFont(Font font);

        /**
         * @brief Measure the width of text with given font size.
         * @param text Text string.
         * @param fontSize Font size.
         * @return Text width in pixels.
         */
        int measureText(std::string const &text, int fontSize);

		/**
		 * @brief Measure string size for a specific Font
		 * @param font The font used for the text
		 * @param text The text to measure
		 * @param fontSize The size of the text font
		 * @param spacing The spacing between letters in the text
		 * @return Vector {width, height} of the measured text
		 */
		Vector2 measureTextEx(Font font, std::string const &text, float fontSize, float spacing);

        /**
         * @brief Draw text on screen.
         * @param text Text string.
         * @param posX X position.
         * @param posY Y position.
         * @param fontSize Font size.
         * @param color Text color.
         */
        void drawText(std::string const &text, int posX, int posY, int fontSize, Color color);

		/**
		 * @brief Draw text using font and additional parameters
		 * @param font The font to be used to draw the text
		 * @param text The text to draw
		 * @param position The position of the text
		 * @param fontSize The font size of the text
		 * @param spacing The spacing between letters
		 * @param tint The text color
		 */
		void drawTextEx(Font font, std::string const &text, Vector2 position, float fontSize, float spacing, Color tint);

        /**
         * @brief Format text with variable arguments (like printf).
         * @param format Format string.
         * @param ... variadic arguments
         * @return Formatted string.
         */
        std::string textFormat(std::string format, ...);

        // === rtextures module ===

        /**
         * @brief Draw text directly onto an image.
         * @param dst Target image.
         * @param text Text to draw.
         * @param posX X position.
         * @param posY Y position.
         * @param fontSize Font size.
         * @param color Text color.
         */
        void imageDrawText(Image *dst, std::string const &text, int posX, int posY, int fontSize, Color color);

        /**
         * @brief Apply alpha fading to a color.
         * @param color Base color.
         * @param alpha Alpha value (0.0 to 1.0).
         * @return Faded color.
         */
        Color fade(Color color, float alpha);

        // === rmodels module ===

        /**
         * @brief Draw a 3D model in the scene.
         * @param model The model to draw.
         * @param position Position to place the model.
         * @param scale Model scale.
         * @param tint Tint color.
         */
        void drawModel(Model model, Vector3 position, float scale, Color tint);

		/**
		 * @brief Draw a 3D model in the scene with extended parameters
		 * @param model The model to draw.
		 * @param position Position to place the model.
		 * @param rotationAxis The axis on the rotation angle will take effect.
		 * @param rotationAngle The angle to rotate the model.
		 * @param scale Model scale.
		 * @param tint Tint color.
		 */
		void drawModelEx(Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale, Color tint);

		/**
		 * Draws a cube
		 * @param position The position of the cube
		 * @param width The cube width
		 * @param height The cube height
		 * @param length The cube length
		 * @param color The cube color
		 */
		void drawCube(Vector3 position, float width, float height, float length, Color color);

		/**
		 * @brief Draws a cube's wires
		 * @param position Cylinder position.
		 * @param width Cylinder width.
		 * @param height Cylinder height.
		 * @param length Cylinder length.
		 * @param color Wires colors.
		 */
		void drawCubeWires(Vector3 position, float width, float height, float length, Color color);

        /**
         * @brief Draw a 3D cylinder.
         * @param position Cylinder base position.
         * @param radiusTop Top radius.
         * @param radiusBottom Bottom radius.
         * @param height Cylinder height.
         * @param slices Number of slices.
         * @param color Cylinder color.
         */
        void drawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color);

		/**
		 * @brief Draw a 3D cylinder wires.
		 * @param position Cylinder base position.
		 * @param radiusTop Top radius.
		 * @param radiusBottom Bottom radius.
		 * @param height Cylinder height.
		 * @param slices Number of slices.
		 * @param color Cylinder color.
		 */
		void drawCylinderWires(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color);

		/**
		 * @brief Loads a model from file
		 * @param fileName The file path to the model.
		 * @return The loaded model
		 */
		Model loadModel(std::string const &fileName);

        /**
         * @brief Unload and free a model from VRAM.
         * @param model The model to unload.
         */
        void unloadModel(Model const &model);

        /**
		 * @brief Loads a music from file
		 * @param fileName The file path to the music.
		 * @return The loaded music
		 */
		Music loadMusicStream(std::string const &fileName);

        /**
         * @brief init the audio device.
         */
        void initAudioDevice();

        /**
         * @brief play a music from VRAM.
         * @param music The music to play.
         */
        void playMusicStream(Music const &music);

        /**
         * @brief update a music from VRAM.
         * @param music The music to update.
         */
        void updateMusicStream(Music const &music);

        /**
         * @brief stop a music from VRAM.
         * @param music The music to stop.
         */
     	void stopMusicStream(Music const &music);

        /**
         * @brief Unload and free a music from VRAM.
         * @param music The music to unload.
         */
	    void unloadMusicStream(Music const &music);

		/**
		 * Changes music volume
		 * @param music The music to change volume
		 * @param volume The volume value
		 */
		void setMusicVolume(Music const &music, float volume);

        /**
         * @brief play a sound from VRAM.
         * @param sound The sound to play.
         */
        void playSound(Sound const &sound);

        /**
         * Changes sound volume
         * @param sound The sound to change volume
         * @param volume The volume value
         */
        void setSoundVolume(Sound const &sound, float volume);

        /**
         * @brief Loads a sound from file
         * @param fileName The file path to the sound.
         * @return The loaded sound
         */
        Sound loadSound(std::string const &fileName);

        /**
         * @brief Unload and free a sound from VRAM.
         * @param sound The sound to unload.
         */ 
        void unloadSound(Sound const &sound);

		/**
		 * @brief Load shader from files and bind default locations
		 * @param vsFileName File name for vertex shader
		 * @param fsFileName File name for fragment shader
		 * @return The loaded shader
		 */
		Shader loadShader(std::string const &vsFileName, std::string const &fsFileName);

		/**
		 * @brief Unloads shaders from gpu memory
		 * @param shader The shader to unload
		 */
		void unloadShader(Shader const &shader);

		/**
		 * Changes shader value
		 * @param shader The shader to change value from
		 * @param locIndex The value index
		 * @param value The new value (as a Vector4, float or int)
		 * @param uniformType The type of the value
		 */
		void setShaderValue(Shader const& shader, int locIndex, const Vector4& value, int uniformType);
        void setShaderValue(Shader const& shader, int locIndex, float value, int uniformType);
        void setShaderValue(Shader const& shader, int locIndex, int value, int uniformType);

		/**
		 * Get shader uniform location
		 * @param shader The shader to find urniform location
		 * @param uniformName The uniform to find
		 * @return The uniform location
		 */
		int getShaderLocation(Shader const &shader, std::string const &uniformName);

        /**
         * @brief close the audio device.
         */
	    void closeAudioDevice();


		// === rtextures module ===

		/**
		 * @brief Load texture from file into GPU memory (VRAM)
		 * @param fileName The file to load
		 * @return The loaded texture
		 */
		Texture2D loadTexture(const std::string &fileName);

		/**
		 * @brief Unload texture from GPU memory (VRAM)
		 * @param texture The texture to unload
		 */
		void unloadTexture(Texture2D texture);

		/**
		 * @brief Draw a Texture2D
		 * @param texture The texture to draw
		 * @param posX The X position of the texture
		 * @param posY The Y position of the texture
		 * @param tint The tint color of the texture
		 */
		void drawTexture(Texture2D texture, int posX, int posY, Color tint);

		/**
		 * @brief Draw a Texture2D with extended parameters
		 * @param texture The texture to draw
		 * @param position The texture position
		 * @param rotation Texture rotation in degrees
		 * @param scale Multiplier for texture size
		 * @param tint The tint color of the texture
		 */
		void drawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);

        // === mouse input ===

        /**
         * @brief Get the current mouse position.
         * @return Mouse position as Vector2.
         */
        Vector2 getMousePosition();

        /**
         * @brief Check if a mouse button is currently pressed.
         * @param button Mouse button code.
         * @return True if the button is down.
         */
        bool isMouseButtonDown(int button);

        /**
         * @brief Check if a mouse button was released.
         * @param button Mouse button code.
         * @return True if the button was released.
         */
        bool isMouseButtonReleased(int button);

        /**
         * @brief Check if a mouse button was pressed.
         * @param button Mouse button code.
         * @return True if the button was pressed.
         */
        bool isMouseButtonPressed(int button);

        // === keyboard input ===

        /**
         * @brief Check if a key is being held down.
         * @param key Key code.
         * @return True if the key is down.
         */
        bool isKeyDown(int key);

        /**
         * @brief Check if a key was just pressed.
         * @param key Key code.
         * @return True if the key was pressed.
         */
        bool isKeyPressed(int key);

        /**
         * @brief Check if a key was just released.
         * @param key Key code.
         * @return True if the key was released.
         */
        bool isKeyReleased(int key);

		// === Game controller input ===

		/**
		 * @brief Check if a gamepad is available
		 * @param gamepad The gamepad to check
		 * @return True if available, false otherwise
		 */
		bool isGamepadAvailable(int gamepad);

		/**
		 * @brief Get gamepad internal name id
		 * @param gamepad The gamepad to get the name of
		 * @return The name of the given gamepad
		 */
		std::string getGamepadName(int gamepad) const;

		/**
		 * @brief Check if a gamepad button has been pressed once
		 * @param gamepad The current gamepad
		 * @param button The button to check if pressed
		 * @return True if the button has been pressed once
		 */
		bool isGamepadButtonPressed(int gamepad, int button);

		/**
		 * @brief Check if a gamepad button is being pressed
		 * @param gamepad The current gamepad
		 * @param button The button to check if pressed
		 * @return True if the button is being pressed
		 */
		bool isGamepadButtonDown(int gamepad, int button);

		/**
		 * @brief Check if a gamepad button has been released once
		 * @param gamepad The current gamepad
		 * @param button The button to check if released
		 * @return True if the button has been released once
		 */
		bool isGamepadButtonReleased(int gamepad, int button);

		/**
		 * @brief Get the last gamepad button pressed
		 * @return The last button pressed
		 */
		int getGamepadButtonPressed();

		/**
		 * @brief Get gamepad axis count for a gamepad
		 * @param gamepad The current gamepad
		 * @return The gamepad's axis count
		 */
		int getGamepadAxisCount(int gamepad);

		/**
		 * @brief Get axis movement value for a gamepad axis
		 * @param gamepad The current gamepad
		 * @param axis The axis to get the movement from
		 * @return the axis movement value
		 */
		float getGamepadAxisMovement(int gamepad, int axis);

		/**
		 * @brief Set gamepad vibration for both motors (duration in seconds)
		 * @param gamepad The current gamepad
		 * @param leftMotor The left motor force (0 to 1)
		 * @param rightMotor The right motor force (0 to 1)
		 * @param duration The duration of the vibration
		 */
		void setGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration);

		// === RLGL ===

		/**
		 * @brief Push the current matrix to stack
		 */
		void pushMatrix();

		/**
		 * @brief Multiply the current matrix by a translation matrix
		 * @param x The X pos
		 * @param y The Y pos
		 * @param z The Z pos
		 */
		void translatef(float x, float y, float z);

		/**
		 * @brief Multiply the current matrix by a rotation matrix
		 * @param angle The rotation angle
		 * @param x The X pos
		 * @param y The Y pos
		 * @param z The Z pos
		 */
		void rotatef(float angle, float x, float y, float z);

		/**
		 * @brief Pop latest inserted matrix from stack
		 */
		void popMatrix();

        void drawTexturePro(Texture2D texture, Rectangle sourceRec, Rectangle destRec, Vector2 origin, float rotation, Color tint);
};

#endif /* !RAYLIB_HPP */
