
#include "renderer.hpp"

namespace mc2d {


        Renderer::Renderer() : m_isInit(false), m_worldVao(0), m_worldVbo(0), m_worldVerticesBufferSize(Chunk::width * Chunk::height * 30)
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
                if(!m_gameTileset.load("../resources/myTileset.png", 5, 16, 16, 16))
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
                glBufferData(GL_ARRAY_BUFFER, Chunk::width * Chunk::height * 30, NULL, GL_DYNAMIC_DRAW);

                // Define format of data in world vbo (so the vertex shader knows how to use such data)
                // Vertex's X and y coordinates
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
                glEnableVertexAttribArray(0);

                // Vertex's UV coordinates
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)) );
                glEnableVertexAttribArray(1);

                // Vertex's tile id (id of the texture in the tileset's texture array)
                glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (4 * sizeof(float)) );
                glEnableVertexAttribArray(2);

                // Allocate a memory buffer to compute world data
                m_worldVerticesNum = 0;
                m_worldVertices = new float[m_worldVerticesBufferSize];
                
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

                m_worldShader.deactivate();
                m_worldShader.terminate();

                glDeleteVertexArrays(1, &m_worldVao);
                glDeleteBuffers(1, &m_worldVbo);

                m_worldVerticesNum = 0;
                delete[] m_worldVertices;

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
        void Renderer::renderWorld(GameWorld& world, Camera& camera, bool optimized)
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::renderWorld() failed, renderer has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                                               // Activate shader to render the world
                
                // Compute view-projection matrix and set uniform
                glm::mat4 vpMatrix = glm::ortho(0.0f, (float) camera.getWidth(), 0.0f, (float) camera.getHeight()) * camera.getViewMatrix();
                int viewMatUniform = m_worldShader.getUniformId("transformMatrix");
                m_worldShader.setUniform(viewMatUniform, vpMatrix);

                glBindVertexArray(m_worldVao);                                          // Bind world vao
                m_gameTileset.activate();                                               // Bind tileset's texture

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(world.hasChanged() || camera.hasChanged())
                {
                        if(optimized)
                                camera.optimizedComputeVisibleBlocksVertices(world, m_worldVertices, m_worldVerticesBufferSize, &m_worldVerticesNum);
                        else
                                camera.computeVisibleBlocksVertices(world, m_worldVertices, m_worldVerticesBufferSize, &m_worldVerticesNum);

                        // FIXME: Buffer subdata should be used insetad of glBufferData but its not working and I don't know why...
                        //glBufferSubData(GL_ARRAY_BUFFER, 0, m_worldVerticesNum * sizeof(float), m_worldVertices);
                        glBufferData(GL_ARRAY_BUFFER, m_worldVerticesNum * sizeof(float), m_worldVertices, GL_DYNAMIC_DRAW);

                        world.setHasChanged(false);
                        camera.setHasChanged(false);
                }

                glClear(GL_COLOR_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLES, 0, m_worldVerticesNum);
        }


        // Computes vertices (in world coordinates) and texture coordinates for all the blocks in the given chunk,
        // for now all chunk's blocks will be rendered on screen (the camera is not being used)
        void Renderer::computeWorldVertices(const GameWorld& world, const Camera& camera)
        {
                m_worldVerticesNum = 0;
                uint32_t blockIndex = 0;

                for(uint32_t y = Chunk::height - 1; y != -1; --y)
                {
                        for(uint32_t x = 0; x < Chunk::width; ++x)
                        {
                                BlockType currBlock = world.getPlayerChunk().blocks[blockIndex++];

                                // If the current block is not air then we need to compute vertices to render it
                                if(currBlock != BlockType::AIR)
                                {
                                        // Compute bottom left vertex coordinates of the block
                                        float xPos = (float) x * BLOCK_WIDTH;
                                        float yPos = (float) y * BLOCK_HEIGHT;

                                        // First triangle bottom left vertex
                                        m_worldVertices[m_worldVerticesNum + 0] = xPos;                 // x
                                        m_worldVertices[m_worldVerticesNum + 1] = yPos;                 // y
                                        m_worldVertices[m_worldVerticesNum + 2] = 0.0f;                 // u
                                        m_worldVertices[m_worldVerticesNum + 3] = 0.0f;                 // v
                                        m_worldVertices[m_worldVerticesNum + 4] = (float) currBlock;    // tile id

                                        // First triangle bottom right vertex
                                        m_worldVertices[m_worldVerticesNum + 5] = xPos + BLOCK_WIDTH;   // x
                                        m_worldVertices[m_worldVerticesNum + 6] = yPos;                 // y
                                        m_worldVertices[m_worldVerticesNum + 7] = 1.0f;                 // u
                                        m_worldVertices[m_worldVerticesNum + 8] = 0.0f;                 // v
                                        m_worldVertices[m_worldVerticesNum + 9] = (float) currBlock;    // tile id

                                        // First triangle top left vertex
                                        m_worldVertices[m_worldVerticesNum + 10] = xPos;                // x
                                        m_worldVertices[m_worldVerticesNum + 11] = yPos + BLOCK_HEIGHT; // y
                                        m_worldVertices[m_worldVerticesNum + 12] = 0.0f;                // u
                                        m_worldVertices[m_worldVerticesNum + 13] = 1.0f;                // v
                                        m_worldVertices[m_worldVerticesNum + 14] = (float) currBlock;   // tile id
                                        
                                        // Second triangle bottom right vertex
                                        m_worldVertices[m_worldVerticesNum + 15] = xPos + BLOCK_WIDTH;  // x
                                        m_worldVertices[m_worldVerticesNum + 16] = yPos;                // y
                                        m_worldVertices[m_worldVerticesNum + 17] = 1.0f;                // u
                                        m_worldVertices[m_worldVerticesNum + 18] = 0.0f;                // v
                                        m_worldVertices[m_worldVerticesNum + 19] = (float) currBlock;   // tile id
                                        
                                        // Second triangle top right vertex
                                        m_worldVertices[m_worldVerticesNum + 20] = xPos + BLOCK_WIDTH;  // x
                                        m_worldVertices[m_worldVerticesNum + 21] = yPos + BLOCK_HEIGHT; // y
                                        m_worldVertices[m_worldVerticesNum + 22] = 1.0f;                // u
                                        m_worldVertices[m_worldVerticesNum + 23] = 1.0f;                // v
                                        m_worldVertices[m_worldVerticesNum + 24] = (float) currBlock;   // tile id
                                        
                                        // Second triangle top left vertex
                                        m_worldVertices[m_worldVerticesNum + 25] = xPos;                // x
                                        m_worldVertices[m_worldVerticesNum + 26] = yPos + BLOCK_HEIGHT; // y
                                        m_worldVertices[m_worldVerticesNum + 27] = 0.0f;                // u
                                        m_worldVertices[m_worldVerticesNum + 28] = 1.0f;                // v
                                        m_worldVertices[m_worldVerticesNum + 29] = (float) currBlock;   // tile id
  
                                        m_worldVerticesNum += 30;
                                }
                        }
                }

                logInfo("Basic world rendering computed %u vertices for %u rectangles", m_worldVerticesNum, m_worldVerticesNum / 30);
        }


        // Computes the vertices (in world coordinates) and the texture coordinates for all the blocks in the given chunk
        // that are visible from the given camera. (uses an horizontal (1D) greedy meshing algorithm to do so)
        void Renderer::optimizedComputeWorldVertices(const GameWorld& world, const Camera& camera)
        {
                m_worldVerticesNum = 0;
                uint32_t startBlockIndex = 0;

                for(uint32_t y = Chunk::height - 1; y != -1; --y)
                {
                        for(uint32_t x = 0; x < Chunk::width; ++x)
                        {
                                BlockType startBlock = world.getPlayerChunk().blocks[startBlockIndex++];

                                if(startBlock == BlockType::AIR)     // Skip air blocks
                                        continue;

                                uint32_t endX;
                                uint32_t endBlockIndex = startBlockIndex - 1;
                                for(endX = x; endX < Chunk::width; ++endX, ++endBlockIndex)
                                {
                                        BlockType endBlock = world.getPlayerChunk().blocks[endBlockIndex];

                                        if(startBlock != endBlock)
                                                break;
                                }

                                // Compute bottom left vertex coordinates of the rectangle
                                float xPos = (float) x * BLOCK_WIDTH;
                                float yPos = (float) y * BLOCK_HEIGHT;

                                float rectWidth = BLOCK_WIDTH * (float) (endX - x);
                                float rectHeight = BLOCK_HEIGHT;                                // We are using 1D greedy meshing for now (2D would be a lot better)...
                               
                                // First triangle bottom left vertex
                                m_worldVertices[m_worldVerticesNum + 0] = xPos;                 // x
                                m_worldVertices[m_worldVerticesNum + 1] = yPos;                 // y
                                m_worldVertices[m_worldVerticesNum + 2] = 0.0f;                 // u
                                m_worldVertices[m_worldVerticesNum + 3] = 0.0f;                 // v
                                m_worldVertices[m_worldVerticesNum + 4] = (float) startBlock;   // tile id

                                // First triangle bottom right vertex
                                m_worldVertices[m_worldVerticesNum + 5] = xPos + rectWidth;     // x 
                                m_worldVertices[m_worldVerticesNum + 6] = yPos;                 // y
                                m_worldVertices[m_worldVerticesNum + 7] = (float) (endX - x);   // u
                                m_worldVertices[m_worldVerticesNum + 8] = 0.0f;                 // v
                                m_worldVertices[m_worldVerticesNum + 9] = (float) startBlock;   // tile id

                                // First triangle top left vertex
                                m_worldVertices[m_worldVerticesNum + 10] = xPos;                // x
                                m_worldVertices[m_worldVerticesNum + 11] = yPos + rectHeight;   // y
                                m_worldVertices[m_worldVerticesNum + 12] = 0.0f;                // u
                                m_worldVertices[m_worldVerticesNum + 13] = 1.0f;                // v
                                m_worldVertices[m_worldVerticesNum + 14] = (float) startBlock;  // tile id
                                
                                // Second triangle bottom right vertex
                                m_worldVertices[m_worldVerticesNum + 15] = xPos + rectWidth;    // x
                                m_worldVertices[m_worldVerticesNum + 16] = yPos;                // y
                                m_worldVertices[m_worldVerticesNum + 17] = (float) (endX - x);  // u
                                m_worldVertices[m_worldVerticesNum + 18] = 0.0f;                // v
                                m_worldVertices[m_worldVerticesNum + 19] = (float) startBlock;  // tile id
                                
                                // Second triangle top right vertex
                                m_worldVertices[m_worldVerticesNum + 20] = xPos + rectWidth;    // x
                                m_worldVertices[m_worldVerticesNum + 21] = yPos + rectHeight;   // y
                                m_worldVertices[m_worldVerticesNum + 22] = (float) (endX - x);  // u
                                m_worldVertices[m_worldVerticesNum + 23] = 1.0f;                // v
                                m_worldVertices[m_worldVerticesNum + 24] = (float) startBlock;  // tile id
                                
                                // Second triangle top left vertex
                                m_worldVertices[m_worldVerticesNum + 25] = xPos;                // x
                                m_worldVertices[m_worldVerticesNum + 26] = yPos + rectHeight;   // y
                                m_worldVertices[m_worldVerticesNum + 27] = 0.0f;                // u
                                m_worldVertices[m_worldVerticesNum + 28] = 1.0f;                // v
                                m_worldVertices[m_worldVerticesNum + 29] = (float) startBlock;  // tile id
                                
                                m_worldVerticesNum += 30;
                                startBlockIndex += endBlockIndex - startBlockIndex;
                                x = endX - 1;
                        }
                }

                logInfo("Optimized world rendering computed %u vertices for %u rectangles", m_worldVerticesNum, m_worldVerticesNum / 30);
        }      
}



