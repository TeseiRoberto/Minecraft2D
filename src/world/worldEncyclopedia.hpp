
// Contains definition of the WorldEncyclopedia class this class is responsible for defining
// all the properties of the game world related concepts.
// In particular this class holds static look-up tables that can be used to retrieve properties of elements
// in the game world such as: blocks properties, biomes properties, trees properties, ...
//

#ifndef WORLD_ENCYCLOPEDIA
#define WORLD_ENCYCLOPEDIA

#include <string>
#include <vector>
#include <cstdint>

#include "blockTypes.hpp"
#include "biomeTypes.hpp"

namespace mc2d {


        // Enumeration of all tree types available in the game
        enum class TreeType : uint32_t
        {
                OAK,
                BIRCH,
                JUNGLE,
                SPRUCE,

                TREE_TYPE_MAX   // Keep me last!
        };


        // Defines all the properties of a certain tree type
        struct TreeProperties
        {
                BlockType               logBlockType;                   // Type of block used for the tree log
                BlockType               leafBlockType;                  // Type of block used for the tree leaf
                uint8_t                 minWidth;                       // Minimum width of the tree log
                uint8_t                 maxWidth;                       // Maximum width of the tree log
                uint8_t                 minHeight;                      // Minimum height of the tree log
                uint8_t                 maxHeight;                      // Maximum height of the tree log
        };


        // Defines all the properties of a certain biome type
        struct BiomeProperties {
                std::string             name;                           // Human redable version of the biome type

                uint8_t                 terrainControlPointsNum;        // Number of control points used to generate the terrain for this biome type

                uint8_t                 minTerrainHeight;               // Minimum terrain height for the first layer of blocks (measured in blocks)
                uint8_t                 maxTerrainHeight;               // Maximum terrain height for the first layer of blocks (measured in blocks)

                float                   minTerrainSlope;                // Minimum value of slope that terrain of this biome can have (measured in blocks)
                float                   maxTerrainSlope;                // Maximum value of slope that terrain of this biome can have (measured in blocks)

                BlockType               firstLayerBlockType;            // Type of block used for the superficial terrain layer
                BlockType               secondLayerBlockType;           // Type of block used under the superficial terrain layer
                BlockType               thirdLayerBlockType;            // Type of block used to fill the underground terrain
        
                float                   treeSpawnProbability;
                std::vector<TreeType>   spawnableTrees;                 // Types of trees that can be spawned in this biome

                float                   waterSpawnProbability;
        };


        // Defines all the properties of a certain block type
        struct BlockProperties {
                bool                    collidable;
                uint32_t                hardness;
        };


        class WorldEncyclopedia {
        public:
                WorldEncyclopedia() = delete;
                ~WorldEncyclopedia() = delete;

                static inline const BiomeProperties&    getBiomeProperties(const BiomeType type)        { return s_biomePropsLUT[ static_cast<uint32_t>(type) ]; }
                static inline const TreeProperties&     getTreeProperties(const TreeType type)          { return s_treePropsLUT[ static_cast<uint32_t>(type) ]; }
                static inline const BlockProperties&    getBlockProperties(const BlockType type)        { return s_blockPropsLUT[ static_cast<uint32_t>(type) ]; }

        private:

                static const BiomeProperties    s_biomePropsLUT[];
                static const TreeProperties     s_treePropsLUT[];
                static const BlockProperties    s_blockPropsLUT[];
        };


}

#endif // WORLD_ENCYCLOPEDIA
