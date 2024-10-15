
#include <cmath>
#include "gameWorld.hpp"

#include "worldEncyclopedia.hpp"


namespace mc2d {

// ============================== [ Biome properties look-up table ] ==============================

        // Macro used to compute the number of terrain control points in function of a percentage value relative to the Chunk::width
        #define COMPUTE_CONTROL_POINTS(percentage)  static_cast<uint8_t>( std::floor((Chunk::width * percentage) / 100.0f) ) 
        
        const BiomeProperties WorldEncyclopedia::s_biomePropsLUT[] = {
         
                // PLAIN biome properties
                {       
                        "plain",                                                // Human redable biome name
                        COMPUTE_CONTROL_POINTS(30),                             // Number of terrain control points
                        (Chunk::height / 2) - 1, (Chunk::height / 2) + 1,       // Min and max terrain heights (measured in blocks)
                        -0.7f, 0.7f,                                            // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.2f,                                                   // Tree spawn probability
                        { TreeType::OAK },                                      // Spawnable tree types
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
                        { TreeType::OAK, TreeType::BIRCH },                     // Spawnable tree types
                        0.3f                                                    // Water spawn probability
                },

                // MOUNTAIN biome properties
                {       
                        "mountain",                                             // Human redable biome name
                        COMPUTE_CONTROL_POINTS(80),                             // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 4,       // Min and max terrain heights (measured in blocks)
                        -3.0f, 3.0f,                                            // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.1f,                                                   // Tree spawn probability
                        { TreeType::OAK, TreeType::BIRCH },                     // Spawnable tree types
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
                        { TreeType::SPRUCE },                                           // Spawnable tree types
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
                        { TreeType::SPRUCE },                                           // Spawnable tree types
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
                        { TreeType::SPRUCE },                                           // Spawnable tree types
                        0.0f                                                            // Water spawn probability
                },

                // JUNGLE biome properties
                {       
                        "jungle",                                                       // Human redable biome name
                        COMPUTE_CONTROL_POINTS(40),                                     // Number of terrain control points
                        (Chunk::height / 2) - 2, (Chunk::height / 2) + 2,               // Min and max terrain heights (measured in blocks)
                        -1.0f, 1.0f,                                                    // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,            // Type of blocks for the terrain layers
                        0.95f,                                                          // Tree spawn probability
                        { TreeType::JUNGLE },                                           // Spawnable tree types
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
                        { TreeType::OAK },                                      // Spawnable tree types
                        0.0f                                                    // Water spawn probability
                },

                // BIOME_TYPE_MAX (same as SUPER_FLAT biome properties) 
                {       
                        "super flat",                                           // Human redable biome name
                        2,                                                      // Number of terrain control points
                        (Chunk::height / 2), (Chunk::height / 2),               // Min and max terrain heights (measured in blocks)
                        0.0f, 0.0f,                                             // Min and max terrain slope (measured in blocks)
                        BlockType::GRASS, BlockType::DIRT, BlockType::STONE,    // Type of blocks for the terrain layers
                        0.5f,                                                   // Tree spawn probability
                        { TreeType::OAK },                                      // Spawnable tree types
                        0.0f                                                    // Water spawn probability
                }

        };


// ============================== [ Tree properties look-up table ] ==============================

        const TreeProperties WorldEncyclopedia::s_treePropsLUT[] = {
 
                // OAK_TREE tree properties
                {
                        BlockType::OAK_WOOD, BlockType::OAK_LEAF,       // Types of blocks used for log and leaf
                        1, 2,                                           // Min and max log width
                        5, 10                                           // Min and max log height
                },

                // BIRCH_TREE tree properties
                {
                        BlockType::BIRCH_WOOD, BlockType::BIRCH_LEAF,   // Types of blocks used for log and leaf
                        1, 2,                                           // Min and max log width
                        5, 10                                           // Min and max log height
                },

                // JUNGLE_TREE tree properties
                {
                        BlockType::JUNGLE_WOOD, BlockType::JUNGLE_LEAF, // Types of blocks used for log and leaf
                        1, 3,                                           // Min and max log width
                        5, 20                                           // Min and max log height
                },

                // SPRUCE_TREE tree properties
                {
                        BlockType::SPRUCE_WOOD, BlockType::SPRUCE_LEAF, // Types of blocks used for log and leaf
                        1, 2,                                           // Min and max log width
                        5, 12                                           // Min and max log height
                },

                // TREE_TYPE_MAX (same as OAK_TREE tree properties)
                {
                        BlockType::OAK_WOOD, BlockType::OAK_LEAF,       // Types of blocks used for log and leaf
                        1, 2,                                           // Min and max log width
                        5, 10                                           // Min and max log height
                }
        };

// ============================== [ Blocks properties look-up table ] ==============================

