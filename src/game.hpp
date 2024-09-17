
// Contains definition of the Game class and the GameSettings struct.

#ifndef GAME_H
#define GAME_H

#include <string>
#include <memory>
#include <chrono>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "logging.hpp"
#include "graphics/renderer.hpp"
#include "graphics/camera.hpp"

#include "scene/scene.hpp"
#include "scene/menuScene.hpp"
#include "scene/gameScene.hpp"

namespace mc2d {


        // Name of the directory in which all game data will be saved  (settings and game worlds)
        constexpr const char* GAME_DATA_DIRECTORY = "minecraft2D_data";


        struct GameSettings {
                uint32_t        windowWidth     = 720;          // The width of the game window
                uint32_t        windowHeight    = 480;          // the height of the game window
        };


        class Game {
        public:
                Game();
                ~Game();

                int                             init(const GameSettings& settings);
                void                            terminate();
                void                            run();

                bool                            setScene(std::unique_ptr<Scene>&& newScene);

                inline GameSettings&            getSettings()                   { return m_settings; }
                inline std::filesystem::path    getGameDataDirectory() const    { return m_gameDataDir; }


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
                static void     onMouseButtonEvent(GLFWwindow* wnd, int btn, int action, int mods);

                GameState               m_gameState;                    // The current state of the game
                GameSettings            m_settings;                     // The game settings
                std::filesystem::path   m_gameDataDir;                  // Directory that contains all game data (settings and save games)
                GLFWwindow*             m_window;                       // Game's main window
                Renderer                m_renderer;

                std::unique_ptr<Scene>  m_currScene;                    // The scene currently active 
        };

}

#endif // GAME_H
