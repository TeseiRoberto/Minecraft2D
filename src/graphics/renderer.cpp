
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

                // Load the tileset that contains all the game's sprites
                if(!m_gameTileset.load("../resources/blockTileset.png", 16, 14, 16, 16))
                {
                        logError("Renderer::init() failed, game tileset creation failed!");
                        return 1;
                }

                // Initialize shader, vao and vbo needed to render the blocks in the game world
                if(m_worldShader.init("../resources/worldVrtxShader.vert", "../resources/worldFragShader.frag") != 0)
                {
                        logError("Renderer::init() failed, world shader creation failed!");
                        m_gameTileset.unload();
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

                m_worldVerticesNum = 0;
                m_worldVertices = new float[ Chunk::width * Chunk::height * 12 ];
                
                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void Renderer::terminate()
        {
                if(!m_isInit)
                        return;

                m_gameTileset.deactivate();
                m_gameTileset.unload();

                m_worldVerticesNum = 0;
                delete[] m_worldVertices;

                m_worldShader.deactivate();
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
                        logWarn("Renderer::renderWorld() failed, renderer has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                       // Activate shader to render the world
                glBindVertexArray(m_worldVao);                  // Bind world vao
                //m_gameTileset.activate();

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(chunk.hasChanged)
                {
                        computeWorldVertices(chunk, camera);
                        glBufferData(GL_ARRAY_BUFFER, m_worldVerticesNum * sizeof(float), m_worldVertices, GL_DYNAMIC_DRAW);

                        // FIXME: Buffer subdata should be used insetad of glBufferData but its not working and I don't know why...
                        //glBufferSubData(GL_ARRAY_BUFFER, 0, m_worldVerticesNum * sizeof(float), m_worldVertices);
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
                                BlockType currBlock = chunk.blocks[ (y * Chunk::width) + x ];

                                // If the current block is not air then we need to compute vertices to render it
                                if(currBlock != Block::AIR)
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

                logInfo("Basic world rendering computed %u vertices for %u rectangles", m_worldVerticesNum, m_worldVerticesNum / 12);
        }


        // NEW CODE ===============

        void Renderer::optimizedRenderWorld(Chunk& chunk, Camera& camera)
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::optimizedRenderWorld() failed, renderer has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                       // Activate shader to render the world
                glBindVertexArray(m_worldVao);                  // Bind world vao
                //m_gameTileset.activate();

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(chunk.hasChanged)
                {
                        optimizedComputeWorldVertices(chunk, camera);
                        glBufferData(GL_ARRAY_BUFFER, m_worldVerticesNum * sizeof(float), m_worldVertices, GL_DYNAMIC_DRAW);

                        // FIXME: Buffer subdata should be used insetad of glBufferData but its not working and I don't know why...
                        //glBufferSubData(GL_ARRAY_BUFFER, 0, m_worldVerticesNum * sizeof(float), m_worldVertices);
                        chunk.hasChanged = false;
                }

                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, m_worldVerticesNum);
        }


        // Computes the vertices (in NDC coordinates) and the texture coordinates for all the blocks in the given chunk
        // that are visible from the given camera. (uses an horizontal (1D) greedy meshing algorithm to do so)
        void Renderer::optimizedComputeWorldVertices(const Chunk& chunk, const Camera& camera)
        {
                m_worldVerticesNum = 0;

                // Determine block size
                const float blockWidth = 2.0f / (float) Chunk::width;
                const float blockHeight = 2.0f / (float) Chunk::height;

                uint32_t limitX = Chunk::width;
                uint32_t limitY = Chunk::height;

                for(uint32_t y = 0; y < limitY; ++y)
                {
                        for(uint32_t x = 0; x < limitX; ++x)
                        {
                                BlockType startBlock = chunk.blocks[ (y * Chunk::width) + x ];

                                if(startBlock == Block::AIR)     // Skip air blocks
                                        continue;

                                uint32_t endX;
                                for(endX = x; x < limitX; ++endX)
                                {
                                        BlockType endBlock = chunk.blocks[ (y * Chunk::width) + endX ];

                                        if(startBlock != endBlock)
                                                break;
                                }

                                // Compute top left vertex coordinates of the rectangle
                                float xPos = -1.0f + ((float)x * blockWidth);
                                float yPos = 1.0f - (float) (y * blockWidth);

                                float rectWidth = blockWidth * (float) (endX - x);
                                float rectHeight = blockHeight;
                                
                                // First triangle
                                m_worldVertices[m_worldVerticesNum + 0] = xPos;                 // Bottom left x
                                m_worldVertices[m_worldVerticesNum + 1] = yPos - rectHeight;    // Bottom left y

                                m_worldVertices[m_worldVerticesNum + 2] = xPos + rectWidth;     // Bottom right x 
                                m_worldVertices[m_worldVerticesNum + 3] = yPos - rectHeight;    // Bottom right y

                                m_worldVertices[m_worldVerticesNum + 4] = xPos;                 // Top left x
                                m_worldVertices[m_worldVerticesNum + 5] = yPos;                 // Top left y
                                
                                // Second triangle
                                m_worldVertices[m_worldVerticesNum + 6] = xPos + rectWidth;     // Bottom right x
                                m_worldVertices[m_worldVerticesNum + 7] = yPos - rectHeight;    // Bottom right y
                                
                                m_worldVertices[m_worldVerticesNum + 8] = xPos + rectWidth;     // Top right x
                                m_worldVertices[m_worldVerticesNum + 9] = yPos;                 // Top right y
                                
                                m_worldVertices[m_worldVerticesNum + 10] = xPos;                // Top left x
                                m_worldVertices[m_worldVerticesNum + 11] = yPos;                // Top left y
                                
                                m_worldVerticesNum += 12;

                                x = endX - 1;
                        }
                }

                logInfo("Optimized world rendering computed %u vertices for %u rectangles", m_worldVerticesNum, m_worldVerticesNum / 12);
        }      
}



