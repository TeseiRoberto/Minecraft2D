
#include "gameWorld.hpp"
#include "worldGenerator.hpp"
#include "worldLoader.hpp"
#include "graphics/camera.hpp"

namespace mc2d {


        // GameWorld constructor, creates a zero intialized world
        GameWorld::GameWorld() :
                m_hasChanged(false), m_worldSeed(0), m_pathToWorldDir(""),
                m_loadedChunks({}), m_players( { Entity(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, EntityType::PLAYER) } )
        {}


        // Copy contructor
        GameWorld::GameWorld(GameWorld& otherWorld)
        {
                m_hasChanged = true;
                m_worldSeed = otherWorld.m_worldSeed;
                m_loadedChunks = otherWorld.m_loadedChunks;
                m_players = otherWorld.m_players;
                m_pathToWorldDir = otherWorld.m_pathToWorldDir;
        }


        // Creates a game world that contains the given chunks
        // @chunks: chunks that makes up the world
        // @seed: seed that will be used to generate new chunks when needed
        GameWorld::GameWorld(std::vector<Chunk>&& chunks, unsigned seed) :
                m_hasChanged(true), m_worldSeed(seed), m_pathToWorldDir(""),
                m_loadedChunks({}), m_players( { Entity(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, EntityType::PLAYER) } )
        {
                if(chunks.size() == 0)
                {
                        const int defaultNumOfChunks = 3;
                        *this = WorldGenerator::generateRandomWorld(seed, defaultNumOfChunks);
                        logWarn("GameWorld::GameWorld() failed, given chunks list is empty, a random world has been created!");
                } else {

                        int id = -1 * (chunks.size() / 2);      
                        for(auto& c : chunks)                   // Assign ids to chunks so that they are all unique and ordered
                        {
                                c.id = id;
                                m_loadedChunks.insert( { id, std::move(c) } );
                                ++id;
                        }

                        // TODO: Calculate player spawn position
                }
        }


        // Move assignement operator
        GameWorld& GameWorld::operator = (GameWorld&& otherWorld)
        {
                m_hasChanged = true;
                m_worldSeed = otherWorld.m_worldSeed;
                m_loadedChunks = std::move(otherWorld.m_loadedChunks);
                m_players = std::move(otherWorld.m_players);
                m_pathToWorldDir = otherWorld.m_pathToWorldDir;

                return *this;
        }


        // Updates all the entities contained in the chunks currently loaded
        void GameWorld::update(float deltaTime)
        {
                bool needToRecomputeChunks = false;
                for(auto& p : m_players)                                // Update all players in the world
                {
                        int prevChunkId = getEntityChunkId(p);
                        p.update(deltaTime);

                        if(getEntityChunkId(p) != prevChunkId)          // Check if a transition between chunks has occurred
                                needToRecomputeChunks = true;           // If so then we may need to load/unload some chunks
                }

                if(needToRecomputeChunks)                               // Load/unload chunks if needed
                        recomputeLoadedChunks();

                for(auto& c : m_loadedChunks)                           // Update entities in all loaded chunks
                {
                        for(auto& e : c.second.entities)
                                e.update(deltaTime);
                }
        }


        // Changes the block at the specified location
        // @x: x coordinate of the block in world space
        // @y: y coordinate of the block in world space
        // @newBlock: new block to place
        void GameWorld::setBlock(float x, float y, BlockType newBlock)
        {
                if(y > ((float) Chunk::height * BLOCK_HEIGHT) || y < 0.0f)
                        return;
        
                int searchedChunkId = std::floor(x / (float) Chunk::width);
                auto c = m_loadedChunks.find(searchedChunkId);

                if(c == m_loadedChunks.end())
                {
                        // NOTE: This method works only on the loaded chunks so it is not
                        // possible to set a block in a chunk that is not currently loaded in memory
                        logWarn("GameWorld::setBlock() failed, trying to set a block in a chunk that is not currently loaded in memory!");
                        return;
                }

                // Compute indexes relative to the blocks array of the chunk
                size_t xIndex = (size_t) std::floor(x - c->second.getPos().x);
                size_t yIndex = (size_t) std::floor(c->second.getPos().y - y);
                
                c->second.blocks[(yIndex * Chunk::width) + xIndex] = newBlock;
                m_hasChanged = true;
        }


