
// Contains definition of the GameScene class, this class implements the ingame behaviour.

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <ctime>
#include <memory>

#include "log.hpp"
#include "game.hpp"
#include "scene.hpp"
#include "world/gameWorld.hpp"
#include "world/worldGenerator.hpp"
#include "graphics/renderer.hpp"
#include "graphics/worldRenderer.hpp"
#include "graphics/camera.hpp"
#include "graphics/sprite.hpp"

// keep this include order (renderer.hpp includes glad.h which must be included before glfw)
#include <GLFW/glfw3.h>

namespace mc2d {


        class GameScene : public Scene {
        public:
                GameScene(GameWorld&& gameWorld);
                virtual ~GameScene();

                virtual int     init();
                virtual void    terminate();

                virtual void    update(Game& game, float deltaTime);
                virtual void    render(Game& game, Renderer& renderer);

                virtual void    onKeyEvent(Game& game, GLFWwindow* wnd, int key, int scancode, int action, int mods);
                virtual void    onMouseButtonEvent(Game& game, GLFWwindow* wnd, int btn, int action, int modifiers);

        private:
                void            printHelp() const;

                Sprite          m_playerSprite;         // TODO: move this somewhere else ???
                
                WorldRenderer   m_worldRenderer;
                Camera          m_playerCamera;         // Camera focused on player
                GameWorld       m_gameWorld;
        
                size_t          m_currPlayerId;         // Indicates which player in the game world we are currently controlling
                bool            m_optimizedDraw;        // TODO: remove me when testing is over
                BlockType       m_cursorBlockType;      // Which type of block will be placed in the world when right mouse is clicked
        };
}

#endif // GAME_SCENE_H
