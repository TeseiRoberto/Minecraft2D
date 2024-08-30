
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
                // TODO: add implementation...
                logWarn("MenuScene::update() called, but still lacks implementation");
        }


        // Defines how the menu scene is rendered
        // @game: the game instance that invoked the rendering of the scene
        // @renderer: the main game renderer
        void MenuScene::render(Game& game, Renderer& renderer)
        {
                // TODO: add implementation...
                logWarn("MenuScene::render() called, but still lacks implementation");
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
                // TODO: add implementation...
                logWarn("Key event happend in MenuScene");
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
                logWarn("Mouse button event happend in MenuScene");
        }

}

