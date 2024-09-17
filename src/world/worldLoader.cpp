
#include "worldLoader.hpp"
#include "worldGenerator.hpp"

namespace mc2d {


        // Loads a game world from the given path
        // @worldPath: path to the directory that contains chunks and world files
        // @returns: a game world (loaded from file system if an associated file exists in the given world path, randomly generated otherwise)
        GameWorld WorldLoader::loadWorld(const std::filesystem::path& worldPath)
        {
                logWarn("WorldLoader::laodWorld() called, try to load from \"%s\"", worldPath.string());

                if(worldPath.empty())
                        return WorldGenerator::generateRandomWorld( (unsigned) std::time(nullptr), 3 );

                return WorldGenerator::generateRandomWorld( (unsigned) std::time(nullptr), 3 );
        }


        // Saves data about the given game world on file system at the given path
        // @worldPath: path to the directory in which chunks and world files will be saved
        // @gameWorld: the game world that must be saved
        void WorldLoader::saveWorld(const std::filesystem::path& worldPath, const GameWorld& world)
        {
                logWarn("WorldLoader::saveWorld() called, trying to save world at: %s", worldPath.string().c_str());
        }


        // Loads a chunk from the given path
        // @worldPath: path to the directory that contains chunks and world files
        // @returns: a chunk (loaded from file system if an associated file exists in the given world path, randomly generated otherwise)
        Chunk WorldLoader::loadChunk(const std::filesystem::path& worldPath, int chunkId)
        {
                return WorldGenerator::generateRandomChunk( (unsigned) std::time(nullptr));
        }


        void WorldLoader::saveChunk(const std::filesystem::path& worldPath, const Chunk& chunk)
        {
                logInfo("WorldLoader::saveChunk() called, trying to save chunk at: %s", worldPath.string().c_str());
        }

}
