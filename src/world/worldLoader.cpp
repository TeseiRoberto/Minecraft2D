
#include "worldLoader.hpp"
#include "worldGenerator.hpp"

namespace mc2d {


        // Loads a game world from the filesystem
        // @worldDirPath: path to the directory that contains the world data
        // @world: the GameWorld instance in which the data loaded will be stored
        // @returns: true if the world gets loaded correctly, false otherwise
        bool WorldLoader::loadWorld(const std::filesystem::path& worldDirPath, GameWorld& world)
        {
                if(worldDirPath.empty())
                {
                        logError("WorldLoader::loadWorld() failed, the path to the directory from which world data should be loaded is empty!");
                        return false;
                }

                std::filesystem::path worldFilePath = worldDirPath / getWorldFilename();

                // Try to open "..worldDirPath../world.dat" file and deserialize its content
                std::ifstream worldFile;
                worldFile.open(worldFilePath, std::ios::binary);
                if(!worldFile.is_open())
                {
                        logError("WorldLoader::loadWorld() failed, cannot open \"%s\" file!", worldFilePath.c_str())
                        return false;
                }

                world.setWorldSaveDirectory(worldDirPath);
                bool res = world.deserialize(worldFile);

                worldFile.close();
                return res;
        }


        // Saves data about the given game world on the filesystem
        // @worldDirPath: path to the directory in which the world data should be saved
        // @gameWorld: the game world that should be saved
        // @returns: true if the world gets saved correctly on filesystem, false otherwise
        bool WorldLoader::saveWorld(const std::filesystem::path& worldDirPath, GameWorld& world)
        {
                if(worldDirPath.empty())
                {
                        logError("WorldLoader::saveWorld() failed, the path to the directory in which world data should be saved is empty!");
                        return false;
                }

                // Create world directory if it does not exists yet
                if(!std::filesystem::exists(worldDirPath))
                        std::filesystem::create_directory(worldDirPath);

                std::filesystem::path worldFilePath = worldDirPath / getWorldFilename();

                // Try to open "world.dat" file and write content into it
                std::ofstream worldFile;
                worldFile.open(worldFilePath, std::ios::binary | std::ios::trunc);
                if(!worldFile.is_open())
                {
                        logError("WorldLoader::saveWorld() failed, cannot open \"%s\" file!", worldFilePath.c_str())
                        return false;
                }

                world.setWorldSaveDirectory(worldDirPath);
                bool res = world.serialize(worldFile);

                worldFile.close();
                return res;
        }


        // Loads a chunk from the filesystem
        // @worldDirPath: path to the directory that contains the world data
        // @chunkId: id of the chunk that should be loaded
        // @chunk: Chunk instance in which the data loaded will be stored
        // @returns: true if the chunk gets loaded correctly, false otherwise
        bool WorldLoader::loadChunk(const std::filesystem::path& worldDirPath, int chunkId, Chunk& chunk)
        {
                if(worldDirPath.empty())
                {
                        logError("WorldLoader::loadChunk() failed, the path to the directory from which chunk data should be loaded is empty!");
                        return false;
                }

                std::filesystem::path chunkFilePath = worldDirPath / getChunkFilename(chunkId);

                // Try to open "..worldDirPath../chunkX.dat" file and deserialize its content
                std::ifstream chunkFile;
                chunkFile.open(chunkFilePath, std::ios::binary);
                if(!chunkFile.is_open())
                {
                        logError("WorldLoader::loadChunk() failed, cannot open \"%s\" file!", chunkFilePath.c_str())
                        return false;
                }

                bool res = chunk.deserialize(chunkFile);
                chunk.id = chunkId;
                chunkFile.close();
                return res;
        }

        
        // Saves data about the given chunk on the filesystem
        // @worldDirPath: path to the directory that contains the world data
        // @returns: true if the chunk gets saved correctly, false otherwise
        bool WorldLoader::saveChunk(const std::filesystem::path& worldDirPath, const Chunk& chunk)
        {
                if(worldDirPath.empty())
                {
                        logError("WorldLoader::saveChunk() failed, the path to the directory in which the chunk should be saved is empty!");
                        return false;
                }

                std::filesystem::path chunkFilePath = worldDirPath / getChunkFilename(chunk.id);

                // Try to open "..worldDirPath../chunkX.dat" file and write content into it
                std::ofstream chunkFile;
                chunkFile.open(chunkFilePath, std::ios::binary | std::ios::trunc);
                if(!chunkFile.is_open())
                {
                        logError("WorldLoader::saveChunk() failed, cannot open \"%s\" file!", chunkFilePath.c_str())
                        return false;
                }

                bool res = chunk.serialize(chunkFile);
                chunkFile.close();
                return res;
        }


        // Returns a string that can be used as a name for a game world
        std::string WorldLoader::createDummyWorldName()
        {
                std::stringstream worldName;
                std::time_t currTime = std::time(nullptr);

                worldName << "World_" << std::put_time( std::localtime(&currTime), "%F %T" );
                return worldName.str();
        }

}
