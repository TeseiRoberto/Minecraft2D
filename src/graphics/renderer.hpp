
// Contans definition of the renderer class, this class is responsible for drawing blocks and game objects

#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <glad/glad.h>

#include "logging.hpp"
#include "shader.hpp"

namespace mc2d {


        class Renderer
        {
        public:
                Renderer();
                ~Renderer();

                int     init();
                void    terminate();
                void    resizeViewport(int newWidth, int newHeight);

                void    render();

        private:

                bool            m_isInit;
                uint32_t        m_vao;
                uint32_t        m_vbo;
                uint32_t        m_ibo;
                Shader          m_mainShader;
        };
}

#endif // RENDERER_H
