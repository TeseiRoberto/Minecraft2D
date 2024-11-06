
// Contains definition of the WorldLoader class, this class is responsible for the serializaton
// and deserialization of the GameWorld class and all things related to it (chunks, players, ...)
//
// Each game world (save game) is saved in it's own directory, each directory contains:
//      - one "world.dat" file: this file contains gloabl information about the game world
//      - N "chunkX.dat" files: a file of this type contains data about the chunk with id X
//

#ifndef WORLD_LOADER_H
#define WORLD_LOADER_H

#include <string>
#include <fstream>
#include <filesystem>

#include "log.hpp"

namespace mc2d {

        class GameWorld;
        struct Chunk;

        class WorldLoader {
        public:
                WorldLoader() = delete;
                ~WorldLoader() = delete;

                static bool             loadWorld(const std::filesystem::path& worldDirPath, GameWorld& world);
                static bool             saveWorld(const std::filesystem::path& worldDirPath, GameWorld& world);

                static bool             loadChunk(const std::filesystem::path& worldDirPath, int chunkId, Chunk& chunk);
                static bool             saveChunk(const std::filesystem::path& worldDirPath, const Chunk& chunk);

                static std::string      createDummyWorldName();

        private:

                static inline std::filesystem::path     getWorldFilename()              { return std::filesystem::path("world.dat"); }
                static inline std::filesystem::path     getChunkFilename(int chunkId)   { return std::filesystem::path("chunk" + std::to_string(chunkId) + ".dat"); }

        };

}

#endif // WORLD_LOADER_H
