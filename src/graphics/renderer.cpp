
#include "renderer.hpp"

namespace mc2d {


        Renderer::Renderer() : m_isInit(false), m_vao(0), m_vbo(0), m_ibo(0)
        {}


        // Terminates the renderer (if not done yet)
        Renderer::~Renderer()
        {
                terminate();
        }


        // Attempts to initialize all the resources needed by the renderer
        // @returns: zero on success, non zero on failure
        int Renderer::init()
        {
                if(m_isInit)
                {
                        logWarn("Renderer::init() failed, renderer has already been initialized!");
                        return 1;
                }

                // TODO: for now we are hardcoding vao, vbo, ibo and shader creation just for debug purpouses
        
                // Initialize shader
                if(m_mainShader.init("../resources/vrtxShader.vert", "../resources/fragShader.frag") != 0)
                {
                        logError("Renderer::init() failed, main shader creation failed!");
                        return 1;
                }

                float quadVertices[] = {
                        -0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f,
                        0.5f, 0.5f, 0.0f,
                        -0.5f, 0.5f, 0.0f,
                };

                uint32_t quadIndexes[] = {
                        0, 1, 3,
                        1, 2, 3
                };

                // Set clear color
                glClearColor(0.6f, 0.4f, 0.4f, 1.0f);

                // Generate vao, vbo and ibo
                glGenVertexArrays(1, &m_vao);
                glGenBuffers(1, &m_vbo);
                glGenBuffers(1, &m_ibo);

                // Bind vao, vbo and ibo
                glBindVertexArray(m_vao);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

                // Insert data into vbo and ibo
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndexes), quadIndexes, GL_STATIC_DRAW);

                // Define format of data in vbo (so the vertex shader knows how to use such data)
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
                glEnableVertexAttribArray(0);

                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void Renderer::terminate()
        {
                if(!m_isInit)
                        return;

                m_mainShader.terminate();
                glDeleteVertexArrays(1, &m_vao);
                glDeleteBuffers(1, &m_vbo);
                glDeleteBuffers(1, &m_ibo);

                m_isInit = false;
        }


        // Resizes the viewport
        // @newWidth: the new width of the viewport
        // @newHeight: the new height of the viewport
        void Renderer::resizeViewport(int newWidth, int newHeight)
        {
                if(!m_isInit)
                        return;

                glViewport(0, 0, newWidth, newHeight);
        }


        void Renderer::render()
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::render() failed, render has not been initialized correctly!");
                        return;
                }

                // TODO: for now we simply draw a rectangle

                glClear(GL_COLOR_BUFFER_BIT);
                m_mainShader.activate();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
}

