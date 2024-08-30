
// Contans definition of the renderer class, this class is responsible for drawing blocks and game objects

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <glad/glad.h>

#include "logging.hpp"
#include "shader.hpp"
#include "sprite.hpp"
#include "camera.hpp"

namespace mc2d {

        class GameWorld;

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

                int             init();
                void            terminate();
                inline bool     isInit() const          { return m_isInit; }

                void            resizeViewport(int newWidth, int newHeight);
                void            clearScreen();

                void            renderSprite(const Sprite& sprite, const glm::vec3& pos, const glm::vec3& scale, const float rotation, const Camera& camera);
                void            renderSprite(const Sprite& sprite, const glm::mat4& modelMat, const glm::mat4& viewMat, const glm::mat4& projectionMat);

        private:

                int             initSpriteRenderingData();
                void            terminateSpriteRenderingData();

                bool            m_isInit;

                // Data needed to render sprites
                uint32_t        m_spriteVao;
                uint32_t        m_spriteVbo;
                Shader          m_spriteShader;
        };
}

#endif // RENDERER_H
