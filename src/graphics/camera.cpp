
#include "camera.hpp"

namespace mc2d {


        // Camera constructor
        // @xPos: x coordinate of the top left corner of the camera in world space
        // @yPos: y coordinate of the top left corner of the camera in world space
        // @zoom: zoom value
        // @width: the width of the camera measured in blocks
        // @height: the height of the camera measured in blocks
        Camera::Camera(float xPos, float yPos, float zoom, uint8_t width, uint8_t height)
                : m_pos( glm::vec3(xPos, yPos, 0.0f) ), m_zoom(zoom), m_width(width), m_height(height), m_hasChanged(true)
        {}


        // Updates the camera width
        // @width: the new width of the camera (must be in range [MIN_WIDTH, UINT8_MAX])
        void Camera::setWidth(uint8_t width)
        {
                if(width < Camera::MIN_WIDTH)
                {
                        logWarn("Camera::setWidth() failed, camera width must be a value between %u and %u!", Camera::MIN_WIDTH, UINT8_MAX);
                        return;
                }

                m_width = width;
                m_hasChanged = true;
        }


        // Updates the camera height
        // @height: the new height of the camera (must be in range [MIN_HEIGHT, UINT8_MAX])
        void Camera::setHeight(uint8_t height)
        {
                if(height < Camera::MIN_HEIGHT)
                {
                        logWarn("Camera::setHeight() failed, camera height must be a value between %u and %u!", Camera::MIN_HEIGHT, UINT8_MAX);
                        return;
                }

                m_height = height;
                m_hasChanged = true;
        }


        // Converts coordinates from window space to world space
        // @x: x coordinate in window space
        // @y: y coordinate in window space
        // @windowWidth: the width of the window 
        // @windowHeight: the height of the window 
        glm::vec2 Camera::windowToWorldCoord(float x, float y, float windowWidth, float windowHeight) const
        {
                float xRelativeToCam = (x / windowWidth) *  (float) m_width;
                float yRelativeToCam = (y / windowHeight) * (float) m_height;

                return glm::vec2(
                        m_pos.x + xRelativeToCam,
                        m_pos.y - yRelativeToCam
                );
        }


        // Returns a matrix that transforms world coordinates to view coordinates
        glm::mat4 Camera::getViewMatrix() const
        {
                return glm::translate(glm::mat4(1.0f), glm::vec3( -m_pos.x, -(m_pos.y - (float) m_height), 0.0f) );
        }


        // Computes the vertices (in world coordinates) and the texture coordinates for all the blocks in
        // the given world that are visible from this camera (and that are in an already loaded chunk)
        // @world: the world of which blocks will be considered
        // @vertices: memory buffer in which the computed vertices will be stored
        // @maxVerticesNum: maximum amount of floats that can be stored in the given buffer
        // @verticesNum: variable in which the number of vertices computed will be written
        void Camera::computeVisibleBlocksVertices(const GameWorld& world, float* vertices, const size_t maxVerticesNum, size_t* verticesNum)
        {
                if(vertices == nullptr || maxVerticesNum == 0 || verticesNum == nullptr)
                {
                        logError("Camera::computeVisibleBlocksVertices() failed, cannot store vertices in the given buffer"
                                        "(one of the input parameter is nullptr or zero)!");
                        return;
                }

                *verticesNum = 0;
                size_t vertexIndex = 0; // Counter for the vertices inserted in the buffer
        
                // Step 1] Determine all the chunks that are covered by the camera
                std::vector<const Chunk*> intersectedChunks = computeVisibleChunks(world);
                if(intersectedChunks.size() == 0)
                        return;

                glm::vec<2, size_t> initialIndex(0, 0);                 // X and Y indexes of the first block visible from the camera (relative to the blocks array of the first chunk)
                glm::vec2 initialPos(0.0f, 0.0f);                       // X and y coordinates (in world space) of the top left vertex of the first block that is visible from the camera

                // Step 2] Determine the indexes and the world position of the first block that is visible from the camera
                if(m_pos.x <= intersectedChunks[0]->getPos().x)         // Camera origin is at the left of the chunk origin
                {
                        initialIndex.x = 0;
                        initialPos.x = intersectedChunks[0]->getPos().x;
                } else {
                        initialIndex.x = std::floor(m_pos.x - intersectedChunks[0]->getPos().x);
                        initialPos.x = intersectedChunks[0]->getPos().x + ( (float) initialIndex.x * BLOCK_WIDTH);
                }

                if(m_pos.y < intersectedChunks[0]->getPos().y)          // Camera origin is below the chunk origin
                {
                        initialIndex.y = std::floor(intersectedChunks[0]->getPos().y - m_pos.y);
                        initialPos.y = intersectedChunks[0]->getPos().y - ( (float) initialIndex.y * BLOCK_HEIGHT);
                } else {
                        initialIndex.y = 0;
                        initialPos.y = intersectedChunks[0]->getPos().y;
                }

                glm::vec<2, size_t> currIndex = initialIndex;           // X and Y indexes of the current block (relative to the blocks array of the current chunk)
                glm::vec2 currPos = initialPos;                         // X and y coordinates (in world space) of the top left vertex of the current block
                
                // Step 3] Iterate over the region of space that is visible from the camera (+ 1 block of safety)
                for(size_t i = 0; i < m_height + 1; ++i, currPos.y -= BLOCK_HEIGHT)
                {
                        auto currChunk = intersectedChunks.begin();

                        if(currPos.y > (*currChunk)->getPos().y)        // If we are on top of the chunk then there is no block to analyze so skip this line
                                continue;
                        else if(currPos.y <= 0)                         // If we are below the chunk then there's no more blocks to render, our job is done
                                break;

                        // Reset values for the x axis
                        currIndex.x = initialIndex.x;
                        currPos.x = initialPos.x;

                        for(size_t j = 0; j < m_width + 1; ++j, currPos.x += BLOCK_WIDTH)
                        {
                                if(currIndex.x == Chunk::width)         // Handle passage to the adjacent chunk
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

                                if(vertexIndex + 30 > maxVerticesNum)   // Check if given buffer is full
                                {
                                        logError("GameWorld::computeVisibleBlocksVertices() failed, cannot store all vertices in the given buffer,"
                                                        "the number of vertices of the visible blocks is greater than the given buffer size");
                                        *verticesNum = vertexIndex;
                                        return;
                                }

                                // Step 4] Generate vertices for the current block
                                if(!generateBlockVertices(vertices, vertexIndex, maxVerticesNum, currPos.x, currPos.y, currPos.x + BLOCK_WIDTH, currPos.y - BLOCK_HEIGHT, currBlock))
                                {
                                        logError("GameWorld::optimizedComputeVisibleBlocksVertices() failed, cannot store all vertices in the given buffer,"
                                                        "the number of vertices of the visible blocks is greater than the given buffer size");

                                        *verticesNum = vertexIndex;
                                        return;
                                }

                                ++currIndex.x;
                        }
                        ++currIndex.y;
                }

                *verticesNum = vertexIndex;
                logInfo("computeVisibleBlocksVertices algorithm computed %u vertices for %u rectangles", vertexIndex, vertexIndex / 30);
        }


