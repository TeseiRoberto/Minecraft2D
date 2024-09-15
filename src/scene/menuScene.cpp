
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

               
                m_currScreen = MenuScreen::MAIN_MENU;

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
                switch(m_currScreen)
                {
                        case MenuScreen::MAIN_MENU:

                                switch(m_userChoice)
                                {
                                        case -1:        /*User has not chosen yet*/     break;
                                        case 1:         onCreateNewGameSelected();      break;
                                        case 2:         onLoadGameSaveSelected();       break;
                                        case 3:         onSettingsSelected();           break;
                                        default:        onInvalidOptionSelected();      break;
                                }
                                break;

                        case MenuScreen::CREATE_NEW_GAME:
                                // TODO: Add implementation...
                                if(game.setScene(std::make_unique<GameScene>()))
                                        return;
                                break;

                        case MenuScreen::LOAD_GAME_SAVE:
                                switch(m_userChoice)
                                {
                                        case -1:        /*User has not chosen yet*/     break;
                                        case 0:         onBackToMainMenuSelected();     break;
                                        default:        onGameSaveSelected(game);       break;
                                }
                                break;

                        case MenuScreen::SETTINGS:      onBackToMainMenuSelected();     break;
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
                        switch(m_currScreen)
                        {
                                case MenuScreen::MAIN_MENU:
                                        logInfo("");
                                        logInfo("==========[ Minecraft 2D ] ==========");
                                        logInfo("1] Create a new game save");
                                        logInfo("2] Load a saved game");
                                        logInfo("3] Settings");
                                        logInfo("==> choose an option...");
                                        break;

                                case MenuScreen::CREATE_NEW_GAME:
                                        // TODO: Add implementation...
                                        logInfo("Launching new game!");
                                        logInfo("======================================");
                                        break;

                                case MenuScreen::LOAD_GAME_SAVE:
                                {

                                        logInfo("");
                                        logInfo("==========[ available game saves ] ==========");
                                        
                                        uint32_t i = 1;
                                        for(const auto& entry : std::filesystem::directory_iterator(game.getGameDataDirectory()))
                                                logInfo("%u] %s", i++, entry.path().c_str());

                                        logInfo("");
                                        logInfo("==> press 0 to go back or choose a game save...");
                                }
                                        break;

                                case MenuScreen::SETTINGS:
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


        // Callback invoked when the player selects the "create new game" entry in the main menu
        void MenuScene::onCreateNewGameSelected()
        {
                m_currScreen = MenuScreen::CREATE_NEW_GAME;
                m_userChoice = -1;
                m_renderMenu = true;
        }


        // Callback invoked when the player selects the "load game save" entry in the main menu
        void MenuScene::onLoadGameSaveSelected()
        {
                m_currScreen = MenuScreen::LOAD_GAME_SAVE;
                m_userChoice = -1;
                m_renderMenu = true;
        }
        
        
        // Callback invoked when the player selects the "settings" entry in the main menu
        void MenuScene::onSettingsSelected()
        {
                m_currScreen = MenuScreen::SETTINGS;
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
                m_currScreen = MenuScreen::MAIN_MENU;
                m_userChoice = -1;
                m_renderMenu = true;
        }


        // Callback invoked when the player selects a game save from the load game save menu
        void MenuScene::onGameSaveSelected(Game& game)
        {
                uint32_t i = 1;
                for(const auto& entry : std::filesystem::directory_iterator(game.getGameDataDirectory()))
                {
                        if(i++ == m_userChoice)
                        {
                                // TODO: Actually load the chosen game save
                                logInfo("Loading game save: %s!", entry.path().c_str());
                                logError("Loading of game saves is not implemented yet!");
                                logInfo("%d is not valid, please choose a valid option ==>", m_userChoice);
                                m_userChoice = -1;
                                return;
                        }
                }

               logInfo("%d is not valid, please choose a valid option ==>", m_userChoice);
               m_userChoice = -1;
        }

}

