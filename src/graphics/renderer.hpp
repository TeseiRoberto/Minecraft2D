
// Contans definition of the renderer class, this class is responsible for drawing blocks and game objects

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <glad/glad.h>

#include "logging.hpp"
#include "world/gameWorld.hpp"
#include "shader.hpp"
#include "tileset.hpp"
#include "sprite.hpp"
#include "camera.hpp"

namespace mc2d {


        class Renderer
        {
        public:
                Renderer();
                ~Renderer();

                // Delete copy constructors
                Renderer(Renderer& other) = delete;
                Renderer(const Renderer& other) = delete;
                Renderer operator = (Renderer& other) = delete;
                Renderer operator = (const Renderer& other) = delete;

                int     init();
                void    terminate();
                void    resizeViewport(int newWidth, int newHeight);

                void    clearScreen();

                void    renderWorld(GameWorld& world, Camera& camera, bool optimized);

                void    renderSprite(const Sprite& sprite, const glm::vec3& pos, const glm::vec3& scale, const float rotation, const Camera& camera);
                void    renderSprite(const Sprite& sprite, const glm::mat4& modelMat, const glm::mat4& viewMat, const glm::mat4& projectionMat);

        private:

                int     initWorldRenderingData();
                void    terminateWorldRenderingData();

                int     initSpriteRenderingData();
                void    terminateSpriteRenderingData();


                bool            m_isInit;

                // Data needed to render the blocks in the game world
                Tileset         m_blocksTileset;
                size_t          m_worldVerticesBufferSize;
                size_t          m_worldVerticesNum;
                float*          m_worldVertices;
                
                uint32_t        m_worldVao;
                uint32_t        m_worldVbo;
                Shader          m_worldShader;

                // Data needed to render sprites
                size_t          m_spriteVerticesNum;
                uint32_t        m_spriteVao;
                uint32_t        m_spriteVbo;
                Shader          m_spriteShader;
        };
}

#endif // RENDERER_H
