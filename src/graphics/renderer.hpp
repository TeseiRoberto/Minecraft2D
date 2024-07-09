
// Contans definition of the renderer class, this class is responsible for drawing blocks and game objects

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <glad/glad.h>

#include "logging.hpp"
#include "block.hpp"
#include "chunk.hpp"
#include "shader.hpp"
#include "tileset.hpp"
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

                void    renderWorld(Chunk& chunk, Camera& camera);

                void    optimizedRenderWorld(Chunk& chunk, Camera& camera);
                void    optimizedComputeWorldVertices(const Chunk& chunk, const Camera& camera);

        private:

                void            computeWorldVertices(const Chunk& chunk, const Camera& camera);

                bool            m_isInit;
                Tileset         m_gameTileset;

                // Data needed to render the blocks in the game world
                uint32_t        m_worldVerticesNum;
                float*          m_worldVertices;
                
                uint32_t        m_worldVao;
                uint32_t        m_worldVbo;
                Shader          m_worldShader;
        };
}

#endif // RENDERER_H
