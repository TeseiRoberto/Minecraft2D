
// Contains definition of BiomeType enum, BiomeProperties struct and Biome class.
//
// BiomeType is simply an enumeration of all the biome types available in the game.
//
// BiomeProperties is used to define all the properties of a certain biome type, those 
// properties are used during world generation.
//
// The Biome class holds a look up table that enables us to get all the properties of a biome
// using the biome type.
//

#ifndef BIOME_H
#define BIOME_H

#include <string>
#include <cstdint>

#include "block.hpp"
#include "gameWorld.hpp"

namespace mc2d {


        enum class BiomeType : uint32_t {
                PLAIN = 0,              // Mostly flat terrain (made of: grass, dirt, stone)
                FOREST,                 // Hilly terrain (made of: grass, dirt, stone) with oak and birch trees
                MOUNTAIN,               // Pretty steep terrain (made of: grass dirt and stone)
                SNOWY_PLAIN,            // Mostly flat terrain (made of: snowy grass, dirt, stone)
                SNOWY_FOREST,           // Hilly terrain (made of: snowy grass, dirt, stone) with spruce trees
                SNOWY_MOUNTAIN,         // Pretty steep terrain (made of: snowy grass dirt and stone)
        
                SUPER_FLAT,             // Super flat terrain (made of: grass, dirt and stone)
                BIOME_TYPE_MAX          // Keep me last!!!
        };


        struct BiomeProperties {
                std::string     name;                           // Human redable version of the biome type

                uint8_t         terrainControlPointsNum;        // Number of control points used to generate the terrain for this biome type

                uint8_t         minTerrainHeight;               // Minimum terrain height for the first layer of blocks (measured in blocks)
                uint8_t         maxTerrainHeight;               // Maximum terrain height for the first layer of blocks (measured in blocks)

                float           minTerrainSlope;                // Minimum value of slope that terrain of this biome can have (measured in blocks)
                float           maxTerrainSlope;                // Maximum value of slope that terrain of this biome can have (measured in blocks)

                BlockType       firstLayerBlockType;            // Type of block used for the superficial terrain layer
                BlockType       secondLayerBlockType;           // Type of block used under the superficial terrain layer
                BlockType       thirdLayerBlockType;            // Type of block used to fill the underground terrain
        
                float           treeSpawnProbability;
                float           waterSpawnProbability;
        };


        class Biome {
        public:
                Biome() = delete;
                ~Biome() = delete;

                static inline const BiomeProperties&    getBiomeProperties(const BiomeType type)        { return s_biomePropertiesLUT[ static_cast<uint32_t>(type) ]; }

        private:
                static const BiomeProperties    s_biomePropertiesLUT[];
        };

}

#endif // BIOME_H

