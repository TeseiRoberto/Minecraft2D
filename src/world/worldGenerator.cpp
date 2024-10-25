
#include "worldGenerator.hpp"

namespace mc2d {


        // Returns a world in which chunks are generated randomly
        // @seed: string from which a number will be generated and used as seed for the (pseudo) random number generator
        // @initialChunksNum: the number of chunks that will be generated for this world
        GameWorld WorldGenerator::generateRandomWorld(const std::string& seed, uint32_t initialChunksNum)
        {
                // TODO: Implement some "serious" algorithm to compute a seed from the given string
                unsigned value = 0;
                for(const char& c : seed)
                        value += (unsigned) (3.1415f * (float) c);
        
                return generateRandomWorld(value, initialChunksNum);
        }


        // Returns a world in which all chunks are generated randomly
        // @seed: used to initialize the (pseudo) random number generator
        // @initialChunksNum: the number of chunks that will be generated for this world
        GameWorld WorldGenerator::generateRandomWorld(const unsigned seed, uint32_t initialChunksNum)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");

                if(initialChunksNum == 0)
                        initialChunksNum = 3;
                
                uint32_t leftChunksNum = initialChunksNum / 2;                  // Number of chunks on the left of the root chunk
                int currId = -1 * leftChunksNum;

                // Create random chunks
                std::vector<Chunk> chunks;
                for(uint32_t i = 0; i < initialChunksNum; ++i)
                {
                        Chunk c = generateRandomChunk(seed + currId);
                        c.id = currId++;

                        chunks.push_back(std::move(c));
                }