        // Computes the vertices (in world coordinates) and the texture coordinates for all the blocks in
        // the given world that are visible from this camera (and that are in an already loaded chunk), to do
        // so it uses a 1D greedy meshing algorithm that composes adjacent block of the same type in one single rectangle.
        // @world: the world of which blocks will be considered
        // @vertices: memory buffer in which the computed vertices will be stored
        // @maxVerticesNum: maximum amount of floats that can be stored in the given buffer
        // @verticesNum: variable in which the number of vertices computed will be written
        void Camera::optimizedComputeVisibleBlocksVertices(const GameWorld& world, float* vertices, const size_t maxVerticesNum, size_t* verticesNum)
        {
                if(vertices == nullptr || maxVerticesNum == 0 || verticesNum == nullptr)
                {
                        logError("Camera::computeVisibleBlocksVertices() failed, cannot store vertices in the given buffer"
                                        "(one of the input parameter is nullptr or zero)!");
                        return;
                }
        
                *verticesNum = 0;
                size_t vertexIndex = 0; // Counter for the vertices inserted in the buffer

                // Step 1] Determine all the chunks that are covered by the camera
                std::vector<const Chunk*> intersectedChunks = computeVisibleChunks(world);
                if(intersectedChunks.size() == 0)
                        return;

                glm::vec<2, size_t> initialIndex(0, 0);                 // X and Y indexes of the first block visible from the camera (relative to the blocks array of the first chunk)
                glm::vec2 initialPos(0.0f, 0.0f);                       // X and y coordinates (in world space) of the top left vertex of the first block that is visible from the camera

                // Step 2] Determine the indexes and the world position of the first block that is visible from the camera
                if(m_pos.x <= intersectedChunks[0]->getPos().x)         // Camera origin is at the left of the chunk origin
                {
                        initialIndex.x = 0;
                        initialPos.x = intersectedChunks[0]->getPos().x;
                } else {
                        initialIndex.x = std::floor(m_pos.x - intersectedChunks[0]->getPos().x);
                        initialPos.x = intersectedChunks[0]->getPos().x + ( (float) initialIndex.x * BLOCK_WIDTH);
                }

                if(m_pos.y < intersectedChunks[0]->getPos().y)          // Camera origin is below the chunk origin
                {
                        initialIndex.y = std::floor(intersectedChunks[0]->getPos().y - m_pos.y);
                        initialPos.y = intersectedChunks[0]->getPos().y - ( (float) initialIndex.y * BLOCK_HEIGHT);
                } else {
                        initialIndex.y = 0;
                        initialPos.y = intersectedChunks[0]->getPos().y;
                }

                glm::vec<2, size_t> currIndex = initialIndex;           // X and Y indexes of the current block (relative to the blocks array of the current chunk)
                glm::vec2 currPos = initialPos;                         // X and y coordinates (in world space) of the top left vertex of the current block
                
                // Step 3] Iterate over the region of space that is visible from the camera (+ 1 block of safety)
                for(size_t i = 0; i < m_height + 1; ++i, currPos.y -= BLOCK_HEIGHT)
                {
                        auto currChunk = intersectedChunks.begin();

                        if(currPos.y > (*currChunk)->getPos().y)        // If we are on top of the chunk then there is no block to analyze so skip this line
                                continue;
                        else if(currPos.y <= 0)                         // If we are below the chunk then there's no more blocks to render, our job is done
                                break;

                        // Reset values for the x axis
                        currIndex.x = initialIndex.x;
                        currPos.x = initialPos.x;

                        for(size_t j = 0; j <= m_width; ++j)
                        {
                                size_t k;
                                float firstBlockPosX = currPos.x;
                                BlockType firstBlock = (*currChunk)->blocks[(currIndex.y * Chunk::width) + currIndex.x];
                                size_t counter = 0;

                                // Step 4] Use greedy meshing to compose adjacent blocks of the same type into one single rectangle
                                for(k = j; k <= m_width; ++k, currPos.x += BLOCK_WIDTH, ++currIndex.x)
                                {
                                        if(currIndex.x == Chunk::width)         // Handle passage to the adjacent chunk
                                        {
                                                ++currChunk;
                                                if(currChunk == intersectedChunks.end())
                                                {
                                                        k = SIZE_MAX;           // This makes sure that we will exit the outer loop too
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
                                                logError("GameWorld::optimizedComputeVisibleBlocksVertices() failed, cannot store all vertices in the given buffer,"
                                                                "the number of vertices of the visible blocks is greater than the given buffer size");

                                                *verticesNum = vertexIndex;
                                                return;
                                        }
                                }

                                j = k - 1;
                        }

                        ++currIndex.y;
                }

                *verticesNum = vertexIndex;
                logInfo("optimizedComputeVisibleBlocksVertices algorithm computed %u vertices for %u rectangles", vertexIndex, vertexIndex / 30);
        }


        // Utility function that generates vertices for blocks in a way such that the blocks generated will fill the given area
        // @vertices: memory buffer in which the computed vertices will be stored
        // @index: specifies the starting point in the buffer 
        // @maxVerticesNum: maximum amount of floats that can be stored in the given buffer
        // @startX, startY: x and y coordinates in world space of the top left corner of the area to be filled
        // @endX, endY: x and y coordinates in world space of the bottom right corner of the area to be filled
        // @blockType: the type of block to be used for fill the area
        bool Camera::generateBlockVertices(float* vertices, size_t& index, const size_t& maxVerticesNum,
                        const float& startX, const float& startY, const float& endX, const float& endY, BlockType block) const
        {
                if(vertices == nullptr || index + 30 > maxVerticesNum)
                        return false;

                float uCoord = endX - startX;
                float vCoord = startY - endY;

                // First triangle bottom left vertex
                vertices[index + 0] = startX;                   // x
                vertices[index + 1] = endY;                     // y
                vertices[index + 2] = 0.0f;                     // u
                vertices[index + 3] = 0.0f;                     // v
                vertices[index + 4] = (float) block;            // tile id

                // First triangle bottom right vertex
                vertices[index + 5] = endX;                     // x
                vertices[index + 6] = endY;                     // y
                vertices[index + 7] = uCoord;                   // u
                vertices[index + 8] = 0.0f;                     // v
                vertices[index + 9] = (float) block;            // tile id

                // First triangle top left vertex
                vertices[index + 10] = startX;                  // x
                vertices[index + 11] = startY;                  // y
                vertices[index + 12] = 0.0f;                    // u
                vertices[index + 13] = vCoord;                  // v
                vertices[index + 14] = (float) block;           // tile id
                
                // Second triangle bottom right vertex
                vertices[index + 15] = endX;                    // x
                vertices[index + 16] = endY;                    // y
                vertices[index + 17] = uCoord;                  // u
                vertices[index + 18] = 0.0f;                    // v
                vertices[index + 19] = (float) block;           // tile id
                
                // Second triangle top right vertex
                vertices[index + 20] = endX;                    // x
                vertices[index + 21] = startY;                  // y
                vertices[index + 22] = uCoord;                  // u
                vertices[index + 23] = vCoord;                  // v
                vertices[index + 24] = (float) block;           // tile id
                
                // Second triangle top left vertex
                vertices[index + 25] = startX;                  // x
                vertices[index + 26] = startY;                  // y
                vertices[index + 27] = 0.0f;                    // u
                vertices[index + 28] = vCoord;                  // v
                vertices[index + 29] = (float) block;           // tile id
               
                index += 30;
                return true;
        }


        // Utility function used to determine all chunks of the given world that are visible from this camera
        // @world: the world that contains the chunks
        std::vector<const Chunk*> Camera::computeVisibleChunks(const GameWorld& world)
        {
                std::vector<const Chunk*> intersectedChunks;
                for(auto& chunk : world.getChunks())
                {
                        if(doesRectsIntersect(m_pos.x, m_pos.y, (float) m_width, (float) m_height,
                                                chunk.getPos().x, chunk.getPos().y, (float) Chunk::width, (float) Chunk::height))
                                intersectedChunks.push_back(&chunk);
                }

                return intersectedChunks;
        }

}
