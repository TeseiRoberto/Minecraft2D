
// Contains definition of the MenuScene class, this class implements the game menu behaviour.

#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <memory>

#include "logging.hpp"
#include "game.hpp"
#include "scene.hpp"
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
        };
}

#endif // MENU_SCENE_H
