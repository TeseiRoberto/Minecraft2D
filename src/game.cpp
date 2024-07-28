
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED), m_window(NULL), m_camera(Camera(0.0f, 18.0f, 1.0f, 18, 18)), m_gameWorld(GameWorld()),
                m_optimizedDraw(false), m_cursorBlockType(BlockType::GRASS)
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

                printHelp();
                m_gameWorld.generateRandomWorld();

                while(!glfwWindowShouldClose(m_window))
                {
                        m_renderer.renderWorld(m_gameWorld, m_camera, m_optimizedDraw);

                        glfwPollEvents();
                        glfwSwapBuffers(m_window);
                }

                m_gameState = GameState::QUITTED;
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

                if(action == GLFW_PRESS)
                {
                        switch(key)
                        {
                                // Debug code to generate random chunk when G is pressed
                                case GLFW_KEY_G:
                                        game->m_gameWorld.generateRandomWorld();
                                        break;

                                // Debug code to generate flat chunk when F is pressed
                                case GLFW_KEY_F:
                                        game->m_gameWorld.generateFlatWorld();
                                        break;

                                // Change cursor block type to the previous one
                                case GLFW_KEY_1:
                                        if(game->m_cursorBlockType != BlockType::GRASS)
                                                game->m_cursorBlockType = (BlockType) ( (uint8_t) game->m_cursorBlockType - 1);
                                        break;

                                // Change cursor block type to the next one
                                case GLFW_KEY_2:
                                        if(game->m_cursorBlockType != BlockType::AIR)
                                                game->m_cursorBlockType = (BlockType) ( (uint8_t) game->m_cursorBlockType + 1);
                                        break;

                                // Switch between solid and wireframe rendering
                                case GLFW_KEY_W: {
                                        static bool isWireframe = false;

                                        isWireframe = !isWireframe;
                                        isWireframe == true ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                                        } break;

                                // Switch between optimized and basic world rendering (TODO: Remove this when testing on renderer will be over)
                                case GLFW_KEY_O:
                                        game->m_optimizedDraw = !game->m_optimizedDraw;
                                        logInfo("Switched to %s world rendering", game->m_optimizedDraw == true ? "optimized" : "basic");
                                        game->m_gameWorld.setHasChanged(true);          // Say that world has change to force the recomputation of vertices
                                        break;

                                // Print controls list in console
                                case GLFW_KEY_H:
                                        game->printHelp();
                                        break;

                                // Camera movement
                                case GLFW_KEY_LEFT:     
                                        game->m_camera.updatePos(-1.0f, 0.0f);
                                        logWarn("Camera moved to (%f, %f)", game->m_camera.getPos().x, game->m_camera.getPos().y);
                                        break;

                                case GLFW_KEY_RIGHT:
                                        game->m_camera.updatePos(1.0f, 0.0f);
                                        logWarn("Camera moved to (%f, %f)", game->m_camera.getPos().x, game->m_camera.getPos().y);
                                        break;

                                case GLFW_KEY_UP:
                                        game->m_camera.updatePos(0.0f, 1.0f);
                                        logWarn("Camera moved to (%f, %f)", game->m_camera.getPos().x, game->m_camera.getPos().y);
                                        break;

                                case GLFW_KEY_DOWN:
                                        game->m_camera.updatePos(0.0f, -1.0f);
                                        logWarn("Camera moved to (%f, %f)", game->m_camera.getPos().x, game->m_camera.getPos().y);
                                        break;

                                // Camera resize
                                case GLFW_KEY_KP_ADD:
                                        game->m_camera.setWidth(game->m_camera.getWidth() - 1);
                                        game->m_camera.setHeight(game->m_camera.getHeight() - 1);
                                        logWarn("Camera resized to (%u, %u)", game->m_camera.getWidth(), game->m_camera.getHeight());
                                        break;

                                case GLFW_KEY_KP_SUBTRACT:
                                        game->m_camera.setWidth(game->m_camera.getWidth() + 1);
                                        game->m_camera.setHeight(game->m_camera.getHeight() + 1);
                                        logWarn("Camera resized to (%u, %u)", game->m_camera.getWidth(), game->m_camera.getHeight());
                                        break;
                        }
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

                        // Convert window coordinates into world coordinates
                        glm::vec2 blockCoord = game->m_camera.windowToWorldCoord((float) mouseX, (float) mouseY,
                                        (float) game->m_settings.windowWidth, (float) game->m_settings.windowHeight);

                        logInfo("Deleted block of type: %u", game->m_gameWorld.getBlock(blockCoord.x, blockCoord.y));
                        game->m_gameWorld.setBlock(blockCoord.x, blockCoord.y, BlockType::AIR);
                }

                // On right mouse click add a block in the current chunk
                if(btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                        double mouseX, mouseY;
                        glfwGetCursorPos(wnd, &mouseX, &mouseY);

                        // Convert window coordinates into world coordinates
                        glm::vec2 blockCoord = game->m_camera.windowToWorldCoord((float) mouseX, (float) mouseY,
                                        (float) game->m_settings.windowWidth, (float) game->m_settings.windowHeight);

                        game->m_gameWorld.setBlock(blockCoord.x, blockCoord.y, game->m_cursorBlockType);
                }
        }


        // Prints some info about the game controls in the console
        void Game::printHelp()
        {
                logInfo("Available controls:");
                logInfo("Chunk controls:");
                logInfo("       press G to generate a random chunck");
                logInfo("       press F to generate a flat world chunk");

                logInfo("Rendering controls:");
                logInfo("       press W to switch between solid and wireframe rendering");
                logInfo("       press O to switch between optimized and basic world rendering");
                
                logInfo("Block controls:");
                logInfo("       left mouse click to delete a block");
                logInfo("       right mouse click to place a block");
                logInfo("       press 1 and 2 to change the block type that will be placed");

                logInfo("Camera controls:");
                logInfo("       use arrows to move the camera around");
                logInfo("       use keypad + and keypad - to change camera size");

                logInfo("Other controls:");
                logInfo("       press H to show this controls list\n");
        }

}

