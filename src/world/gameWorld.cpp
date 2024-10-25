
#include "gameWorld.hpp"
#include "worldGenerator.hpp"
#include "worldLoader.hpp"
#include "graphics/camera.hpp"

namespace mc2d {

        
        // Writes the chunk data in the given file
        // @file: output file stream in which chunk data will be written
        // @returns: true if serialization is successfull, false otherwise
        bool Chunk::serialize(std::ofstream& file) const
        {
                if(!file.good())
                {
                        logError("Chunk::serialize() failed, the given file stream is broken!");
                        return false;
                }

                bool res = true;

                // First we save the chunk biome type
                file << static_cast<uint32_t>(biome) << '\n';

                // Then we save the chunk dimensions (for now they are fixed but they may become a game setting in the future)
                file << Chunk::width << ' ' << Chunk::height << '\n';
                
                // Then we save all the blocks in the chunk
                for(size_t y = 0; y < Chunk::height; ++y)
                {
                        for(size_t x = 0; x < Chunk::width; ++x)
                                file << static_cast<uint32_t>(blocks[ (y * Chunk::width) + x ]) << ' ';

                        file << '\n';
                }

                // And then data about all the entities contained in this chunk
                file << entities.size() << '\n';
                for(auto e = entities.begin(); e != entities.end() && res != false; ++e)
                        res = e->serialize(file);

                return res;
        }


        // Reads chunk data from the given file
        // @file: input file stream from which chunk data will be read
        // @returns: true if deserialization is successfull, false otherwise
        bool Chunk::deserialize(std::ifstream& file)
        {
                if(!file.good())
                {
                        logError("Chunk::deserialize() failed, the given file stream is broken!");
                        return false;
                }

                bool res;
                uint32_t biomeType;
                uint8_t expectedChunkWidth;
                uint8_t expectedChunkHeight;
                std::vector<BlockType> blocks;
                size_t entitiesNum;
                std::vector<Entity> entities;

                file >> biomeType;                              // Read biome of the chunk
                file >> expectedChunkWidth;                     // Read width of the chunk
                file >> expectedChunkHeight;                    // Read height of the chunk

                res = file.good();
                if(!res)
                {
                        logError("Chunk::deserialize() failed, cannot read chunk properties (biome type and/or dimensions)!");
                        return false;
                }

                // Then we read data about all the blocks in the chunk
                blocks.reserve(expectedChunkWidth * expectedChunkHeight);
                for(size_t i = 0; i < expectedChunkWidth * expectedChunkHeight && res != false; ++i)
                {
                        uint32_t currBlockType;
                        file >> currBlockType;
                        blocks.push_back(static_cast<BlockType>(currBlockType));

                        res = file.good();
                }

                // And then we read data about all the entities contained in the chunk
                file >> entitiesNum;
                blocks.reserve(entitiesNum);
                for(size_t i = 0; i < entitiesNum && res != false; ++i)
                {
                        Entity e(glm::vec3(0.0f), 100.0f, EntityType::CHICKEN);
                        res = e.deserialize(file);
                        entities.push_back(e);
                }

                // If deserialization was successfull we can use such data to intiialize this chunk
                if(res)
                {
                        this->biome = static_cast<BiomeType>(biomeType);
                        this->blocks = std::move(blocks);
                        this->entities = std::move(entities);
                }

                return res;
        }


        // GameWorld constructor, creates a zero intialized world
        GameWorld::GameWorld() :
                m_hasChanged(false), m_worldSeed(0), m_dayDuration(0), m_dayTime(0.0f), m_pathToWorldDir(""),
                m_loadedChunks({}), m_players( { Entity(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f, EntityType::PLAYER) } )
        {}


        // Copy constructor
        GameWorld::GameWorld(GameWorld& otherWorld)
        {
                m_hasChanged = true;

                m_worldSeed = otherWorld.m_worldSeed;
                m_dayDuration = otherWorld.m_dayDuration;
                m_dayTime = otherWorld.m_dayTime;

                m_loadedChunks = otherWorld.m_loadedChunks;
                m_players = otherWorld.m_players;

                m_pathToWorldDir = otherWorld.m_pathToWorldDir;
        }


        // Creates a game world that contains the given players and chunks 
        // @chunks: chunks that makes up the world
        // @seed: seed that will be used to generate new chunks when needed
        // @dayDuration: duration of one day in the world (in milliseconds)
        GameWorld::GameWorld(std::vector<Chunk>&& chunks, unsigned seed, size_t dayDuration) :
                m_hasChanged(true), m_worldSeed(seed), m_dayDuration(dayDuration), m_pathToWorldDir(""),
                m_loadedChunks( {} ), m_players( {} )
        {
                if(chunks.empty())
                {
                        const int defaultNumOfChunks = 3;
                        *this = WorldGenerator::generateRandomWorld(seed, defaultNumOfChunks);
                        logWarn("GameWorld::GameWorld() failed, given vector of chunks is empty, a random world has been created!");

                } else {
                        // Reassign ids to the given chunks to ensure that they are all unique
                        int id = -1 * (chunks.size() / 2);      
                        for(auto& c : chunks)
                        {
                                c.id = id;
                                m_loadedChunks.insert( { id, std::move(c) } );
                                ++id;
                        }

                        // Compute spawn position for the main player and insert it in the game world
                        auto rootChunk = m_loadedChunks.find(0);
                        float spawnPosX = Chunk::width / 2.0f;
                        float spawnPosY = WorldEncyclopedia::getBiomeProperties(rootChunk->second.biome).maxTerrainHeight + 2.0f;

                        m_players.emplace_back( glm::vec3(spawnPosX, spawnPosY, 0.0f), 100.0f, EntityType::PLAYER );

                        setDayTime(7, 0);               // Set day time to 07:00
                }
        }


