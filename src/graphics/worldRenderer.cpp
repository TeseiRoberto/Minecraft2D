
#include "worldRenderer.hpp"

namespace mc2d {


        WorldRenderer::WorldRenderer() : m_isInit(false),
                m_maxBlocksInBatch(0), m_currBlocksInBatch(0), m_blocksVertices(nullptr),
                m_worldVao(0), m_worldVbo(0)
        {}


        // Terminates the renderer (if not done yet)
        WorldRenderer::~WorldRenderer()
        {
                terminate();
        }


        // Attempts to initialize all the resources needed by the world renderer
        // @maxBlocksInBatch: the maximum amount of blocks that can be rendered in a single draw call
        // @returns: zero on success, non zero on failure
        int WorldRenderer::init(size_t maxBlocksInBatch)
        {
                // Load the tileset that contains all the game's blocks
                if(!m_blocksTileset.load("../resources/textures/myTileset.png", 5, 16, 16, 16))
                {
                        logError("Renderer::initWorldRenderingData() failed, game tileset creation failed!");
                        return 1;
                }

                // Initialize shader, vao and vbo needed to render the blocks in the game world
                if(m_worldShader.init("../resources/worldVrtxShader.vert", "../resources/worldFragShader.frag") != 0)
                {
                        logError("Renderer::initWorldRenderingData() failed, world shader creation failed!");
                        m_blocksTileset.unload();
                        return 1;
                }

                // Generate vao and vbo to render world's blocks
                glGenVertexArrays(1, &m_worldVao);
                glBindVertexArray(m_worldVao);

                glGenBuffers(1, &m_worldVbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_worldVbo);

                // Allocate space for world vbo
                glBufferData(GL_ARRAY_BUFFER, maxBlocksInBatch * 6 * sizeof(BlockVertex), NULL, GL_DYNAMIC_DRAW);

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

                // Allocate a memory buffer to hold blocks vertices data
                m_maxBlocksInBatch = maxBlocksInBatch;
                m_currBlocksInBatch = 0;

                m_blocksVertices = new BlockVertex[m_maxBlocksInBatch * 6];

                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void WorldRenderer::terminate()
        {
                if(!m_isInit)
                        return;

                if(m_worldShader.isInit())
                {
                        m_worldShader.deactivate();
                        m_worldShader.terminate();
                }

                if(m_blocksTileset.isInit())
                {
                        m_blocksTileset.deactivate();
                        m_blocksTileset.unload();
                }

                if(m_worldVao != 0)
                {
                        glDeleteVertexArrays(1, &m_worldVao);
                        m_worldVao = 0;

                        glDeleteBuffers(1, &m_worldVbo);
                        m_worldVbo = 0;
                }

                m_maxBlocksInBatch = 0;
                m_currBlocksInBatch = 0;
                delete[] m_blocksVertices;

                m_isInit = false;
        }


        // Renders all the blocks in the given game world that are visible from the given camera
        void WorldRenderer::render(GameWorld& world, Camera& camera, bool optimized)
        {
                if(!m_isInit)
                {
                        logWarn("WorldRenderer::render() failed, world renderer has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                                               // Activate shader to render the world
                m_blocksTileset.activate();                                             // Bind tileset's texture
                
                // Compute view-projection matrix and set uniform
                glm::mat4 vpMatrix = glm::ortho(0.0f, (float) camera.getWidth(), 0.0f, (float) camera.getHeight()) * camera.getViewMatrix();
                int viewMatUniform = m_worldShader.getUniformId("transformMatrix");
                m_worldShader.setUniform(viewMatUniform, vpMatrix);

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(world.hasChanged() || camera.hasChanged())
                {
                        size_t verticesNum = 0;
                        if(optimized)
                        {
                                m_currBlocksInBatch = optimizedComputeVisibleBlocksVertices(world, camera, m_blocksVertices, m_maxBlocksInBatch * 6, verticesNum);
                                logInfo("optimizedComputeVisibleBlocksVertices computed %u vertices for %u blocks (%u triangles)", verticesNum, m_currBlocksInBatch, m_currBlocksInBatch / 2);
                        } else {
                                m_currBlocksInBatch = computeVisibleBlocksVertices(world, camera, m_blocksVertices, m_maxBlocksInBatch * 6, verticesNum);
                                logInfo("computeVisibleBlocksVertices computed %u vertices for %u blocks (%u triangles)", verticesNum, m_currBlocksInBatch, m_currBlocksInBatch / 2);
                        }

                        glBindBuffer(GL_ARRAY_BUFFER, m_worldVbo);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, m_currBlocksInBatch * 6 * sizeof(BlockVertex), m_blocksVertices);

                        world.setHasChanged(false);
                        camera.setHasChanged(false);
                }

                // Render a batch of blocks
                glBindVertexArray(m_worldVao);
                glDrawArrays(GL_TRIANGLES, 0, m_currBlocksInBatch * 6);
        }


        // Computes the vertices (in world coordinates) and the texture coordinates for all the blocks in
        // the given world that are visible from the given camera (and that are in an already loaded chunk)
        // @world: the world of which blocks will be considered
        // @camera: the point from which the world is looked at
        // @vertices: memory buffer in which the computed vertices will be stored
        // @maxVerticesNum: maximum amount of elements that can be stored in the given buffer
        // @verticesNum: variable in which the number of vertices computed will be written
        // @returns: number of blocks for which vertices have been computed (number of blocks visible)
        size_t WorldRenderer::computeVisibleBlocksVertices(const GameWorld& world, const Camera& camera, BlockVertex* vertices, const size_t maxVerticesNum, size_t& verticesNum)
        {
                if(vertices == nullptr || maxVerticesNum == 0)
                {
                        logError("WorldRenderer::computeVisibleBlocksVertices() failed, cannot store vertices "
                                        "in the given buffer because such buffer is nullptr or its size is zero!");
                        return 0;
                }

                verticesNum = 0;
                size_t vertexIndex = 0;                                         // Counter for the elements inserted in the vertices buffer
                size_t blocksNum = 0;                                           // Counter to keep track of the blocks for which vertices have been generated

                // Step 1] Determine all the chunks that are covered by the camera
                std::vector<Chunk const*> intersectedChunks = camera.getVisibleChunks(world);
                if(intersectedChunks.size() == 0)
                        return 0;

                glm::vec<2, size_t> initialIndex(0, 0);                         // X and Y indexes of the first block visible from the camera (relative to the blocks array of the first chunk)
                glm::vec2 initialPos(0.0f, 0.0f);                               // X and y coordinates (in world space) of the top left vertex of the first block that is visible from the camera

                // Step 2] Determine the indexes and the world position of the first block that is visible from the camera
                if(camera.getPos().x <= intersectedChunks[0]->getPos().x)       // Camera origin is at the left of the chunk origin
                {
                        initialIndex.x = 0;
                        initialPos.x = intersectedChunks[0]->getPos().x;
                } else {
                        initialIndex.x = std::floor(camera.getPos().x - intersectedChunks[0]->getPos().x);
                        initialPos.x = intersectedChunks[0]->getPos().x + ( (float) initialIndex.x * BLOCK_WIDTH);
                }

                if(camera.getPos().y < intersectedChunks[0]->getPos().y)        // Camera origin is below the chunk origin
                {
                        initialIndex.y = std::floor(intersectedChunks[0]->getPos().y - camera.getPos().y);
                        initialPos.y = intersectedChunks[0]->getPos().y - ( (float) initialIndex.y * BLOCK_HEIGHT);
                } else {
                        initialIndex.y = 0;
                        initialPos.y = intersectedChunks[0]->getPos().y;
                }

                glm::vec<2, size_t> currIndex = initialIndex;                   // X and Y indexes of the current block (relative to the blocks array of the current chunk)
                glm::vec2 currPos = initialPos;                                 // X and y coordinates (in world space) of the top left vertex of the current block
                
                // Step 3] Iterate over the region of space that is visible from the camera (+ 1 block of safety)
                for(size_t i = 0; i <= camera.getHeight(); ++i, currPos.y -= BLOCK_HEIGHT)
                {
                        auto currChunk = intersectedChunks.begin();

                        if(currPos.y > (*currChunk)->getPos().y)                // If we are on top of the chunk then there is no block to analyze so skip this line
                                continue;
                        else if(currPos.y <= 0)                                 // If we are below the chunk then there's no more blocks to render, our job is done
                                break;

                        // Reset values for the x axis
                        currIndex.x = initialIndex.x;
                        currPos.x = initialPos.x;

                        for(size_t j = 0; j <= camera.getWidth(); ++j, currPos.x += BLOCK_WIDTH)
                        {
                                if(currIndex.x == Chunk::width)                 // Handle passage to the adjacent chunk
                                {
                                        ++currChunk;
                                        if(currChunk == intersectedChunks.end())
                                                break;

                                        currIndex.x = 0;
                                }

                                // Step 4] generate vertices for all blocks that are not air
                                BlockType currBlock = (*currChunk)->blocks[(currIndex.y * Chunk::width) + currIndex.x];
                                if(currBlock == BlockType::AIR)
                                {
                                        ++currIndex.x;
                                        continue;
                                }

                                if(!generateBlockVertices(vertices, vertexIndex, maxVerticesNum, currPos.x, currPos.y, currPos.x + BLOCK_WIDTH, currPos.y - BLOCK_HEIGHT, currBlock))
                                {
                                        logError("GameWorld::optimizedComputeVisibleBlocksVertices() failed, cannot store all vertices in the given buffer,"
                                                        "the number of vertices of the visible blocks is greater than the given buffer size");

                                        verticesNum = vertexIndex;
                                        return blocksNum;
                                }

                                ++blocksNum;
                                ++currIndex.x;
                        }
                        ++currIndex.y;
                }

                verticesNum = vertexIndex;
                return blocksNum;
        }


        // Computes the vertices (in world coordinates) and the texture coordinates for all the blocks in
        // the given world that are visible from the given camera (and that are in an already loaded chunk), to do
        // so it uses a 1D greedy meshing algorithm that composes adjacent block of the same type in one single rectangle.
        // @world: the world of which blocks will be considered
        // @camera: the point from which the world is looked at
        // @vertices: memory buffer in which the computed vertices will be stored
        // @maxVerticesNum: maximum amount of floats that can be stored in the given buffer
        // @verticesNum: variable in which the number of vertices computed will be written
        // @returns: number of blocks for which vertices have been computed (number of blocks visible)
        size_t WorldRenderer::optimizedComputeVisibleBlocksVertices(const GameWorld& world, const Camera& camera, BlockVertex* vertices, const size_t maxVerticesNum, size_t& verticesNum)
        {
                if(vertices == nullptr || maxVerticesNum == 0)
                {
                        logError("Camera::optimizedComputeVisibleBlocksVertices() failed, cannot store vertices "
                                        "in the given buffer because such buffer is nullptr or its size is zero!");
                        return 0;
                }
        
                verticesNum = 0;
                size_t vertexIndex = 0;                                         // Counter for the elements inserted in the vertices buffer
                size_t blocksNum = 0;                                           // Counter to keep track of the blocks for which vertices have been generated

                // Step 1] Determine all the chunks that are covered by the camera
                std::vector<Chunk const*> intersectedChunks = camera.getVisibleChunks(world);
                if(intersectedChunks.size() == 0)
                        return 0;

                glm::vec<2, size_t> initialIndex(0, 0);                         // X and Y indexes of the first block visible from the camera (relative to the blocks array of the first chunk)
                glm::vec2 initialPos(0.0f, 0.0f);                               // X and y coordinates (in world space) of the top left vertex of the first block that is visible from the camera

                // Step 2] Determine the indexes and the world position of the first block that is visible from the camera
                if(camera.getPos().x <= intersectedChunks[0]->getPos().x)         // Camera origin is at the left of the chunk origin
                {
                        initialIndex.x = 0;
                        initialPos.x = intersectedChunks[0]->getPos().x;
                } else {
                        initialIndex.x = std::floor(camera.getPos().x - intersectedChunks[0]->getPos().x);
                        initialPos.x = intersectedChunks[0]->getPos().x + ( (float) initialIndex.x * BLOCK_WIDTH);
                }

                if(camera.getPos().y < intersectedChunks[0]->getPos().y)          // Camera origin is below the chunk origin
                {
                        initialIndex.y = std::floor(intersectedChunks[0]->getPos().y - camera.getPos().y);
                        initialPos.y = intersectedChunks[0]->getPos().y - ( (float) initialIndex.y * BLOCK_HEIGHT);
                } else {
                        initialIndex.y = 0;
                        initialPos.y = intersectedChunks[0]->getPos().y;
                }

                glm::vec<2, size_t> currIndex = initialIndex;           // X and Y indexes of the current block (relative to the blocks array of the current chunk)
                glm::vec2 currPos = initialPos;                         // X and y coordinates (in world space) of the top left vertex of the current block
                
                // Step 3] Iterate over the region of space that is visible from the camera (+ 1 block of safety)
                for(size_t i = 0; i <= camera.getHeight(); ++i, currPos.y -= BLOCK_HEIGHT)
                {
                        auto currChunk = intersectedChunks.begin();

                        if(currPos.y > (*currChunk)->getPos().y)        // If we are on top of the chunk then there is no block to analyze so skip this line
                                continue;
                        else if(currPos.y <= 0)                         // If we are below the chunk then there's no more blocks to render, our job is done
                                break;

                        // Reset values for the x axis
                        currIndex.x = initialIndex.x;
                        currPos.x = initialPos.x;

                        for(size_t j = 0; j <= camera.getWidth(); ++j)
                        {
                                size_t k;
                                float firstBlockPosX = currPos.x;
                                BlockType firstBlock = (*currChunk)->blocks[(currIndex.y * Chunk::width) + currIndex.x];
                                size_t counter = 0;

                                // Step 4] Use greedy meshing to compose adjacent blocks of the same type into one single rectangle
                                for(k = j; k <= camera.getWidth(); ++k, currPos.x += BLOCK_WIDTH, ++currIndex.x)
                                {
                                        if(currIndex.x == Chunk::width)         // Handle passage to the adjacent chunk
                                        {
                                                ++currChunk;
                                                if(currChunk == intersectedChunks.end())
                                                {
                                                        k = SIZE_MAX - 1;       // This makes sure that we will exit the outer loop too
                                                        break;
                                                }

                                                currIndex.x = 0;
                                        }

                                        BlockType currBlock = (*currChunk)->blocks[(currIndex.y * Chunk::width) + currIndex.x];
                                        if(currBlock != firstBlock)
                                                break;

                                        ++counter;
                                }

                                // Step 5] Generate vertices for the rectangle resulting from the application of greedy meshing
                                if(firstBlock != BlockType::AIR)
                                {
                                        if(!generateBlockVertices(vertices, vertexIndex, maxVerticesNum,
                                                                firstBlockPosX, currPos.y, currPos.x, currPos.y - BLOCK_HEIGHT, firstBlock))
                                        {
                                                logError("WorldRenderer::optimizedComputeVisibleBlocksVertices() failed, cannot store all vertices in the given buffer,"
                                                                "the number of vertices of the visible blocks is greater than the given buffer size");

                                                verticesNum = vertexIndex;
                                                return blocksNum;
                                        }

                                        ++blocksNum;
                                }

                                j = k - 1;
                        }

                        ++currIndex.y;
                }

                verticesNum = vertexIndex;
                return blocksNum;
        }


        // Utility function that generates vertices for blocks in a way such that the blocks generated will fill the given area
        // @vertices: memory buffer in which the computed vertices will be stored
        // @index: specifies the starting point in the buffer 
        // @maxVerticesNum: maximum amount of floats that can be stored in the given buffer
        // @startX, startY: x and y coordinates in world space of the top left corner of the area to be filled
        // @endX, endY: x and y coordinates in world space of the bottom right corner of the area to be filled
        // @blockType: the type of block to be used for fill the area
        bool WorldRenderer::generateBlockVertices(BlockVertex* vertices, size_t& index, const size_t& maxVerticesNum,
                        const float& startX, const float& startY, const float& endX, const float& endY, BlockType block) const
        {
                if(vertices == nullptr || index + 6 > maxVerticesNum)
                        return false;

                float uCoord = endX - startX;
                float vCoord = startY - endY;

                // First triangle bottom left vertex
                vertices[index + 0].position[0] = startX;               // x
                vertices[index + 0].position[1] = endY;                 // y
                vertices[index + 0].uv[0]       = 0.0f;                 // u
                vertices[index + 0].uv[1]       = 0.0f;                 // v
                vertices[index + 0].tileId      = (float) block;        // tile id

                // First triangle bottom right vertex
                vertices[index + 1].position[0] = endX;                 // x
                vertices[index + 1].position[1] = endY;                 // y
                vertices[index + 1].uv[0]       = uCoord;               // u
                vertices[index + 1].uv[1]       = 0.0f;                 // v
                vertices[index + 1].tileId      = (float) block;        // tile id

                // First triangle top left vertex
                vertices[index + 2].position[0] = startX;               // x
                vertices[index + 2].position[1] = startY;               // y
                vertices[index + 2].uv[0]       = 0.0f;                 // u
                vertices[index + 2].uv[1]       = vCoord;               // v
                vertices[index + 2].tileId      = (float) block;        // tile id
                
                // Second triangle bottom right vertex
                vertices[index + 3].position[0] = endX;                 // x
                vertices[index + 3].position[1] = endY;                 // y
                vertices[index + 3].uv[0]       = uCoord;               // u
                vertices[index + 3].uv[1]       = 0.0f;                 // v
                vertices[index + 3].tileId      = (float) block;        // tile id
                
                // Second triangle top right vertex
                vertices[index + 4].position[0] = endX;                 // x
                vertices[index + 4].position[1] = startY;               // y
                vertices[index + 4].uv[0]       = uCoord;               // u
                vertices[index + 4].uv[1]       = vCoord;               // v
                vertices[index + 4].tileId      = (float) block;        // tile id
                
                // Second triangle top left vertex
                vertices[index + 5].position[0] = startX;               // x
                vertices[index + 5].position[1] = startY;               // y
                vertices[index + 5].uv[0]       = 0.0f;                 // u
                vertices[index + 5].uv[1]       = vCoord;               // v
                vertices[index + 5].tileId      = (float) block;        // tile id
               
                index += 6;
                return true;
        }

}
