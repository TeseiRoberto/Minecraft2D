
#include "gameWorld.hpp"
#include "worldGenerator.hpp"

namespace mc2d {


        // GameWorld constructor, creates a single uninitialized chunk
        GameWorld::GameWorld() : m_hasChanged(true), m_player(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, EntityType::PLAYER)
        {
                initializeDummyWorld();
        }


        // Creates a game world that contains the given chunks
        GameWorld::GameWorld(std::vector<Chunk>&& chunks) : m_hasChanged(true), m_player(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, EntityType::PLAYER)
        {
                if(chunks.size() != 0)
                {
                        size_t playerChunkIndex = chunks.size() / 2;
                        m_loadedChunks = std::move(chunks);

                        // TODO: Compute player pos in the chunk
                } else {
                        logWarn("GameWorld::GameWorld() failed, given chunks list is empty, a dummy world has been created!");
                        initializeDummyWorld();
                }
        }


        // Move assignement operator
        GameWorld& GameWorld::operator = (GameWorld&& world)
        {
                m_loadedChunks = std::move(world.m_loadedChunks);
                m_hasChanged = true;

                return *this;
        }


        // Attempts to initialize a game world from the data contained in the specified file
        // @filepath: path to the file that contains the world data
        // @returns: true if world is loaded correctly, false otherwise
        bool GameWorld::loadFromFile(const std::string& filepath)
        {
                // TODO: Add implementation...
                return false;
        }


        // Saves the world data to the given filepath
        // @filepath: path to the file in which the world data will be saved
        // @returns: true if world is saved correctly, false otherwise
        bool GameWorld::saveToFile(const std::string& filepath)
        {
                // TODO: Add implementation...
                return false;
        }


        // Changes the block at the specified location
        // @x: x coordinate of the block in world space
        // @y: y coordinate of the block in world space
        // @newBlock: new block to place
        void GameWorld::setBlock(float x, float y, BlockType newBlock)
        {
                if(y > ((float) Chunk::height * BLOCK_HEIGHT) || y < 0.0f)
                        return;

                for(auto& c : m_loadedChunks)
                {
                        if(x > c.getPos().x && x < c.getPos().x + ((float)Chunk::width * BLOCK_WIDTH) )
                        {
                                // Compute indexes relative to the blocks array of the chunk
                                size_t xIndex = (size_t) std::floor(x - c.getPos().x);
                                size_t yIndex = (size_t) std::floor(c.getPos().y - y);
                                
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
                if(y > ((float) Chunk::height * BLOCK_HEIGHT) || y < 0)
                        return BlockType::AIR;

                for(auto& c : m_loadedChunks)
                {
                        if(x > c.getPos().x && x < c.getPos().x + ((float)Chunk::width * BLOCK_WIDTH) )
                        {
                                // Compute indexes relative to the blocks array of the chunk
                                size_t xIndex = (size_t) std::floor(x - c.getPos().x);
                                size_t yIndex = (size_t) std::floor(c.getPos().y - y);
                                
                                return c.blocks[(yIndex * Chunk::width) + xIndex];
                        }
                }

                // TODO: For now this method works only on the loaded chunks so it is not
                // possible to get a block in a chunk that is not currently loaded in memory
                logWarn("GameWorld::getBlock() failed, trying to get a block in a chunk that is not currently loaded in memory! Air will be returned.");
                return BlockType::AIR;
        }


        // Adds the given chunk to the list of loaded chunks for this world
        void GameWorld::appendChunk(Chunk&& newChunk)
        {
                for(const auto& c : m_loadedChunks)
                {
                        if(c.id == newChunk.id)
                        {
                                logWarn("GameWorld::appendChunk() failed, trying to append a chunk with an id that is alredy in use by another chunk!");
                                return;
                        }
                }

                m_loadedChunks.push_back(std::move(newChunk));
                m_hasChanged = true;
        }


        // Unloads the chunk with the given id from this world (if it is currently loaded)
        void GameWorld::removeChunk(int id)
        {
                for(auto it = m_loadedChunks.begin(); it != m_loadedChunks.end(); ++it)
                {
                        if(it->id == id)
                        {
                                m_loadedChunks.erase(it);
                                m_hasChanged = true;
                                return;
                        }
                }
        }


        // Returns a pointer to the chunk that contains the player, nullptr if such chunk cannot be determined
        Chunk* GameWorld::getPlayerChunk() 
        {
                for(Chunk& c : m_loadedChunks)
                {
                        if(c.getPos().x <= m_player.getPos().x && c.getPos().x + (float) Chunk::width > m_player.getPos().x)
                                return &c;
                }

                return nullptr;
        }


        // Initializes game world so that it contains only one flat chunk
        void GameWorld::initializeDummyWorld()
        {
                Chunk c = WorldGenerator::generateFlatChunk();
                c.id = 0;

                m_loadedChunks.push_back(std::move(c));
                // TODO: Compute player position
        }

}

