
#include "worldGenerator.hpp"

namespace mc2d {


        // Returns a world in which chunks are generated randomly
        // @seed: string from which a number will be generated and used as seed for std::srand
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
        // @seed: the value for std::srand
        // @initialChunksNum: the number of chunks that will be generated for this world
        GameWorld WorldGenerator::generateRandomWorld(const unsigned seed, uint32_t initialChunksNum)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");

                if(initialChunksNum == 0)
                        initialChunksNum = 3;

                int numOfLeftChunks = (int) ( ((float) initialChunksNum / 2.0f) );      // Number of chunks on the left of the root chunk
                int id = -1 * numOfLeftChunks;

                std::vector<Chunk> chunks;
                for(uint32_t i = 0; i < initialChunksNum; ++i)
                {
                        Chunk c = generateRandomChunk(seed + id);
                        c.id = id++;

                        chunks.push_back(std::move(c));
                }

                return GameWorld(std::move(chunks));
        }
        

        // Returns a world in which all chunks are flat
        // @initialChunksNum: the number of chunks that will be generated for this world
        GameWorld WorldGenerator::generateFlatWorld(uint32_t initialChunksNum)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");

                if(initialChunksNum == 0)
                        initialChunksNum = 3;

                int numOfLeftChunks = (int) ( ((float) initialChunksNum / 2.0f) );      // Number of chunks on the left of the root chunk
                int id = -1 * numOfLeftChunks;

                std::vector<Chunk> chunks;
                for(uint32_t i = 0; i < initialChunksNum; ++i)
                {
                        Chunk c = generateFlatChunk();
                        c.id = id++;

                        chunks.push_back(std::move(c));
                }

                return GameWorld(std::move(chunks));
        }
        

        // Generates a chunk with a random terrain
        // @seed: used to initialize the (pseudo) random number generator
        // (Note: terrain generation uses a linear interpolation of terrain control points, those points are distributed 
        // equally on the x axis and pseudo randomly on the y axis)
        Chunk WorldGenerator::generateRandomChunk(unsigned seed)
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");
                
                std::mt19937 gen(seed);

                // Create a new chunk with a random biome type
                Chunk newChunk {};
                newChunk.blocks.resize(Chunk::width * Chunk::height, BlockType::AIR);
                newChunk.biome = static_cast<BiomeType>( gen() % ((uint32_t) BiomeType::BIOME_TYPE_MAX - 2) );

                // Get biome properties using the biome look up table
                const BiomeProperties& biomeProps = Biome::getBiomeProperties(newChunk.biome);

                // Setup distributions needed for terrain generation
                std::uniform_int_distribution heightDistrib(biomeProps.minTerrainHeight, biomeProps.maxTerrainHeight);
                std::uniform_real_distribution slopeDistrib(biomeProps.minTerrainSlope, biomeProps.maxTerrainSlope);

                // Generate control points for the terrain
                float lastHeight = static_cast<float>(heightDistrib(gen));

                size_t terrainControlPointsNum = 2;
                if(biomeProps.terrainControlPointsNum > 2)
                        terrainControlPointsNum = biomeProps.terrainControlPointsNum;

                float controlPointsSpacing = (float) Chunk::width / (float) (terrainControlPointsNum - 1);
                glm::vec2 controlPoints[biomeProps.terrainControlPointsNum];

                for(size_t i = 0; i < terrainControlPointsNum; ++i)
                {
                        lastHeight = lastHeight + slopeDistrib(gen);

                        controlPoints[i].x = controlPointsSpacing * i;
                        controlPoints[i].y = lastHeight;
                }

                // Generate terrain (using a linear interpolation of the terrain control points generated previously)
                glm::vec2* controlPointA = controlPoints;
                glm::vec2* controlPointB = controlPoints + 1;

                for(size_t x = 0; x < Chunk::width; ++x)
                {
                        // Handle passage between the different control points
                        if((float) x > controlPointA->x && controlPointB + 1 < controlPoints + terrainControlPointsNum)
                        {
                                controlPointA = controlPointB;
                                ++controlPointB;
                        }

                        float deltaXa = (float) x - controlPointA->x;
                        float deltaXb = (float) x - controlPointB->x;
                        float deltaX = controlPointA->x - controlPointB->x;

                        float height = (deltaXb / deltaX) * controlPointA->y;
                        height -= (deltaXa / deltaX) * controlPointB->y;

                        // Compute y index relative to the chunk's blocks array
                        size_t yIndex = std::clamp(static_cast<size_t>(height), 0Lu, Chunk::height - 1Lu);

                        newChunk.blocks[(yIndex * Chunk::width) + x] = biomeProps.firstLayerBlockType;

                        for(uint8_t y = yIndex + 1; y < yIndex + 3 && y < Chunk::height; ++y)
                                newChunk.blocks[(y * Chunk::width) + x] = biomeProps.secondLayerBlockType;

                        for(uint8_t y = yIndex + 3; y < Chunk::height; ++y)
                                newChunk.blocks[(y * Chunk::width) + x] = biomeProps.thirdLayerBlockType;
                }

                // Add final bedrock layer
                std::memset(&(newChunk.blocks[ (Chunk::height - 1) * Chunk::width ]), (uint8_t) BlockType::BEDROCK, Chunk::width * sizeof(BlockType));
                return newChunk;
        }


        // Generates a flat chunk that contains only grass, dirt and stone
        Chunk WorldGenerator::generateFlatChunk()
        {
                static_assert(Chunk::width >= 8 && Chunk::height >= 8, "WorldGenerator requires chunk dimensions equal or greater than 8x8 to work properly");
                
                Chunk newChunk {};
                newChunk.biome = BiomeType::SUPER_FLAT;

                newChunk.blocks.reserve(Chunk::width * Chunk::height);
                uint32_t offset = 0;

                // Half of the chunk must be filled with air blocks
                std::memset(newChunk.blocks.data(), (uint8_t) BlockType::AIR, (Chunk::height / 2) * Chunk::width * sizeof(BlockType));
                offset += ((Chunk::height / 2) * Chunk::width) * sizeof(BlockType);

                // then we add a layer of grass
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::GRASS, Chunk::width * sizeof(BlockType));
                offset += Chunk::width * sizeof(BlockType);;

                // then we add two layers of dirt
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::DIRT, Chunk::width * 2 * sizeof(BlockType));
                offset += Chunk::width * 2 * sizeof(BlockType);;

                // and then we add stone blocks until the end of the chunk
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::STONE, ((Chunk::height / 2) - 3) * Chunk::width * sizeof(BlockType));
        
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

}

