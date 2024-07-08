
#include "renderer.hpp"

namespace mc2d {


        Renderer::Renderer() : m_isInit(false), m_worldVao(0), m_worldVbo(0)
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

                // Set clear color
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

                // Initialize shader, vao and vbo needed to render the blocks in the game world
                if(m_worldShader.init("../resources/worldVrtxShader.vert", "../resources/worldFragShader.frag") != 0)
                {
                        logError("Renderer::init() failed, world shader creation failed!");
                        return 1;
                }

                // Generate world vao and vbo
                glGenVertexArrays(1, &m_worldVao);
                glGenBuffers(1, &m_worldVbo);

                // Bind world vao and vbo
                glBindVertexArray(m_worldVao);
                glBindBuffer(GL_ARRAY_BUFFER, m_worldVbo);

                // Allocate space for world vbo
                glBufferData(GL_ARRAY_BUFFER, Chunk::width * Chunk::height * 12, NULL, GL_DYNAMIC_DRAW);

                // Define format of data in world vbo (so the vertex shader knows how to use such data)
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
                glEnableVertexAttribArray(0);

                // Allocate space to hold the vertices of the blocks that makes the game world
                m_worldVerticesNum = 0;
                m_worldVertices = new float[Chunk::width * Chunk::height * 12];

                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void Renderer::terminate()
        {
                if(!m_isInit)
                        return;

                m_worldVerticesNum = 0;
                delete[] m_worldVertices;

                m_worldShader.terminate();
                glDeleteVertexArrays(1, &m_worldVao);
                glDeleteBuffers(1, &m_worldVbo);

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


        // Renders all the blocks in the given game world that are visible from the given camera
        void Renderer::renderWorld(Chunk& chunk, Camera& camera)
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::render() failed, render has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                       // Activate shader to render the world
                glBindVertexArray(m_worldVao);                  // Bind world vao

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(chunk.hasChanged)
                {
                        computeWorldVertices(chunk, camera);
                        glBufferData(GL_ARRAY_BUFFER, m_worldVerticesNum * sizeof(float), m_worldVertices, GL_DYNAMIC_DRAW);
                        chunk.hasChanged = false;
                }

                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, m_worldVerticesNum);
        }


        // Computes vertices (in NDC coordinate) for all the blocks in the given chunk,
        // for now all chunk's blocks will be rendered on screen (the camera is not being used)
        void Renderer::computeWorldVertices(const Chunk& chunk, const Camera& camera)
        {
                m_worldVerticesNum = 0;

                // Determine block size
                const float blockWidth = 2.0f / (float) Chunk::width;
                const float blockHeight = 2.0f / (float) Chunk::height;

                for(uint32_t y = 0; y < Chunk::height; y++)
                {
                        for(uint32_t x = 0; x < Chunk::width; x++)
                        {
                                // If the current block is solid then we need to compute vertices to render it
                                if(chunk.blocks[ (y * Chunk::width) + x ] == 1)
                                {
                                        // Compute top left vertex coordinates of the block
                                        float xPos = -1.0f + ((float)x * blockWidth);
                                        float yPos = 1.0f - (float) (y * blockWidth);

                                        // First triangle
                                        m_worldVertices[m_worldVerticesNum + 0] = xPos;
                                        m_worldVertices[m_worldVerticesNum + 1] = yPos - blockHeight;

                                        m_worldVertices[m_worldVerticesNum + 2] = xPos + blockWidth;
                                        m_worldVertices[m_worldVerticesNum + 3] = yPos - blockHeight;

                                        m_worldVertices[m_worldVerticesNum + 4] = xPos;
                                        m_worldVertices[m_worldVerticesNum + 5] = yPos;

                                        // Second triangle
                                        m_worldVertices[m_worldVerticesNum + 6] = xPos + blockWidth;
                                        m_worldVertices[m_worldVerticesNum + 7] = yPos - blockHeight;

                                        m_worldVertices[m_worldVerticesNum + 8] = xPos + blockWidth;
                                        m_worldVertices[m_worldVerticesNum + 9] = yPos;

                                        m_worldVertices[m_worldVerticesNum + 10] = xPos;
                                        m_worldVertices[m_worldVerticesNum + 11] = yPos;
                                
                                        m_worldVerticesNum += 12;
                                }
                        }
                }

                logInfo("Computed %u world vertices, %u rectangles", m_worldVerticesNum, m_worldVerticesNum / 12);
        }
      
}

