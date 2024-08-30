
// Contains definition of the Scene interface.
//
// The Scene interface is used to abstract a "phase" of the game, in particular the classes that
// implement this interface will define the way in which the player/user can interact with
// the game and the way in which the game will be updated and rendered each frame.
//
// In this way we can switch between different game phases ,for example menu and in-game, by simply
// changing the current scene in the Game class.
//

#ifndef SCENE_H
#define SCENE_H

struct GLFWwindow;

namespace mc2d {

        class Game;
        class Renderer;

        class Scene {
        public:
                Scene() : m_isInit(false)       {}
                virtual ~Scene() = default;

                virtual int     init() = 0;
                virtual void    terminate() = 0;

                inline bool     isInit() const  { return m_isInit; }

                virtual void    update(Game& game, float deltaTime) = 0;
                virtual void    render(Game& game, Renderer& renderer) = 0;

                virtual void    onKeyEvent(Game& game, GLFWwindow* wnd, int key, int scancode, int action, int mods) = 0;
                virtual void    onMouseButtonEvent(Game& game, GLFWwindow* wnd, int btn, int action, int modifiers) = 0;

        protected:
                bool    m_isInit;
        };
}

#endif // SCENE_H
