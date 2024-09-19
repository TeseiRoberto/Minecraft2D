
#include "worldLoader.hpp"
#include "worldGenerator.hpp"

namespace mc2d {


        // Loads a game world from the given path
        // @worldPath: path to the directory that contains the world data
        // @world: the GameWorld instance in which the data loaded will be stored
        // @returns: true if the world gets loaded correctly, false otherwise
        bool WorldLoader::loadWorld(const std::filesystem::path& worldPath, GameWorld& world)
        {
                if(worldPath.empty())
                {
                        logError("WorldLoader::loadWorld() failed, the path to the directory from which world data should be loaded is empty!");
                        return false;
                }

                // TODO: Add a real implementation...
                
                logWarn("WorldLoader::loadWorld() called, loading from: \"%s\"", worldPath.string().c_str());
                world = WorldGenerator::generateRandomWorld( (unsigned) std::time(nullptr), 3 );
                world.m_pathToWorldDir = worldPath;

                return true;
        }


        // Saves data about the given game world on file system at the specified path
        // @worldPath: path to the directory in which the world data should be saved
        // @gameWorld: the game world that should be saved
        // @returns: true if the world gets saved correctly on filesystem, false otherwise
        bool WorldLoader::saveWorld(const std::filesystem::path& worldPath, GameWorld& world)
        {
                if(worldPath.empty())
                {
                        logError("WorldLoader::saveWorld() failed, the path to the directory in which world data should be saved is empty!");
                        return false;
                }

                // TODO: Add a real implementation...

                logWarn("WorldLoader::saveWorld() called, saving world at: %s", worldPath.string().c_str());
                world.m_pathToWorldDir = worldPath;

                return true;
        }


        // Loads a chunk from the given path
        // @worldPath: path to the directory that contains the world data
        // @chunkId: id of the chunk that should be loaded
        // @chunk: Chunk instance in which the data loaded will be stored
        // @returns: true if the chunk gets loaded correctly, false otherwise
        bool WorldLoader::loadChunk(const std::filesystem::path& worldPath, int chunkId, Chunk& chunk)
        {
                if(worldPath.empty())
                {
                        logError("WorldLoader::loadChunk() failed, the path to the directory from which chunk data should be loaded is empty!");
                        return false;
                }

                // TODO: Add a real implementation...
                
                logWarn("WorldLoader::loadChunk() called, loading chunk: %d", chunkId);
                chunk = WorldGenerator::generateRandomChunk( (unsigned) std::time(nullptr));
                chunk.id = chunkId;

                return true;
        }

        
        // Saves data about the given chunk on file system at the specified path
        // @worldPath: path to the directory that contains the world data
        // @returns: true if the chunk gets saved correctly, false otherwise
        bool WorldLoader::saveChunk(const std::filesystem::path& worldPath, const Chunk& chunk)
        {
                if(worldPath.empty())
                {
                        logError("WorldLoader::saveChunk() failed, the path to the directory in which the chunk should be saved is empty!");
                        return false;
                }

                // TODO: Add a real implementation...
                
                logInfo("WorldLoader::saveChunk() called, saving chunk at: %s", worldPath.string().c_str());

                return true;
        }


        // Returns a string that can be used as a game world name
        std::string WorldLoader::createDummyWorldName()
        {
                std::stringstream worldName;
                std::time_t currTime = std::time(nullptr);

                worldName << "World_" << std::put_time( std::localtime(&currTime), "%F %T" );
                return worldName.str();
        }

}
