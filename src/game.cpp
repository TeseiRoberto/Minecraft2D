
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED), m_window(NULL)
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

                m_gameState = GameState::INITIALIZED;

                // Initialize glfw
                if(!glfwInit())
                {
                        logError("Game::init() failed, glfw initialization failed!");
                        m_gameState = GameState::UNINITIALIZED;
                        return;
                }

                // Set window properties and create the game window
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                m_window = glfwCreateWindow(720, 480, "Minecraft2D", NULL, NULL);
                if(m_window == NULL)
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        terminate();
                        return;
                }

                // Associate the newly created window and this game instance (this is used in callback functions to retrieve the game instance)
                glfwSetWindowUserPointer(m_window, (void*) this);

                // Set callbacks to handle window events
                glfwSetFramebufferSizeCallback(m_window, this->onWindowResize);
                glfwSetKeyCallback(m_window, this->onKeyEvent);

                glfwMakeContextCurrent(m_window);

                // Initialize opengl using glad
                if(!gladLoadGL())
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        terminate();
                        return;
                }

                // Intialize renderer
                if(m_renderer.init() != 0)
                {
                        terminate();
                        return;
                }


                // TODO: Initialize other stuff..
        }


        // Terminates all the resources initialized in the init method
        // (in the reverse order in which they were initialized)
        void Game::terminate()
        {
                if(m_gameState == GameState::UNINITIALIZED)
                        return;

                // TODO: Terminate other stuff...
     
                m_renderer.terminate();

                if(m_window != NULL)                            // If window creation was successfull then glfw was initialized correctly
                {
                        glfwDestroyWindow(m_window);
                        m_window = NULL;
                        glfwTerminate();
                }

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

                m_currChunk.generate();

                while(!glfwWindowShouldClose(m_window))
                {
                        m_renderer.renderWorld(m_currChunk, m_camera);

                        glfwPollEvents();
                        glfwSwapBuffers(m_window);
                }

                m_gameState = GameState::QUITTED;
        }


        // Callback invoked when the game window gets resized
        // @wnd: the window that has been resized
        // @newWidth: the new window width
        // @newHeight: the new window height
        void Game::onWindowResize(GLFWwindow* wnd, int newWidth, int newHeight)
        {
                Game* game = static_cast<Game*>( glfwGetWindowUserPointer(wnd) );

                game->m_renderer.resizeViewport(newWidth, newHeight);
        }


        // Callback invoked when the game window receives a keyboard event
        // @wnd: the window that has been resized
        // @key: which key the event is related to
        // @scancode: the scancode of the key
        // @action: the event type (press, release, ...)
        // @mods: the state of modifier keys
        void Game::onKeyEvent(GLFWwindow* wnd, int key, int scancode, int action, int mods)
        {
                Game* game = static_cast<Game*>( glfwGetWindowUserPointer(wnd) );

                // Debug code to regenerate chunk when G is pressed
                if(key == GLFW_KEY_G && action == GLFW_PRESS)
                        game->m_currChunk.generate();
        }
}
