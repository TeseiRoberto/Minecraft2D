
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED), m_gameDataDir("minecraft2D_data"), m_window(NULL), m_currScene(nullptr)
        {}


        // Terminates the game (if not already done)
        Game::~Game()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                        terminate();
        }


        // Attempts to initialize all the resources needed by the game
        // @settings: struct that defines the game settings
        // @returns: zero on success, non zero on failure
        int Game::init(const GameSettings& settings)
        {
                if(m_gameState != GameState::UNINITIALIZED)
                {
                        logWarn("Game::init() failed, game has already been initialized!");
                        return 1;
                }

                m_gameState = GameState::INITIALIZED;
                m_settings = settings;

                // Create directory to contain game data (if it does not exist)
                if(!std::filesystem::exists(m_gameDataDir))
                        std::filesystem::create_directory(m_gameDataDir);

                // Initialize glfw
                if(!glfwInit())
                {
                        logError("Game::init() failed, glfw initialization failed!");
                        m_gameState = GameState::UNINITIALIZED;
                        return 1;
                }

                // Set window properties and create the game window
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                m_window = glfwCreateWindow(settings.windowWidth, settings.windowHeight, "Minecraft-2D", NULL, NULL);
                if(m_window == NULL)
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        terminate();
                        return 1;
                }

                // Associate the newly created window and this game instance (this is used in callback functions to retrieve the game instance)
                glfwSetWindowUserPointer(m_window, (void*) this);

                // Set callbacks to handle window events
                glfwSetFramebufferSizeCallback(m_window, this->onWindowResize);
                glfwSetKeyCallback(m_window, this->onKeyEvent);
                glfwSetMouseButtonCallback(m_window, this->onMouseButtonEvent);

                glfwMakeContextCurrent(m_window);

                // Initialize OpenGL using glad
                if(!gladLoadGL())
                {
                        logError("Game::init() failed, glfwCreateWindow failed!");
                        terminate();
                        return 1;
                }

                // Intialize renderer
                if(m_renderer.init() != 0)
                {
                        terminate();
                        return 1;
                }

                // Create and initialize start scene
                m_currScene = std::make_unique<MenuScene>();
                if(m_currScene->init() != 0)
                {
                        logError("Game::init() failed, initialization of first game scene failed!");
                        terminate();
                        return 1;
                }

                return 0;
        }


        // Terminates all the resources initialized in the init method
        // (in the reverse order in which they were initialized)
        void Game::terminate()
        {
                if(m_gameState == GameState::UNINITIALIZED)
                        return;

                if(m_currScene != nullptr)                      // Terminate the current scene if any
                {
                        m_currScene->terminate();
                        m_currScene = nullptr;
                }

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

                std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
                std::chrono::high_resolution_clock::time_point currFrameTime;
                std::chrono::duration<float, std::milli> deltaTime;

                while(!glfwWindowShouldClose(m_window))
                {
                        // Compute delta time
                        currFrameTime = std::chrono::high_resolution_clock::now();
                        deltaTime = currFrameTime - lastFrameTime;
                        lastFrameTime = currFrameTime;

                        m_renderer.clearScreen();

                        if(m_currScene != nullptr)
                        {
                                m_currScene->update(*this, deltaTime.count());
                                m_currScene->render(*this, m_renderer);
                        }

                        // Poll events and swap buffers
                        glfwPollEvents();
                        glfwSwapBuffers(m_window);
                }

                m_gameState = GameState::QUITTED;
        }


        // Sets the current scene of the game, it can be used to switch from a scene A to 
        // a scene B but after this call sceneA will be terminated so you must return control
        // immediately to the game class
        // @newScene: the new scene for the game
        // @returns: true on success, false otherwise
        bool Game::setScene(std::unique_ptr<Scene>&& newScene)
        {
                if(newScene->init() != 0)
                {
                        logError("Game::setScene() failed, initialization of new scene failed!");
                        return false;
                }

                if(m_currScene != nullptr)
                        m_currScene->terminate();

                m_currScene = std::move(newScene);
                return true;
        }


        // Callback invoked when the game window gets resized
        // @wnd: the window that has been resized
        // @newWidth: the new width of the drawable area
        // @newHeight: the new height of the drawable area
        void Game::onWindowResize(GLFWwindow* wnd, int newWidth, int newHeight)
        {
                Game* game = static_cast<Game*>( glfwGetWindowUserPointer(wnd) );

                // Update window size in game settings
                int width, height;
                glfwGetWindowSize(wnd, &width, &height);
                game->m_settings.windowWidth = (uint32_t) width;
                game->m_settings.windowHeight = (uint32_t) height;

                // Update render viewport
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

                if(game->m_currScene != nullptr)
                        game->m_currScene->onKeyEvent(*game, wnd, key, scancode, action, mods);
        }
        

        // Callback invoked when the game window receives a mouse button event
        // @wnd: the window that received the mouse button event
        // @button: the mouse button on which the event has hapened
        // @action: specify the state of the button (pressed, released, hold)
        // @modifiers: state of modifiers keys (alt, shift, ...)
        void Game::onMouseButtonEvent(GLFWwindow* wnd, int btn, int action, int mods)
        {
                Game* game = static_cast<Game*>( glfwGetWindowUserPointer(wnd) );
                
                if(game->m_currScene != nullptr)
                        game->m_currScene->onMouseButtonEvent(*game, wnd, btn, action, mods);
        }

}

