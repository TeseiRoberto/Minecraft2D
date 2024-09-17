
// Contains definition of the MenuScene class, this class implements the game menu behaviour.

#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <memory>

#include "logging.hpp"
#include "game.hpp"
#include "scene.hpp"
#include "world/worldGenerator.hpp"
#include "world/worldLoader.hpp"
#include "graphics/renderer.hpp"

// keep this include order (renderer.hpp includes glad.h which must be included before glfw)
#include <GLFW/glfw3.h> 

namespace mc2d {


        // TODO: Still needs an implementation...
        class MenuScene : public Scene {
        public:
                MenuScene() = default;
                virtual ~MenuScene();

                virtual int     init();
                virtual void    terminate();

                virtual void    update(Game& game, float deltaTime);
                virtual void    render(Game& game, Renderer& renderer);

                virtual void    onKeyEvent(Game& game, GLFWwindow* wnd, int key, int scancode, int action, int mods);
                virtual void    onMouseButtonEvent(Game& game, GLFWwindow* wnd, int btn, int action, int modifiers);

        private:
                
                void onCreateNewWorldSelected();
                void onLoadWorldSelected();
                void onSettingsSelected();
                void onInvalidOptionSelected();
                void onBackToMainMenuSelected();
                void onWorldSelected(Game& game);

                enum class MenuState {
                        MAIN_MENU,                      // Main menu
                        CREATE_WORLD_MENU,              // Mneu used to setup a new game world
                        LOAD_WORLD_MENU,                // Menu used to load a previously saved game world
                        SETTINGS_MENU                   // Menu used to modify game settings
                };

                MenuState       m_currMenuState;

                int             m_userChoice; // TODO: Remove this when a GUI menu will be implemented
                bool            m_renderMenu; // TODO: Remove this when a GUI menu will be implemented
        };
}

#endif // MENU_SCENE_H
