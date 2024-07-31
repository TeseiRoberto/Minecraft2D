
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
        

        // Generates a completely random (nonsense) chunk
        // @seed: the value for std::srand
        Chunk WorldGenerator::generateRandomChunk(unsigned seed)
        {
                std::srand(seed);

                Chunk newChunk {};
                newChunk.blocks.reserve(Chunk::width * Chunk::height);
                for(uint32_t y = 0; y < Chunk::height; y++)
                {
                        for(uint32_t x = 0; x < Chunk::width; x++)
                        {
                                if(std::rand() % 2 == 0)
                                        newChunk.blocks[(y * Chunk::width) + x] = BlockType::AIR;
                                else
                                        newChunk.blocks[(y * Chunk::width) + x] = BlockType::GRASS;
                        }
                }

                return newChunk;
        }


        // Generates a flat chunk that contains only grass, dirt and stone
        Chunk WorldGenerator::generateFlatChunk()
        {
                Chunk newChunk {};

                newChunk.blocks.reserve(Chunk::width * Chunk::height);
                uint32_t offset = 0;

                // Half of the chunk must be filled with air blocks
                std::memset(newChunk.blocks.data(), (uint8_t) BlockType::AIR, (Chunk::height / 2) * Chunk::width);
                offset += ((Chunk::height / 2) * Chunk::width);

                // then we add a layer of grass
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::GRASS, Chunk::width);
                offset += Chunk::width;

                // then we add two layers of dirt
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::DIRT, Chunk::width * 2);
                offset += Chunk::width * 2;

                // and then we add stone blocks until the end of the chunk
                std::memset(newChunk.blocks.data() + offset, (uint8_t) BlockType::STONE, ((Chunk::height / 2) - 3)  * Chunk::width );
        
                // TODO: REMOVE ME: debug code that creates vertical lines of TNT blocks (to denote the chunk limits)
                for(uint32_t i = 0; i < Chunk::height; ++i)
                {
                        newChunk.blocks[i * Chunk::width] = BlockType::TNT;
                        newChunk.blocks[(i * Chunk::width) + Chunk::width - 1] = BlockType::TNT;
                }

                return newChunk;
        }

}

