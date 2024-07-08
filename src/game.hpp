
// Contains definition of the Game class, this class encapsulates the game state

#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logging.hpp"
#include "chunk.hpp"
#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"

namespace mc2d {


        class Game {
        public:
                Game();
                ~Game();

                void    init();
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

                GameState       m_gameState;
                GLFWwindow*     m_window;
                Renderer        m_renderer;

                Camera          m_camera;
                Chunk           m_currChunk; // Temporary just for debug purpouses
        };

}

#endif // GAME_H
