
#include "gameScene.hpp"

namespace mc2d {


        GameScene::GameScene(GameWorld& gameWorld) : m_playerSprite(Sprite()), m_playerCamera(Camera(0.0f, 18.0f, 1.0f, 18, 18)),
                m_gameWorld(gameWorld), m_currPlayerId(0), m_optimizedDraw(false), m_cursorBlockType(BlockType::GRASS)
        {}


        // Game scene destructor, terminates the scene if not terminated yet
        GameScene::~GameScene()
        {
                if(isInit())
                        terminate();
        }



        // Initializes all the resources needed for the game scene
        // @returns: zero on success, non zero on failure
        int GameScene::init()
        {
                if(isInit())
                        return 1;

                // Initialize the world renderer
                if(m_worldRenderer.init(Chunk::width * Chunk::height) != 0)
                {
                        logError("GameScene::init() failed, initialization of WorldRenderer failed!");
                        return 1;
                }

                // TODO: Move this somewhere else ??? =====
                if(!m_playerSprite.load("../resources/textures/player/steveHead.png"))
                {
                        logError("GameScene::init() failed, cannot load player sprite!");
                        m_worldRenderer.terminate();
                        return 1;
                }//====================================
                
                printHelp();
                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void GameScene::terminate()
        {
                if(!isInit())
                        return;

                m_worldRenderer.terminate();

                m_playerSprite.deactivate();
                m_playerSprite.unload();
                m_isInit = false;
        }


        // Implements the logic of the game scene
        // @game: the game instance that and invoked the update of the scene
        // @deltaTime: time passed from the last update (measured in milliseconds)
        void GameScene::update(Game& game, float deltaTime)
        {
                if(!isInit())
                {
                        logError("GameScene::update() failed, game scene has not been initialized or initialization failed!");
                        return;
                }

                // Update world
                m_gameWorld.update(deltaTime);

                // TODO: this works for gameplay but triggers recomputation of all the vertices for all the visible blocks,
                // doing this each frame seems a little overkill...
                m_playerCamera.centerOnPoint(m_gameWorld.getPlayers()[m_currPlayerId].getPos());
        }


        // Defines how the game scene is rendered
        // @game: the game instance that invoked the rendering of the scene
        // @renderer: the main game renderer
        void GameScene::render(Game& game, Renderer& renderer)
        {
                if(!isInit())
                {
                        logError("GameScene::render() failed, game scene has not been initialized or initialization failed!");
                        return;
                }

                m_worldRenderer.render(m_gameWorld, m_playerCamera, m_optimizedDraw);

                for(auto& p : m_gameWorld.getPlayers()) // Draw heads of all players in the game world
                        renderer.renderSprite(m_playerSprite, p.getPos(), glm::vec3(0.5f), 0.0f, m_playerCamera);
        }


        // Callback that defines how the game scene reacts to keyboard events
        // @game: the game instance that invoked the callback
        // @wnd: the window that has been resized
        // @key: which key the event is related to
        // @scancode: the scancode of the key
        // @action: the event type (press, release, ...)
        // @mods: the state of modifier keys
        void GameScene::onKeyEvent(Game& game, GLFWwindow* wnd, int key, int scancode, int action, int mods)
        {
                if(!isInit())
                {
                        logError("GameScene::onKeyEvent() failed, game scene has not been initialized or initialization failed!");
                        return;
                }

                switch(key)
                {
                        // Returns to the menu screen
                        case GLFW_KEY_ESCAPE:
                                if(action == GLFW_PRESS && game.setScene(std::make_unique<MenuScene>()))
                                {
                                        logInfo("SWITCHING TO MENU SCENE!")
                                        return;
                                }
                                break;

                        // Debug code to generate a new random world when G is pressed
                        case GLFW_KEY_G:
                                if(action == GLFW_PRESS)
                                        m_gameWorld = WorldGenerator::generateRandomWorld((unsigned) std::time(nullptr), 3);
                                break;

                        // Debug code to generate a flat world when F is pressed
                        case GLFW_KEY_F:
                                if(action == GLFW_PRESS)
                                        m_gameWorld = WorldGenerator::generateFlatWorld(3);
                                break;

                        // Save the current game world when Ctrl + S is pressed
                        case GLFW_KEY_S:
                                if(action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
                                        // TODO: Save the current world
                                        //WorldLoader::saveWorld(m_gameWorld.getWorldSaveDirectory(), m_gameWorld);
                                break;

                        // Change cursor block type to the previous one
                        case GLFW_KEY_1:
                                if(action == GLFW_PRESS)
                                        if(m_cursorBlockType != BlockType::GRASS)
                                                m_cursorBlockType = (BlockType) ( (uint8_t) m_cursorBlockType - 1);
                                break;

                        // Change cursor block type to the next one
                        case GLFW_KEY_2:
                                if(action == GLFW_PRESS)
                                        if(m_cursorBlockType != BlockType::AIR)
                                                m_cursorBlockType = (BlockType) ( (uint8_t) m_cursorBlockType + 1);
                                break;

                        // Switch between solid and wireframe rendering
                        case GLFW_KEY_W:
                                if(action == GLFW_PRESS)
                                {
                                        static bool isWireframe = false;

                                        isWireframe = !isWireframe;
                                        isWireframe == true ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                                }
                                break;

                        // Switch between optimized and basic world rendering (TODO: Remove this when testing on renderer will be over)
                        case GLFW_KEY_O:
                                if(action == GLFW_PRESS)
                                {
                                        m_optimizedDraw = !m_optimizedDraw;
                                        logInfo("Switched to %s world rendering", m_optimizedDraw == true ? "optimized" : "basic");
                                        m_gameWorld.setHasChanged(true);        // Say that world has change to force the recomputation of vertices
                                }
                                break;

                        // Print controls list in console
                        case GLFW_KEY_H:
                                if(action == GLFW_PRESS)
                                        printHelp();
                                break;

                        // Print game details in console
                        case GLFW_KEY_F1:
                                if(action == GLFW_PRESS)
                                {
                                        logWarn("Game Info:");
                                        logInfo("       - camera pos: (%f, %f)", m_playerCamera.getPos().x, m_playerCamera.getPos().y);
                                        logInfo("       - camera size: (%f, %f)", m_playerCamera.getWidth(), m_playerCamera.getHeight());

                                        logInfo("");
                                        logInfo("       ==========[ Players info ]==========");
                                        std::vector<Entity>& players = m_gameWorld.getPlayers();
                                        for(size_t i = 0; i < players.size(); ++i)
                                                logInfo("       player %d] pos: (%f, %f), contained in chunk %d", i,
                                                        players[i].getPos().x, players[i].getPos().y, m_gameWorld.getEntityChunkId(players[i]) );

                                        logInfo("");
                                        logInfo("       ==========[ Loaded chunks info ]==========");
                                        for(const auto& c : m_gameWorld.getLoadedChunks())
                                                logInfo("       chunk %d] biome: %s", c.second.id, Biome::getBiomeProperties(c.second.biome).name.c_str() );
                                
                                        logInfo("");
                                }
                                break;

                        // Player movement
                        case GLFW_KEY_LEFT:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                        m_gameWorld.getPlayers()[m_currPlayerId].setAcceleration(glm::vec3(-80.0f, 0.0f, 0.0f));
                                break;

                        case GLFW_KEY_RIGHT:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                        m_gameWorld.getPlayers()[m_currPlayerId].setAcceleration(glm::vec3(80.0f, 0.0f, 0.0f));
                                break;

                        case GLFW_KEY_UP:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                        m_gameWorld.getPlayers()[m_currPlayerId].setAcceleration(glm::vec3(0.0f, 80.0f, 0.0f));
                                break;

                        case GLFW_KEY_DOWN:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                        m_gameWorld.getPlayers()[m_currPlayerId].setAcceleration(glm::vec3(0.0f, -80.0f, 0.0f));
                                break;

                        // Camera resize
                        case GLFW_KEY_KP_ADD:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                {
                                        m_playerCamera.setWidth(m_playerCamera.getWidth() - 1);
                                        m_playerCamera.setHeight(m_playerCamera.getHeight() - 1);
                                }
                                break;

                        case GLFW_KEY_KP_SUBTRACT:
                                if(action == GLFW_PRESS || action == GLFW_REPEAT)
                                {
                                        m_playerCamera.setWidth(m_playerCamera.getWidth() + 1);
                                        m_playerCamera.setHeight(m_playerCamera.getHeight() + 1);
                                }
                                break;

                        // Spawn new player in the game world
                        case GLFW_KEY_P:
                                if(action == GLFW_PRESS)
                                        m_gameWorld.getPlayers().push_back( Entity(glm::vec3(0.0f), 100.0f, EntityType::PLAYER) );
                                break;

                        // Switch to the previous available player in the game world
                        case GLFW_KEY_Z:
                                if(action == GLFW_PRESS)
                                {
                                        size_t newPlayerId = (m_currPlayerId - 1) % m_gameWorld.getPlayers().size();
                                        m_currPlayerId = newPlayerId;
                                        logInfo("Switched to the player %lu", newPlayerId);
                                }
                                break;

                        // Switch to the next available player in the game world
                        case GLFW_KEY_X:
                                if(action == GLFW_PRESS)
                                {
                                        size_t newPlayerId = (m_currPlayerId + 1) % m_gameWorld.getPlayers().size();
                                        m_currPlayerId = newPlayerId;
                                        logInfo("Switched to the player %lu", newPlayerId);
                                }
                                break;
                }
        }


        // Callback that defines how the game scene reacts to keyboard events
        // @game: the game instance that invoked the callback
        // @wnd: the window that received the mouse button event
        // @button: the mouse button on which the event has hapened
        // @action: specify the state of the button (pressed, released, hold)
        // @modifiers: state of modifiers keys (alt, shift, ...)
        void GameScene::onMouseButtonEvent(Game& game, GLFWwindow* wnd, int btn, int action, int modifiers)
        {
                if(!isInit())
                {
                        logError("GameScene::onMouseButtonEvent() failed, game scene has not been initialized or initialization failed!");
                        return;
                }

                // On left mouse click remove a block from the current chunk
                if(btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
                {
                        double mouseX, mouseY;
                        glfwGetCursorPos(wnd, &mouseX, &mouseY);

                        // Convert window coordinates into world coordinates
                        glm::vec2 blockCoord = m_playerCamera.windowToWorldCoord((float) mouseX, (float) mouseY,
                                        (float) game.getSettings().windowWidth, (float) game.getSettings().windowHeight);

                        m_gameWorld.setBlock(blockCoord.x, blockCoord.y, BlockType::AIR);
                }

                // On right mouse click add a block in the current chunk
                if(btn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
                {
                        double mouseX, mouseY;
                        glfwGetCursorPos(wnd, &mouseX, &mouseY);

                        // Convert window coordinates into world coordinates
                        glm::vec2 blockCoord = m_playerCamera.windowToWorldCoord((float) mouseX, (float) mouseY,
                                        (float) game.getSettings().windowWidth, (float) game.getSettings().windowHeight);

                        m_gameWorld.setBlock(blockCoord.x, blockCoord.y, m_cursorBlockType);
                }
        }


        // Prints some info about the game controls in the console
        void GameScene::printHelp() const
        {
                logWarn("Available controls:");
                logInfo("World controls:");
                logInfo("       - press G to generate a random world");
                logInfo("       - press F to generate a flat world");
                logInfo("       - press Ctrl + S to save the current world");

                logInfo("Rendering controls:");
                logInfo("       - press W to switch between solid and wireframe rendering");
                logInfo("       - press O to switch between optimized and basic world rendering");
                
                logInfo("Block controls:");
                logInfo("       - left mouse click to delete a block");
                logInfo("       - right mouse click to place a block");
                logInfo("       - press 1 and 2 to change the block type that will be placed");

                logInfo("Player/camera controls:");
                logInfo("       - use arrows to move the player around");
                logInfo("       - use keypad + and keypad - to change camera size");
                logInfo("       - use P to spawn another player")
                logInfo("       - use Z and X to switch between players in the world");

                logInfo("Other controls:");
                logInfo("       - press Esc to go back to menu");
                logInfo("       - press H to show this controls list");
                logInfo("       - press F1 to show some game info\n");
        }

}

