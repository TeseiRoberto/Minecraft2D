
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED), m_window(NULL), m_optimizedDraw(false), m_cursorBlockType(BlockType::GRASS)
        {}


        // Terminates the game (if not already done)
        Game::~Game()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                        terminate();
        }


        // Attempts to initialize all the resources needed by the game
        // @settings: struct that defines the game settings
        void Game::init(const GameSettings& settings)
        {
                if(m_gameState != GameState::UNINITIALIZED)
                {
                        logWarn("Game::init() failed, game has already been initialized!");
                        return;
                }

                m_gameState = GameState::INITIALIZED;
                m_settings = settings;

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
                m_window = glfwCreateWindow(settings.windowWidth, settings.windowHeight, "Minecraft-2D", NULL, NULL);
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
                glfwSetMouseButtonCallback(m_window, this->onMouseButtonEvent);

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

                // Print available inputs
                logInfo("Available inputs:");
                logInfo("Chunk stuff:");
                logInfo("       press G to generate a random chunck");
                logInfo("       press H to generate a flat world chunk");

                logInfo("Rendering stuff:");
                logInfo("       press W to switch between solid and wireframe rendering");
                logInfo("       press O to switch between optimized and basic world rendering");
                
                logInfo("Block stuff:");
                logInfo("       left mouse click to delete a block");
                logInfo("       right mouse click to place a block");
                logInfo("       press 1 and 2 to change the block type that will be placed\n");

                m_currChunk.generate();

                while(!glfwWindowShouldClose(m_window))
                {
                        // TODO: Remove this if statement when texting will be over
                        if(m_optimizedDraw)
                        {
                                m_renderer.optimizedRenderWorld(m_currChunk, m_camera);
                        } else {
                                m_renderer.renderWorld(m_currChunk, m_camera);
                        }

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

                // Debug code to regenerate chunk when G is pressed
                if(key == GLFW_KEY_G && action == GLFW_PRESS)
                        game->m_currChunk.generate();

                // Debug code to regenerate chunk when G is pressed
                if(key == GLFW_KEY_H && action == GLFW_PRESS)
                        game->m_currChunk.generateFlatChunk();

                // Switch between solid and wireframe rendering
                if(key == GLFW_KEY_W && action == GLFW_PRESS)
                {
                        static bool isWireframe = false;

                        isWireframe = !isWireframe;
                        isWireframe == true ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                // Change cursor block type to the previous one
                if(key == GLFW_KEY_1 && action == GLFW_PRESS)
                {
                        if(game->m_cursorBlockType != BlockType::GRASS)
                                game->m_cursorBlockType = (BlockType) ( (uint8_t) game->m_cursorBlockType - 1);
                }

                // Change cursor block type to the next one
                if(key == GLFW_KEY_2 && action == GLFW_PRESS)
                {
                        if(game->m_cursorBlockType != BlockType::AIR)
                                game->m_cursorBlockType = (BlockType) ( (uint8_t) game->m_cursorBlockType + 1);
                }

                // TODO: Remove this if when testing will be over
                // Switch between optimized and basic world rendering
                if(key == GLFW_KEY_O && action == GLFW_PRESS)
                {
                        game->m_optimizedDraw = !game->m_optimizedDraw;
                        logInfo("Switched to %s world rendering", game->m_optimizedDraw == true ? "optimized" : "basic");
                }
        }


        // Callback invoked when the game window receives a mouse button event
        // @wnd: the window that received the mouse button event
        // @button: the mouse button on which the event has hapened
        // @action: specify the state of the button (pressed, released, hold)
        // @modifiers: state of modifiers keys (alt, shift, ...)
        void Game::onMouseButtonEvent(GLFWwindow* wnd, int btn, int action, int modifiers)
        {
                Game* game = static_cast<Game*>( glfwGetWindowUserPointer(wnd) );

                // On left mouse click remove a block from the current chunk
                if(btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
                {
                        double mouseX, mouseY;
                        glfwGetCursorPos(wnd, &mouseX, &mouseY);

                        double blockWidth = (double) game->m_settings.windowWidth / (double) Chunk::width;
                        double blockHeight = (double) game->m_settings.windowHeight / (double) Chunk::height;

                        uint32_t blockX = (uint32_t) (mouseX / blockWidth);
                        uint32_t blockY = (uint32_t) (mouseY / blockHeight);

                        logInfo("Deleted block of type: %d", game->m_currChunk.blocks[blockY * Chunk::width + blockX]);
                        game->m_currChunk.blocks[blockY * Chunk::width + blockX] = BlockType::AIR;
                        game->m_currChunk.hasChanged = true;
                }

                // On right mouse click add a block in the current chunk
                if(btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                        double mouseX, mouseY;
                        glfwGetCursorPos(wnd, &mouseX, &mouseY);

                        double blockWidth = (double) game->m_settings.windowWidth / (double) Chunk::width;
                        double blockHeight = (double) game->m_settings.windowHeight / (double) Chunk::height;

                        uint32_t blockX = (uint32_t) (mouseX / blockWidth);
                        uint32_t blockY = (uint32_t) (mouseY / blockHeight);

                        game->m_currChunk.blocks[blockY * Chunk::width + blockX] = game->m_cursorBlockType;
                        game->m_currChunk.hasChanged = true;
                }

        }

}

