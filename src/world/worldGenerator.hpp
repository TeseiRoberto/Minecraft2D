
// Defines the WorldGenerator class, this class is responsible for the generation of
// game worlds and chunks

#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <string>
#include <cstring>
#include <random>
#include <algorithm>

#include "biome.hpp"
#include "gameWorld.hpp"

namespace mc2d {

        class GameWorld;
        struct Chunk;


        class WorldGenerator {
        public:
                WorldGenerator() = delete;

                static GameWorld        generateRandomWorld(const std::string& seed, uint32_t initialChunksNum);
                static GameWorld        generateRandomWorld(const unsigned seed, uint32_t initialChunksNum);
                static GameWorld        generateFlatWorld(uint32_t initialChunksNum);

                static Chunk            generateRandomChunk(const unsigned seed);
                static Chunk            generateFlatChunk();
        };

}

#endif // WORLD_GENERATOR_H
