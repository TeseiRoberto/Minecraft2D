
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED), m_window(nullptr)
        {}


        // Terminates the game (if not already done)
        Game::~Game()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                        terminate();
        }


        // Attempts to initialize all the resources needed by the game
        void Game::init()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                {
                        logWarn("Game::init() failed, game has already been initialized!");
                        return;
                }

                // Initialize glfw
                if(!glfwInit())
                {
                        logError("Game::init() failed, glfw initialization failed!");
                        return;
                }

                // Set window properties and create the game window
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                m_window = glfwCreateWindow(720, 480, "Minecraft2D", NULL, NULL);
                if(m_window == NULL)
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        glfwTerminate();
                        return;
                }

                // Set callbacks to handle window events
                glfwSetFramebufferSizeCallback(m_window, this->onWindowResize);
                
                glfwMakeContextCurrent(m_window);

                // Initialize opengl using glad
                if(!gladLoadGL())
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        glfwDestroyWindow(m_window);
                        m_window = nullptr;
                        glfwTerminate();
                        return;
                }

                // TODO: Initialize other stuff..
                m_gameState = GameState::INITIALIZED;
        }


        // Terminates all the resources initialized in the init method
        // (in the reverse order in which they were initialized)
        void Game::terminate()
        {
                if(m_gameState == GameState::UNINITIALIZED)
                {
                        logWarn("Game::terminate() failed, game has already been terminated!");
                        return;
                }

                // TODO: Terminate other stuff..
      
                glfwDestroyWindow(m_window);
                m_window = nullptr;
                glfwTerminate();

                m_gameState = GameState::UNINITIALIZED;                
        }


        // Starts the main game loop
        void Game::run()
        {
                if(m_gameState != GameState::INITIALIZED)
                {
                        logError("Game::run() failed, game has not been intialized correctly or is already running!");
                        return;
                }

                while(!glfwWindowShouldClose(m_window))
                {
                        // TODO: Implement main game loop
                
                        glfwPollEvents();
                        glfwSwapBuffers(m_window);
                }

                m_gameState = GameState::QUITTED;
        }


        // Callback invoked when the game window gets resized
        // @wnd: the window that has been resized
        // @width: the new window width
        // @height: the new window height
        void Game::onWindowResize(GLFWwindow* wnd, int width, int height)
        {
                logInfo("Window resized to (%d, %d)", width, height);
        }
}
