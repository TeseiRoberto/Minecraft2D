
#include "tileset.hpp"

namespace mc2d {

        Tileset::Tileset() : m_rows(0), m_cols(0), m_tileWidth(0), m_tileHeight(0), m_textureId(0)
        {}


        // Loads an image and creates a texture array to be used as a tileset
        // @filepath: path to the image file that must be loaded to create the tileset
        // @rows: number of rows in which the image must be split
        // @cols: number of columns in which the image must be split
        // @tileWidth: the width of a single tile
        // @tileHeight: the height of a single tile
        // @returns: true if the tileset gets loaded correctly, false otherwise
        bool Tileset::load(const std::string& filepath, uint32_t rows, uint32_t cols, uint32_t tileWidth, uint32_t tileHeight)
        {
                stbi_set_flip_vertically_on_load(true); // TODO: move this somewhere else

                // Check that opengl can hold a texture array containing the tiles for this tileset
                if(rows * cols > GL_MAX_ARRAY_TEXTURE_LAYERS)
                {
                        logError("Tileset::load() failed, the size of the texture array needed for this tileset is too big; the maximum size is: %u!", GL_MAX_ARRAY_TEXTURE_LAYERS);
                        return false;
                }

                unsigned char* imgData = nullptr;
                int imgWidth;
                int imgHeight;
                int imgChannelsNum;

                // Load image data using stbi
                imgData = stbi_load(filepath.c_str(), &imgWidth, &imgHeight, &imgChannelsNum, 0);
                if(imgData == NULL)
                {
                        logError("Tileset::load() failed, stbi_image could not load image \"%s\"!", filepath.c_str());
                        return false;
                }

                // Check that image is big enough to extract the specified amount of tiles
                if(imgWidth < tileWidth * cols || imgHeight < tileHeight * rows)
                {
                        logError("Tileset::load() failed, loaded image is %dx%d but the properties specified for the tileset requires an image of %ux%u!",
                                        imgWidth, imgHeight, tileWidth * cols, tileHeight * rows);
                        stbi_image_free(imgData);
                        return false;
                }
                
                bool res = createTextureArray(imgData, imgChannelsNum, rows, cols, tileWidth, tileHeight);
                stbi_image_free(imgData);
                return res;
        }


        // Destroyes the OpenGL texture array associated to the tileset
        void Tileset::unload()
        {
                if(isInit())
                {
                        glDeleteTextures(1, &m_textureId);
                        m_textureId = 0;

                        m_rows = 0;
                        m_cols = 0;
                        m_tileWidth = 0;
                        m_tileHeight = 0;
                }
        }


        // Binds the texture array associated to the tileset to the OpenGL context
        void Tileset::activate() const
        {
                if(!isInit())
                {
                        logWarn("Tileset::activate() failed, the tileset has not been loaded correctly!");
                        return;
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureId);
        }


        // Unbinds the currently bound TEXTURE_2D_ARRAY from the OpenGL context
        void Tileset::deactivate() const
        {
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }


        // Creates a texture array in which each element holds a tile of the tileset
        // @imgData: the image pixels from which the tiles must be extracted
        // @imgChannelsNum: number of components for each pixel in the image (only 3 (RGB) and 4 (RGBA) are supported for now)
        // @rows: number of rows in which the image must be split
        // @cols: number of columns in which the image must be split
        // @tileWidth: the width of a single tile
        // @tileHeight: the height of a single tile
        // @returns: true if creation of texture array is successfull, false otherwise
        bool Tileset::createTextureArray(unsigned char* imgData, int imgChannelsNum, uint32_t rows, uint32_t cols, uint32_t tileWidth, uint32_t tileHeight)
        {
                if(imgData == nullptr)
                {
                        logError("Tileset::createTexturesArray() failed, given image data is nullptr!");
                        return false;
                }
                
                // Determine tileset (image) format
                GLenum format = GL_RGBA;
                switch(imgChannelsNum)
                {
                        case 3:         format = GL_RGB; break;
                        case 4:         format = GL_RGBA; break;

                        default:
                                logError("Tileset::createTexturesArray() failed, only images with RGB and RGBA image format can be used to create a tileset!");
                                return false;
                                break;
                }

                // If a tileset was already loaded then unload it before loading the new one
                if(isInit())
                        unload();

                // Create the opengl textures array
                glGenTextures(1, &m_textureId);
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureId);

                // Set settings for the textures in the textures array
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                // Allocate space for the texture array
                // TODO (maybe): For now we don't generate mipmaps but in the future we may need them (???)
                glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, tileWidth, tileHeight, rows * cols, 0, format, GL_UNSIGNED_BYTE, nullptr);

                const uint32_t tileRowSize = tileWidth * imgChannelsNum;
                const uint32_t imgRowSize = cols * tileRowSize;

                // Upload all tiles into the texture array (one tile at a time)
                std::vector<unsigned char> tileData(tileRowSize * tileHeight);
                for(uint32_t currRow = 0; currRow < rows; ++currRow)
                {
                        for(uint32_t currCol = 0; currCol < cols; ++currCol)
                        {
                                // Read the current tile's data (one row at at time)
                                for(uint32_t y = 0; y < tileHeight; y++)
                                {
                                        std::copy(
                                                &imgData[ (currRow * tileHeight * imgRowSize) + (currCol * tileRowSize) + ( y * imgRowSize) ],
                                                &imgData[ (currRow * tileHeight * imgRowSize) + (currCol * tileRowSize) + ( y * imgRowSize) + tileRowSize ],
                                                tileData.data() + (y * tileRowSize)
                                        );
                                }

                                // Insert current tile in the texture array
                                // note: we insert starting from the last texture in the texture array because stbi_flip messes the tiles indexes
                                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, ((rows * cols) - 1) - (currRow * cols) - (cols - 1 - currCol), tileWidth, tileHeight, 1, format, GL_UNSIGNED_BYTE, tileData.data());
                        }
                }
               
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

                // Set tileset proerties
                m_rows = rows;
                m_cols = cols;
                m_tileWidth = tileWidth;
                m_tileHeight = tileHeight;

                return true;
        }

}