        const BlockProperties WorldEncyclopedia::s_blockPropsLUT[] = {

                // GRASS
                { true, 2 },

                // DIRT
                { true, 2 },

                // STONE
                { true, 4 },

                // COBBLESTONE
                { true, 4 },

                // GRAVEL
                { true, 2 },

                // MICELIUM
                { true, 2 },

                // SAND
                { true, 2 },

                // SANDSTONE_RAW
                { true, 4 },

                // SANDSTONE
                { true, 4 },

                // SANDSTONE_POLISHED
                { true, 4 },

                // GRASS_SNOW
                { true, 2 },

                // SNOW
                { true, 2 },

                // ICE
                { true, 2 },

                // CLAY
                { true, 2 },

                // OBSIDIAN
                { true, 8 },

                // BEDROCK
                { true, 255 },


                // ===========================[ Wood and trees ]=========================== 
                // OAK_WOOD
                { true, 3 },

                // OAK_LEAF
                { true, 2 },

                // OAK_PLANK
                { true, 3 },

                // OAK_SAPLING
                { true, 0 },

                // BIRCH_WOOD
                { true, 3 },

                // BIRCH_LEAF
                { true, 2 },

                // BIRCH_PLANK
                { true, 3 },

                // BIRCH_SAPLING
                { true, 0 },

                // JUNGLE_WOOD
                { true, 3 },

                // JUNGLE_LEAF
                { true, 2 },

                // JUNGLE_PLANK
                { true, 3 },

                // JUNGLE_SAPLING
                { true, 0 },

                // SPRUCE_WOOD
                { true, 3 },

                // SPRUCE_LEAF
                { true, 2 },

                // SPRUCE_PLANK
                { true, 3 },

                // SPRUCE_SAPLING
                { true, 0 },

                // ===========================[ Minerals ]=========================== 
                // COAL_ORE
                { true, 2 },

                // COAL_BLOCK
                { true, 2 },

                // IRON_ORE
                { true, 2 },

                // IRON_BLOCK
                { true, 2 },

                // GOLD_ORE
                { true, 2 },

                // GOLD_BLOCK
                { true, 2 },

                // DIAMOND_ORE
                { true, 2 },

                // DIAMOND_BLOCK
                { true, 2 },

                // EMERALD_ORE
                { true, 2 },

                // EMERALD_BLOCK
                { true, 2 },

                // REDSTONE_ORE
                { true, 2 },

                // REDSTONE_BLOCK
                { true, 2 },

                // LAPISLAZZUILI_ORE
                { true, 2 },

                // LAPISLAZZUILI_BLOCK
                { true, 2 },


                // ===========================[ Others ]=========================== 
                // MOSSY_COBBLESTONE
                { true, 4 },

                // MOSSY_BRICK
                { true, 4 },


                // ===========================[ Furnitures ]=========================== 
                // WORKBENCH
                { false, 2 },

                // FURNACE
                { false, 3 },

                // FURNACE_ACTIVE
                { false, 3 },

                // CHEST
                { false, 2 },

                // DOOR_OPEN_LOW
                { false, 2 },

                // DOOR_OPEN_HIGH
                { false, 2 },

                // DOOR_CLOSED
                { true, 2 },

                // TRAP_OPEN
                { false, 2 },

                // TRAP_CLOSED
                { true, 2 },

                // STAIR
                { false, 2 },

                // BED_END
                { false, 2 },

                // BED_START
                { false, 2 },


                // ===========================[ Others ]=========================== 
                // TNT
                { true, 0 },

                // WOOL
                { true, 2 },

                // PISTON
                { true, 3 },

                // ENCHANTMENT_BENCH
                { false, 3 },

                
                // BRICK
                { true, 4 },

                // GLASS
                { true, 2 },

                // LIBRARY
                { false, 3 },

                // FLOWER_RED
                { false, 0 },

                // FLOWER_YELLOW
                { false, 0 },

                // SHRUB
                { false, 0 },

                // MUSHROOM_RED
                { false, 0 },

                // MUSHROOM_BROWN
                { false, 0 },


                // ===========================[ Liquids ]=========================== 
                // WATER
                { true, 0 },

                // LAVA
                { true, 0 },

                // AIR
                { false, 0 },

                // BLOCK_TYPE_MAX (same as AIR block properties)
                { false, 0 },
        };

}

