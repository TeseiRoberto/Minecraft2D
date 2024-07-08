
// Contains definition of the Game class and the GameSettings struct.

#ifndef GAME_H
#define GAME_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logging.hpp"
#include "chunk.hpp"
#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"

namespace mc2d {


        struct GameSettings {
                uint32_t        windowWidth     = 720;          // The width of the game window
                uint32_t        windowHeight    = 480;          // the height of the game window
        };


        class Game {
        public:
                Game();
                ~Game();

                void    init(const GameSettings& settings);
                void    terminate();
                void    run();

        private:

                enum class GameState {
                        UNINITIALIZED,  // Game has not been initialized (init has not been called or an error occurred during initialization) or has been terminated
                        INITIALIZED,    // Game has been initialized correctly and is ready for run
                        RUNNING,        // Game is running
                        PAUSED,         // Game has been paused
                        QUITTED         // Game has been quitted
                };


                static void     onWindowResize(GLFWwindow* wnd, int width, int height);
                static void     onKeyEvent(GLFWwindow* wnd, int key, int scancode, int action, int mods);
                static void     onMouseButtonEvent(GLFWwindow* wnd, int btn, int action, int modifiers);


                GameState       m_gameState;                    // The current state of the game
                GameSettings    m_settings;                     // The game settings
                GLFWwindow*     m_window;                       // Game's main window
                Renderer        m_renderer;

                Camera          m_camera;
                Chunk           m_currChunk; // Temporary just for debug purpouses
        };

}

#endif // GAME_H