                return GameWorld(std::move(chunks), seed);
        }
        

        // Returns a world in which all chunks are flat
        // @initialChunksNum: the number of chunks that will be generated for this world
        GameWorld WorldGenerator::generateFlatWorld(uint32_t initialChunksNum)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");

                if(initialChunksNum == 0)
                        initialChunksNum = 3;

                uint32_t leftChunksNum = initialChunksNum / 2;                  // Number of chunks on the left of the root chunk
                int currId = -1 * leftChunksNum;

                std::vector<Chunk> chunks;
                for(uint32_t i = 0; i < initialChunksNum; ++i)
                {
                        Chunk c = generateFlatChunk();
                        c.id = currId++;

                        chunks.push_back(std::move(c));
                }

                return GameWorld(std::move(chunks), 0);
        }

        
        // Generates a chunk with a random terrain
        // @seed: used to initialize the (pseudo) random number generator
        // @biome: biome type to be used for the generated chunk (will be chosen randomly if BIOME_TYPE_MAX is given)
        Chunk WorldGenerator::generateRandomChunk(unsigned seed, BiomeType biome)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");
                
                std::mt19937 gen(seed);

                // If biome has not been specified (it is the default parameter) then choose a random one
                if(biome == BiomeType::BIOME_TYPE_MAX)
                        biome = static_cast<BiomeType>( gen() % ((uint32_t) BiomeType::BIOME_TYPE_MAX - 1) );

                // Retrieve biome properties
                const BiomeProperties& biomeProps = WorldEncyclopedia::getBiomeProperties(biome);

                // Generate random terrain for the chunk
                Terrain t = generateRandomTerrain(gen, Chunk::width, Chunk::height, biomeProps);

                // Add stuff to the terrain
                //addWaterToTerrain(gen, t, biomeProps);
                //addTreesToTerrain(gen, t, biomeProps);
                //addMineralsToTerrain(gen, t, biomeProps);

                // Create new chunk
                Chunk newChunk = {};
                newChunk.biome = biome;
                newChunk.blocks = std::move(t.blocks);
                
                return newChunk;
        }


        // Generates a flat chunk that contains only grass, dirt and stone
        Chunk WorldGenerator::generateFlatChunk()
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");
                
                Chunk newChunk {};
                newChunk.biome = BiomeType::SUPER_FLAT;

                const BiomeProperties& biomeProps = WorldEncyclopedia::getBiomeProperties(BiomeType::SUPER_FLAT);

                newChunk.blocks.resize(Chunk::width * Chunk::height, BlockType::AIR);
                uint32_t offset = ((Chunk::height / 2) * Chunk::width) * sizeof(BlockType);

                // Add one row of first layer block type
                std::memset(newChunk.blocks.data() + offset, (uint8_t) biomeProps.firstLayerBlockType, Chunk::width * sizeof(BlockType));
                offset += Chunk::width * sizeof(BlockType);;

                // Then add two rows of second layer block type
                std::memset(newChunk.blocks.data() + offset, (uint8_t) biomeProps.secondLayerBlockType, Chunk::width * 2 * sizeof(BlockType));
                offset += Chunk::width * 2 * sizeof(BlockType);;

                // And then add third layer block type until the end of chunk
                std::memset(newChunk.blocks.data() + offset, (uint8_t) biomeProps.thirdLayerBlockType, ((Chunk::height / 2) - 3) * Chunk::width * sizeof(BlockType));
        
                // Add final bedrock layer
                std::memset(&(newChunk.blocks[ (Chunk::height - 1) * Chunk::width ]), (uint8_t) BlockType::BEDROCK, Chunk::width * sizeof(BlockType));

                // TODO: REMOVE ME: debug code that creates vertical lines of TNT blocks (to denote the chunk limits)
                for(uint32_t i = 0; i < Chunk::height; ++i)
                {
                        newChunk.blocks[i * Chunk::width] = BlockType::TNT;
                        newChunk.blocks[(i * Chunk::width) + Chunk::width - 1] = BlockType::TNT;
                }

                return newChunk;
        }


        // Generates a random terrain using the given properties
        // @rng: pseudo random number generator to be used to generate terrain
        // @width: the width of terrain
        // @height: the height of terrain
        // @biome: specify the properties and the constraints that the generated terrain must have
        // @returns: the generated terrain
        // (Note: terrain generation uses a linear interpolation of terrain control points, those points are distributed 
        // equally on the x axis and pseudo randomly on the y axis(even if y is bound in range [minTerrainHeight, maxTerrainHeight]))
        WorldGenerator::Terrain WorldGenerator::generateRandomTerrain(RNG& rng, size_t width, size_t height, const BiomeProperties& biome)
        {
                Terrain t = {};
                t.width = width;
                t.height = height;
                t.terrainHeightValues.resize(width, 0);
                t.blocks.resize(width * height, BlockType::AIR);

                std::uniform_int_distribution heightDistrib(biome.minTerrainHeight, biome.maxTerrainHeight);
                std::uniform_real_distribution slopeDistrib(biome.minTerrainSlope, biome.maxTerrainSlope);

                float lastHeight = static_cast<float>(heightDistrib(rng));

                // Generate control points for the terrain
                size_t controlPointsNum = (biome.terrainControlPointsNum > 2 ? biome.terrainControlPointsNum : 2);
                float controlPointsSpacing = (float) t.width / (float) (controlPointsNum - 1);
                glm::vec2 controlPoints[controlPointsNum];

                for(size_t i = 0; i < controlPointsNum; ++i)
                {
                        float currSlope = slopeDistrib(rng) * BLOCK_HEIGHT;
                        lastHeight = std::clamp(lastHeight + currSlope, (float) biome.minTerrainHeight, (float) biome.maxTerrainHeight);

                        controlPoints[i].x = (float) i * controlPointsSpacing;
                        controlPoints[i].y = lastHeight;
                }

                // Generate terrain (using a linear interpolation of the terrain control points generated previously)
                glm::vec2* controlPointA = controlPoints;
                glm::vec2* controlPointB = controlPoints + 1;

                for(size_t x = 0; x < t.width; ++x)
                {
                        // Handle passage between the different control points
                        if((float) x > controlPointA->x && controlPointB + 1 < controlPoints + controlPointsNum)
                        {
                                controlPointA = controlPointB;
                                ++controlPointB;
                        }

                        float deltaXa = (float) x - controlPointA->x;
                        float deltaXb = controlPointB->x - (float) x;
                        float deltaX = controlPointB->x - controlPointA->x;

                        float height = (deltaXa * controlPointB->y) + (deltaXb * controlPointA->y);
                        height /= deltaX;

                        // Compute y index relative to the terrain's blocks vector
                        size_t yIndex = t.height - 1 - (size_t) std::clamp(height, 0.0f, (float) (t.height - 1));
                        t.terrainHeightValues[x] = yIndex;

                        t.blocks[(yIndex * t.width) + x] = biome.firstLayerBlockType;

                        for(size_t y = yIndex + 1; y < yIndex + 3 && y < t.height; ++y)
                                t.blocks[(y * t.width) + x] = biome.secondLayerBlockType;

                        for(size_t y = yIndex + 3; y < t.height; ++y)
                                t.blocks[(y * t.width) + x] = biome.thirdLayerBlockType;
                }

                // Add final bedrock layer
                std::memset(&(t.blocks[ (t.height - 1) * t.width ]), (uint8_t) BlockType::BEDROCK, t.width * sizeof(BlockType));

                return t;
        }


        void WorldGenerator::addTreesToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome)
        {
                // TODO: Add implementation...
        }


        void WorldGenerator::addWaterToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome)
        {
                // TODO: Add implementation...
        }


        void WorldGenerator::addMineralsToTerrain(RNG& rng, Terrain& t, const BiomeProperties& biome)
        {
                // TODO: Add implementation...
        }

}