        // Returns the block placed at the given coordinates
        // @x: x coordinate of the block in world space
        // @y: y coordinate of the block in world space
        BlockType GameWorld::getBlock(float x, float y) const
        {
                if(y > ((float) Chunk::height * BLOCK_HEIGHT) || y < 0)
                        return BlockType::AIR;

                int searchedChunkId = std::floor(x / (float) Chunk::width);
                auto c = m_loadedChunks.find(searchedChunkId);

                if(c == m_loadedChunks.end())
                {
                        // Note: This method works only on the loaded chunks so it is not
                        // possible to get a block in a chunk that is not currently loaded in memory
                        logWarn("GameWorld::getBlock() failed, trying to get a block in a chunk that is not currently loaded in memory! Air will be returned.");
                        return BlockType::AIR;
                }

                // Compute indexes relative to the blocks array of the chunk
                size_t xIndex = (size_t) std::floor(x - c->second.getPos().x);
                size_t yIndex = (size_t) std::floor(c->second.getPos().y - y);
                
                return c->second.blocks[(yIndex * Chunk::width) + xIndex];
        }


        // Attempts to find the chunk that contains the given entity
        // @e: the entity for which we want to find the chunk
        // @returns: on success a pointer to a chunk, nullptr otherwise
        Chunk* GameWorld::getEntityChunk(const Entity& e)
        {
                auto c = m_loadedChunks.find(getEntityChunkId(e));
                return c == m_loadedChunks.end() ? nullptr : &(c->second);
        }


        // Utility function used to determine all the loaded chunks that are visible from the given camera
        // @camera: defines the point of view of the world
        std::vector<Chunk const*> GameWorld::getVisibleChunks(const Camera& camera) const
        {
                std::vector<const Chunk*> intersectedChunks;
                for(auto& c : m_loadedChunks)
                {
                        if(doesRectsIntersect(camera.getPos().x, camera.getPos().y, (float) camera.getWidth(), (float) camera.getHeight(),
                                                c.second.getPos().x, c.second.getPos().y, (float) Chunk::width, (float) Chunk::height))
                                intersectedChunks.push_back( &(c.second) );
                }

                return intersectedChunks;
        }


        // Loads/unloads chunks according to the positions of the players in the game world
        void GameWorld::recomputeLoadedChunks()
        {
                // Maximum value of distance (squared) between a player and a chunk, a chunk will stay in memory until
                // there is at least one player that has a distance smaller than this value
                const float maxDistanceSquared = (Chunk::width + Chunk::width / 2.0f) * (Chunk::width + Chunk::width / 2.0f);

                // Unload all chunks that are far away from players
                auto c = m_loadedChunks.begin();
                while(c != m_loadedChunks.end())                
                {
                        bool unload = true;
                        float chunkPos = c->second.getPos().x + ((float) Chunk::width / 2.0f);

                        for(auto& p : m_players)
                        {
                                if( (p.getPos().x - chunkPos) * (p.getPos().x - chunkPos) < maxDistanceSquared )
                                {
                                        unload = false;
                                        break;
                                }
                        }

                        if(unload)
                        {
                                c = unloadChunk(c);
                                continue;
                        }

                        ++c;
                }

                // Load all chunks near players (for each player the chunks
                // adjacent to the one in which the player currently is must be loaded)
                for(auto& p : m_players)
                {
                        int playerChunkId = getEntityChunkId(p);
                        if(m_loadedChunks.find(playerChunkId - 1) == m_loadedChunks.end())
                                loadChunk(playerChunkId - 1);

                        if(m_loadedChunks.find(playerChunkId + 1) == m_loadedChunks.end())
                                loadChunk(playerChunkId + 1);
                }
        }


        // Loads the chunk with given id and adds it to the loaded chunks, the chunk gets loaded from file system if an 
        // associated chunk file exists, it is generated from scratch otherwise.
        // If a chunk with the given id is already loaded in the world then nothing happens.
        // @id: id associated to the chunk that needs to be loaded
        void GameWorld::loadChunk(int id)
        {
                if(m_loadedChunks.find(id) != m_loadedChunks.end())
                        return;

                Chunk c;

                // Load chunk data from file, if file does not exists then generate a new random chunk
                if(!WorldLoader::loadChunk(m_pathToWorldDir, id, c))
                {
                        c = WorldGenerator::generateRandomChunk(m_worldSeed + id);
                        c.id = id;
                }

                m_loadedChunks.insert( { id, std::move(c) } );
        }


        // Unloads the given chunk (saves it to a file and removes it from the currently loaded ones)
        // @c: iterator pointing to the chunk that must be unloaded
        // @returns: iterator pointing to the chunk after the given one
        std::map<int, Chunk>::iterator GameWorld::unloadChunk(std::map<int, Chunk>::iterator& c)
        {
                if(c == m_loadedChunks.end())
                        return m_loadedChunks.end();

                WorldLoader::saveChunk(m_pathToWorldDir, c->second);
                return m_loadedChunks.erase(c);
        }


}

