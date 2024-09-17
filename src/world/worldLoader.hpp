
// Contains definition of the WorldLoader class, this class is responsible for the serializaton
// and deserialization of the GameWorld class and all things related to it (chunks, players, ...)
//
// Each game world (save game) is saved in it's own directory, each directory contains:
//      - one "world.dat" file: this file contains gloabl information about the game world
//      - N "chunkX.dat" files: a file of this type contains data about the chunk with id X
//

#ifndef WORLD_LOADER_H
#define WORLD_LOADER_H

#include <fstream>
#include <filesystem>

#include "logging.hpp"

namespace mc2d {

        class GameWorld;
        struct Chunk;

        class WorldLoader {
        public:
                WorldLoader() = delete;

                static GameWorld        loadWorld(const std::filesystem::path& worldPath);
                static void             saveWorld(const std::filesystem::path& worldPath, const GameWorld& world);

                static Chunk            loadChunk(const std::filesystem::path& worldPath, int chunkId);
                static void             saveChunk(const std::filesystem::path& worldPath, const Chunk& chunk);

        private:

                // TODO: add implementation
                //static std::vector<Blocks>      deserializeBlocks();
                //static Entity                   deserializeEntity();


        };

}

#endif // WORLD_LOADER_H
