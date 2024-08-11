
#include "biome.hpp"

namespace mc2d {

        // Macro use do to compute the number of terrain control points (given a percentage value relative to the Chunk::width)
        #define COMPUTE_CONTROL_POINTS(percentage)  static_cast<uint8_t>( std::floor((Chunk::width * percentage) / 100.0f) ) 


        // Definition of the biome properties look up table
        // (If you add an element make sure that the order with BiomeType enum is respected, the enum value is used as an index into this array!!!)
        const BiomeProperties Biome::s_biomePropertiesLUT[] = {
 
                // PLAIN biome properties
                {       
                        "plain",                                                // Human redable biome name
                        COMPUTE_CONTROL_POINTS(30),                             // Number of terrain control points
                        (Chunk::height / 2) - 1, (Chunk::height / 2) + 1,       // Min and max terrain heights (measured in blocks)
                        -0.7f, 0.7f,                                            // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.2f,                                                   // Tree spawn probability
                        0.2f                                                    // Water spawn probability
                },

                // FOREST biome properties
                {       
                        "forest",                                               // Human redable biome name
                        COMPUTE_CONTROL_POINTS(40),                             // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 2,       // Min and max terrain heights (measured in blocks)
                        -1.0f, 1.0f,                                            // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.8f,                                                   // Tree spawn probability
                        0.3f                                                    // Water spawn probability
                },

                // MOUNTAIN biome properties
                {       
                        "mountain",                                             // Human redable biome name
                        COMPUTE_CONTROL_POINTS(80),                             // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 4,           // Min and max terrain heights (measured in blocks)
                        -3.0f, 3.0f,                                            // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.1f,                                                   // Tree spawn probability
                        0.0f                                                    // Water spawn probability
                },

                // SNOWY_PLAIN biome properties
                {       
                        "snowy plain",                                                  // Human redable biome name
                        COMPUTE_CONTROL_POINTS(30),                                     // Number of terrain control points
                        (Chunk::height / 2) - 1, (Chunk::height / 2) + 1,               // Min and max terrain heights (measured in blocks)
                        -0.7f, 0.7f,                                                    // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS_SNOW, BlockType::DIRT, BlockType::STONE,       // Type of blocks for the terrain layers
                        0.2f,                                                           // Tree spawn probability
                        0.2f                                                            // Water spawn probability
                },

                // SNOWY_FOREST biome properties
                {       
                        "snowy forest",                                                 // Human redable biome name
                        COMPUTE_CONTROL_POINTS(40),                                     // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 2,               // Min and max terrain heights (measured in blocks)
                        -1.0f, 1.0f,                                                    // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS_SNOW, BlockType::DIRT, BlockType::STONE,       // Type of blocks for the terrain layers
                        0.8f,                                                           // Tree spawn probability
                        0.3f                                                            // Water spawn probability
                },

                // SNOWY_MOUNTAIN biome properties
                {       
                        "snowy mountain",                                               // Human redable biome name
                        COMPUTE_CONTROL_POINTS(80),                                     // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 4,               // Min and max terrain heights (measured in blocks)
                        -3.0f, 3.0f,                                                    // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS_SNOW, BlockType::DIRT, BlockType::STONE,       // Type of blocks for the terrain layers
                        0.1f,                                                           // Tree spawn probability
                        0.0f                                                            // Water spawn probability
                },
                
                // SUPER_FLAT biome properties
                {       
                        "super flat",                                           // Human redable biome name
                        2,                                                      // Number of terrain control points
                        (Chunk::height / 2), (Chunk::height / 2),               // Min and max terrain heights (measured in blocks)
                        0.0f, 0.0f,                                             // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.5f,                                                   // Tree spawn probability
                        0.0f                                                    // Water spawn probability
                },
                
                // BIOME_TYPE_MAX = SUPER_FLAT
                {       
                        "super flat",                                           // Human redable biome name
                        2,                                                      // Number of terrain control points
                        (Chunk::height / 2), (Chunk::height / 2),               // Min and max terrain heights (measured in blocks)
                        0.0f, 0.0f,                                             // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.5f,                                                   // Tree spawn probability
                        0.0f                                                    // Water spawn probability
                }
        };

}