        // Move assignement operator
        GameWorld& GameWorld::operator = (GameWorld&& otherWorld)
        {
                m_hasChanged = true;

                m_worldSeed = otherWorld.m_worldSeed;
                m_dayDuration = otherWorld.m_dayDuration;
                m_dayTime = otherWorld.m_dayTime;

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

                // Update world time (TODO: Still need to implement the day-night cycle)
                m_dayTime += deltaTime;
                if(m_dayTime > (float) m_dayDuration)
                        m_dayTime = 0.0f;
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


        // Sets the current time of the day
        // @hour: hour at which time of the day must be set
        // @minutes: minutes at which time of the day must be set
        void GameWorld::setDayTime(size_t hours, size_t minutes)
        {
                // Compute duration of an hour and a minute in "world time"
                size_t msInAnHour = m_dayDuration / 24;
                size_t msInAMinute = msInAnHour / 60;

                // Update day time
                m_dayTime = static_cast<float>( (hours * msInAnHour) + (minutes * msInAMinute) );
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


        // Returns the current time of the day
        // @hour: output variable in which the current hours value will be written
        // @minutes: output variable in which the current minutes value will be written
        void GameWorld::getDayTime(size_t& hours, size_t& minutes) const
        {
                // Compute duration of an hour and a minute (in milliseconds) in "world time"
                float msInAnHour = (float) m_dayDuration / 24.0f;
                float msInAMinute = (float) msInAnHour / 60.0f;

                // Convert day time from milliseconds to hour and minutes
                float millis = m_dayTime;
                
                hours = static_cast<size_t>(millis / msInAnHour);
                millis -= hours * msInAnHour;

                minutes = static_cast<size_t>(millis / msInAMinute);
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
                        loadChunk(playerChunkId - 1);
                        loadChunk(playerChunkId + 1);
                }
        }


        // Writes the world data in the given file
        // @file: output file stream in which world data will be written
        // @returns: true if serialization is successfull, false otherwise
        bool GameWorld::serialize(std::ofstream& file) const
        {
                if(!file.good())
                {
                        logError("GameWorld::serialize() failed, the given file stream is broken!");
                        return false;
                }

                bool res = true;

                // First we save the world seed
                file << m_worldSeed << '\n';

                // Then the day duration and the current day time
                file << m_dayDuration << ' ' << m_dayTime << '\n';

                // Then data about all players in the game world
                file << m_players.size() << '\n';

                for(auto p = m_players.begin(); p != m_players.end() && res != false; ++p)
                        res = p->serialize(file);

                // Then save all the currently loaded chunks
                for(auto c = m_loadedChunks.begin(); c != m_loadedChunks.end() && res != false; ++c)
                        res = WorldLoader::saveChunk(m_pathToWorldDir, c->second);

                return res;
        }


        // Reads world data from the given file
        // @file: input file stream from which world data will be read
        // @returns: true if deserialization is successfull, false otherwise
        bool GameWorld::deserialize(std::ifstream& file)
        {
                if(!file.good())
                {
                        logError("GameWorld::deserialize() failed, the given file stream is broken!");
                        return false;
                }

                bool res = true;
                unsigned seed;
                size_t dayDuration;
                float dayTime;
                size_t playersNum;
                std::vector<Entity> players;

                file >> seed;                                   // Read world seed
                file >> dayDuration;                            // Read duration of one day in the world
                file >> dayTime;                                // Read current time of the day in the world
                file >> playersNum;                             // Read Number of players in the world

                res = file.good();
                if(!res)
                {
                        logError("GameWorld::deserialize() failed, cannot read world data from the given stream!");
                        return res;
                }

                // Read data for all players
                players.reserve(playersNum);
                for(size_t i = 0; i < playersNum && res != false; ++i)
                {
                        Entity currPlayer(glm::vec3(0.0f), 100.0f, EntityType::PLAYER);
                        res = currPlayer.deserialize(file);
                        players.push_back(currPlayer);
                }

                // If data deserialization was successfull we can use such data to setup this game world
                if(res)
                {
                        m_worldSeed = seed;
                        m_dayDuration = dayDuration;
                        m_dayTime = dayTime;
                        m_players = std::move(players);

                        // Load all chunks near players (for each player the chunks
                        // adjacent to the one in which the player currently is must be loaded)
                        for(auto& p : m_players)
                        {
                                int playerChunkId = getEntityChunkId(p);
                                loadChunk(playerChunkId - 1);
                                loadChunk(playerChunkId);
                                loadChunk(playerChunkId + 1);
                        }
                }

                return res;
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

