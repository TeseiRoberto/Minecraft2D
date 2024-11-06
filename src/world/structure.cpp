
#include "structure.hpp"

namespace mc2d {

        
        Structure::Structure() : m_spawnPoint(glm::vec2(0.0f)), m_blocks( {} ), m_width(0), m_height(0), m_origin(glm::vec2(0.0f))
        {}


        Structure::Structure(glm::vec2 spawnPoint, std::vector<BlockType>& blocks, size_t width, size_t height, glm::vec2 origin) :
                m_spawnPoint(spawnPoint), m_width(0), m_height(0), m_origin(origin)
        {
                if(width * height != blocks.size())
                {
                        logWarn("Structure::Structure() failed, structure width is: %lu and height is: %lu but blocks number is: %lu", width, height, blocks);
                } else {
                        m_width = width;
                        m_height = height;
                        m_blocks = blocks;
                }
        }


        // Writes the structure data in the given file
        // @file: output file stream in which structure data will be written
        // @returns: true if serialization is successfull, false otherwise
        bool Structure::serialize(std::ofstream& file) const
        {
                if(!file.good())
                {
                        logError("Structure::serialize() failed, the given file stream is broken!");
                        return false;
                }

                // First we save the spawn point of the structure
                file << m_spawnPoint.x << ' ' << m_spawnPoint.y << ' ';

                // Then the width and the height
                file << m_width << ' ' << m_height << ' ';

                // Then the origin
                file << m_origin.x << ' ' << m_origin.y << ' ';

                if(!file.good())
                {
                        logError("Structure::serialize() failed, cannot write structure properties (spawn point, dimensions and/or structure's origin)");
                        return false;
                }

                // And then we save all the blocks
                for(auto b = m_blocks.begin(); b != m_blocks.end() && file.good(); ++b)
                        file << static_cast<uint32_t>(*b) << ' ';

                file << '\n';
                return file.good();
        }


        // Reads structure data from the given file
        // @file: input file stream from which structure data will be read
        // @returns: true if deserialization is successfull, false otherwise
        bool Structure::deserialize(std::ifstream& file)
        {
                if(!file.good())
                {
                        logError("Structure::deserialize() failed, the given file stream is broken!");
                        return false;
                }

                // Declare temporary variables to hold the deserialized data
                glm::vec2 spawnPoint(0.0f);
                glm::vec2 origin(0);
                size_t width = 0;
                size_t height = 0;
                std::vector<BlockType> blocks;

                // First we read the spawn point of the structure
                file >> spawnPoint.x;
                file >> spawnPoint.y;

                // Then we read the dimensions
                file >> width;
                file >> height;

                // Then the origin
                file >> origin.x;
                file >> origin.y;

                if(!file.good())
                {
                        logError("Structure::deserialize() failed, cannot read structure properties (spawn point, dimensions and/or structure's origin)!");
                        return false;
                }

                // And then we read all the blocks that makes up the structure
                blocks.reserve(width * height);
                for(size_t i = 0; i < width * height; ++i)
                {
                        uint32_t currBlockType;
                        file >> currBlockType;

                        if(!file.good())
                        {
                                logError("Structure::deserialize() failed, cannot read the blocks that makes up the structure!");
                                return false;
                        }

                        blocks.push_back( static_cast<BlockType>(currBlockType) );
                }

                // If we got to this point then deserialization has been successfull and we can use such data to initialize this structure
                m_spawnPoint = spawnPoint;
                m_width = width;
                m_height = height;
                m_origin = origin;
                m_blocks = std::move(blocks);

                logWarn("LOADED STRUCTURE DATA: spawnPoint (%f, %f), dimensions: (%lu, %lu), origin: (%f, %f)\nBlocks data:",
                                m_spawnPoint.x, m_spawnPoint.y, m_width, m_height, m_origin.x, m_origin.y);

                for(auto& b : m_blocks)
                        logWarn("\t%u", static_cast<uint32_t>(b) );

                return true;
        }

}

