
// Defines the WorldGenerator class, this class is responsible for the generation of
// game worlds and chunks

#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <string>
#include <cstring>
#include <random>
#include <algorithm>

#include "gameWorld.hpp"
#include "worldEncyclopedia.hpp"

namespace mc2d {

        struct Chunk;

        class WorldGenerator {
        public:
                WorldGenerator() = delete;
                ~WorldGenerator() = delete;

                static GameWorld        generateRandomWorld(const std::string& seed, uint32_t initialChunksNum);
                static GameWorld        generateRandomWorld(const unsigned seed, uint32_t initialChunksNum);
                static GameWorld        generateFlatWorld(uint32_t initialChunksNum);

                static Chunk            generateRandomChunk(const unsigned seed, BiomeType biome = BiomeType::BIOME_TYPE_MAX);
                static Chunk            generateFlatChunk();

        private:

                struct Terrain {
                        size_t                  width;                  // Terrain width (measured in blocks)
                        size_t                  height;                 // Terrain height (measured in blocks)
                        std::vector<size_t>     terrainHeightValues;    // Height values (relative to the blocks vector) for most superficial terrain blocks
                        std::vector<BlockType>  blocks;                 // Blocks that makes up the terrain
                };

                using RNG = std::mt19937;

                static Terrain          generateRandomTerrain(RNG& rng, size_t width, size_t height, const BiomeProperties& biome);
                static void             addTreesToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome);
                static void             addWaterToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome);
                static void             addMineralsToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome);
        };

}

#endif // WORLD_GENERATOR_H
