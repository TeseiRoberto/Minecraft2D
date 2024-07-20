
#include "gameWorld.hpp"

namespace mc2d {

        // Set default dimensions for a chunk
        uint8_t Chunk::width = 18;
        uint8_t Chunk::height = 18;


        // Allocates the specified amount of chunks and assignes ids to them
        GameWorld::GameWorld(uint32_t cachedChunksNum) : m_hasChanged(true)
        {
                if(cachedChunksNum == 0)
                        cachedChunksNum = 3;

                int numOfLeftChunks = (int) ( ((float) cachedChunksNum / 2.0f) );       // Number of chunks on the left of the root chunk
                int id = -1 * numOfLeftChunks;

                for(uint32_t i = 0; i < cachedChunksNum; i++)
                {
                        Chunk c;
                        c.id = id++;
                        c.blocks.reserve(Chunk::width * Chunk::height);

                        m_loadedChunks.push_back(std::move(c));
                }

                m_playerChunk = &(m_loadedChunks[numOfLeftChunks]);                     // Player spawns in the root chunk
        }


        // Changes the block at the specified location
        // @x: x coordinate of the block in world space
        // @y: y coordinate of the block in world space
        // @newBlock: new block to place
        void GameWorld::setBlock(float x, float y, BlockType newBlock)
        {
                if(y > (float) Chunk::height || y < 0.0f)
                        return;

                for(auto& c : m_loadedChunks)
                {
                        if(x < c.getXPos() + Chunk::width)
                        {
                                // Compute indexes in chunk array    
                                uint32_t xIndex = (uint32_t) (x - c.getXPos());
                                uint32_t yIndex = Chunk::height - 1 - (uint8_t) y;
                                
                                c.blocks[(yIndex * Chunk::width) + xIndex] = newBlock;
                                m_hasChanged = true;
                                return;
                        }
                }

                // TODO: For now this method works only on the loaded chunks so it is not
                // possible to set a block in a chunk that is not currently loaded in memory
                logWarn("GameWorld::setBlock() failed, trying to set a block in a chunk that is not currently loaded in memory!");
        }


        // Returns the block placed at the given coordinates
        // @x: x coordinate of the block in world space
        // @y: y coordinate of the block in world space
        BlockType GameWorld::getBlock(float x, float y) const
        {
                if(y > Chunk::height || y < 0)
                        return BlockType::AIR;

                for(auto& c : m_loadedChunks)
                {
                        if(x < c.getXPos() + Chunk::width)
                        {
                                // Compute indexes in chunk array    
                                uint32_t xIndex = (uint32_t) (x - c.getXPos());
                                uint32_t yIndex = Chunk::height - 1 - (uint8_t) y;
                                
                                return c.blocks[(yIndex * Chunk::width) + xIndex];
                        }
                }

                // TODO: For now this method works only on the loaded chunks so it is not
                // possible to get a block in a chunk that is not currently loaded in memory
                logWarn("GameWorld::getBlock() failed, trying to get a block in a chunk that is not currently loaded in memory! Air will be returned.");
                return BlockType::AIR;
        }


        //void GameWorld::computeVisibleBlocksVertices(const Camera& camera, float* vertices, size_t maxVerticesNum, size_t* verticesNum);
        //void GameWorld::optimizedComputeVisibleBlocksVertices(const Camera& camera, float* vertices, size_t maxVerticesNum, size_t* verticesNum);


        // Generates a completely random (nonsense) world
        // (TODO: Move world generation in another place)
        void GameWorld::generateRandomWorld()
        {
                for(auto& chunk : m_loadedChunks)
                        generateRandomChunk(chunk);

                m_hasChanged = true;
        }

        // Generates a world that contains only flat chunks 
        // (TODO: Move world generation in another place)
        void GameWorld::generateFlatWorld()
        {
                for(auto& chunk : m_loadedChunks)
                        generateFlatChunk(chunk);

                m_hasChanged = true;
        }


        // Generates a completely random (nonsense) chunk
        // (TODO: Move world generation in another place)
        void GameWorld::generateRandomChunk(Chunk& c)
        {
                c.blocks.reserve(Chunk::width * Chunk::height);
                std::srand(std::time(nullptr));

                for(uint32_t y = 0; y < Chunk::height; y++)
                {
                        for(uint32_t x = 0; x < Chunk::width; x++)
                        {
                                if(std::rand() % 2 == 0)
                                {
                                        c.blocks[(y * Chunk::width) + x] = BlockType::AIR;
                                } else {
                                        c.blocks[(y * Chunk::width) + x] = BlockType::GRASS;
                                }
                        }
                }
        }


        // Generates a flat chunk that contains only grass, dirt and stone
        // (TODO: Move world generation in another place???)
        void GameWorld::generateFlatChunk(Chunk& c)
        {
                c.blocks.reserve(Chunk::width * Chunk::height);
                uint32_t offset = 0;

                // Half of the chunk must be filled with air blocks
                std::memset(c.blocks.data(), (uint8_t) BlockType::AIR, (Chunk::height / 2) * Chunk::width);
                offset += ((Chunk::height / 2) * Chunk::width);

                // then we add a layer of grass
                std::memset(c.blocks.data() + offset, (uint8_t) BlockType::GRASS, Chunk::width);
                offset += Chunk::width;

                // then we add two layers of dirt
                std::memset(c.blocks.data() + offset, (uint8_t) BlockType::DIRT, Chunk::width * 2);
                offset += Chunk::width * 2;

                // and then we add stone blocks until the end of the chunk
                std::memset(c.blocks.data() + offset, (uint8_t) BlockType::STONE, ((Chunk::height / 2) - 3)  * Chunk::width );
        }



}

