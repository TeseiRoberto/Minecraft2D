
#include "menuScene.hpp"

namespace mc2d {

        // Menu scene destructor, terminates the scene if not terminated yet
        MenuScene::~MenuScene()
        {
                if(isInit())
                        terminate();
        }


        // Initializes all the resources needed for the menu scene
        // @returns: zero on success, non zero on failure
        int MenuScene::init()
        {
                // TODO: add implementation...

               
                m_currMenuState = MenuState::MAIN_MENU;

                m_userChoice = -1;  // -1 means that user has not chosen yet
                m_renderMenu = true;

                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void MenuScene::terminate()
        {
                if(!isInit())
                        return;

                // TODO: add implementation...

                m_isInit = false;
        }


        // Implements the logic of the menu scene
        // @game: the game instance that invoked the update of the scene
        // @deltaTime: time passed from the last update (measured in milliseconds)
        void MenuScene::update(Game& game, float deltaTime)
        {
                // TODO: this is a temporary terminal based implementation
                switch(m_currMenuState)
                {
                        case MenuState::MAIN_MENU:

                                switch(m_userChoice)
                                {
                                        case -1:        /*User has not chosen yet*/     break;
                                        case 1:         onCreateNewWorldSelected();     break;
                                        case 2:         onLoadWorldSelected();          break;
                                        case 3:         onSettingsSelected();           break;
                                        default:        onInvalidOptionSelected();      break;
                                }
                                break;

                        case MenuState::CREATE_WORLD_MENU:
                        {
                                // TODO: need to implement a menu for the player so that
                                // he can set world properties (such as: world name, seed, ....).
                                
                                // Generate a new random world and save it
                                std::filesystem::path pathToWorldDir = game.getSettings().pathToGameData;
                                pathToWorldDir.append(WorldLoader::createDummyWorldName());

                                GameWorld newWorld = WorldGenerator::generateRandomWorld(std::time(nullptr), 3);
                                WorldLoader::saveWorld(pathToWorldDir, newWorld);

                                if( game.setScene(std::make_unique<GameScene>( std::move(newWorld) )) )
                                        return;
                        } break;
                                
                        case MenuState::LOAD_WORLD_MENU:
                                switch(m_userChoice)
                                {
                                        case -1:        /*User has not chosen yet*/     break;
                                        case 0:         onBackToMainMenuSelected();     break;
                                        default:        onWorldSelected(game);          break;
                                }
                                break;

                        case MenuState::SETTINGS_MENU:  onBackToMainMenuSelected();     break;
                }
        }


        // Defines how the menu scene is rendered
        // @game: the game instance that invoked the rendering of the scene
        // @renderer: the main game renderer
        void MenuScene::render(Game& game, Renderer& renderer)
        {
                // TODO: this is a temporary terminal based implementation, need to add GUI
                if(m_renderMenu)
                {
                        switch(m_currMenuState)
                        {
                                case MenuState::MAIN_MENU:
                                        logInfo("");
                                        logInfo("==========[ Minecraft 2D ] ==========");
                                        logInfo("1] Create a new game world");
                                        logInfo("2] Load a saved game world");
                                        logInfo("3] Settings");
                                        logInfo("==> choose an option...");
                                        break;

                                case MenuState::CREATE_WORLD_MENU:
                                        // TODO: Add implementation...
                                        logInfo("Launching new game!");
                                        logInfo("======================================");
                                        break;

                                case MenuState::LOAD_WORLD_MENU:
                                {

                                        logInfo("");
                                        logInfo("==========[ available game saves ] ==========");
                                        
                                        uint32_t i = 1;
                                        for(const auto& entry : std::filesystem::directory_iterator(game.getSettings().pathToGameData))
                                                logInfo("%u] %s", i++, entry.path().c_str());

                                        logInfo("");
                                        logInfo("==> press 0 to go back or choose a game save...");
                                }
                                        break;

                                case MenuState::SETTINGS_MENU:
                                        // TODO: Add implementation...
                                        logInfo("");
                                        logInfo("==========[ Settings ] ==========");
                                        logError("Settings are not available yet!");
                                        break;
                        }

                        m_renderMenu = false;
                }
        }


        // Callback that defines how the menu scene reacts to keyboard events
        // @game: the game instance that invoked the callback
        // @wnd: the window that has been resized
        // @key: which key the event is related to
        // @scancode: the scancode of the key
        // @action: the event type (press, release, ...)
        // @mods: the state of modifier keys
        void MenuScene::onKeyEvent(Game& game, GLFWwindow* wnd, int key, int scancode, int action, int mods)
        {
                // Check for key press of keys from 0 to 9 and convert those in range [0, 9]
                if(action == GLFW_PRESS && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
                        m_userChoice = key - GLFW_KEY_0;
        }


        // Callback that defines how the menu scene reacts to keyboard events
        // @game: the game instance that invoked the callback
        // @wnd: the window that received the mouse button event
        // @button: the mouse button on which the event has hapened
        // @action: specify the state of the button (pressed, released, hold)
        // @modifiers: state of modifiers keys (alt, shift, ...)
        void MenuScene::onMouseButtonEvent(Game& game, GLFWwindow* wnd, int btn, int action, int modifiers)
        {
                // TODO: add implementation...
        }


        // Callback invoked when the player selects the "create new world" entry in the main menu
        void MenuScene::onCreateNewWorldSelected()
        {
                m_currMenuState = MenuState::CREATE_WORLD_MENU;
                m_userChoice = -1;
                m_renderMenu = true;
        }


        // Callback invoked when the player selects the "load game world" entry in the main menu
        void MenuScene::onLoadWorldSelected()
        {
                m_currMenuState = MenuState::LOAD_WORLD_MENU;
                m_userChoice = -1;
                m_renderMenu = true;
        }
        
        
        // Callback invoked when the player selects the "settings" entry in the main menu
        void MenuScene::onSettingsSelected()
        {
                m_currMenuState = MenuState::SETTINGS_MENU;
                m_userChoice = -1;
                m_renderMenu = true;
        }


        // Callback invoked when the player selects an invalid option
        void MenuScene::onInvalidOptionSelected()
        {
                logInfo("%d is not valid, please choose a valid option ==>", m_userChoice);
                m_userChoice = -1;
        }
        

        // Callback invoked when the player selects the "back to main menu" entry
        void MenuScene::onBackToMainMenuSelected()
        {
                m_currMenuState = MenuState::MAIN_MENU;
                m_userChoice = -1;
                m_renderMenu = true;
        }


        // Callback invoked when the player selects a game world from the load world menu
        void MenuScene::onWorldSelected(Game& game)
        {
                int i = 1;
                auto currIt = std::filesystem::directory_iterator(game.getSettings().pathToGameData);
                auto endIt = std::filesystem::directory_iterator();

                while(i != m_userChoice && currIt != endIt)
                {
                        ++currIt;
                        ++i;
                }

                if(currIt == endIt)
                {
                        logInfo("%d is not valid, please choose a valid option ==>", m_userChoice);
                        m_userChoice = -1;
                        return;
                }

                m_userChoice = -1;
                GameWorld world;

                if(!WorldLoader::loadWorld(currIt->path(), world))
                        return;
                
                if(!game.setScene(std::make_unique<GameScene>( std::move(world) )) )
                        logError("Cannot load game world: \"%s\", failed to switch to the game scene!", currIt->path().c_str());
                        return;
        }

}

