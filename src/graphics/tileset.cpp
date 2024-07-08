
#include "tileset.hpp"

namespace mc2d {

        Tileset::Tileset() : m_rows(0), m_cols(0), m_tileWidth(0), m_tileHeight(0), m_textureId(0)
        {}


        // Loads a 2D texture to be used as a tileset
        // @filepath: path to the image file to be used as tileset
        // @rows: number of rows in which the image must be split
        // @cols: number of columns in which the image must be split
        // @tileWidth: the width of a single tile
        // @tileHeight: the height of a single tile
        // @returns: true if the tileset gets loaded correctly, false otherwise
        bool Tileset::load(const std::string& filepath, uint32_t rows, uint32_t cols, uint32_t tileWidth, uint32_t tileHeight)
        {
                stbi_set_flip_vertically_on_load(true); // TODO: move this somewhere else

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

                // Create the opengl texture
                glGenTextures(1, &m_textureId);
                glBindTexture(GL_TEXTURE_2D, m_textureId);

                // Set tileset's texture settings
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                // Determine texture format
                GLenum format = GL_RGBA;
                switch(imgChannelsNum)
                {
                        case 3:         format = GL_RGB; break;
                        case 4:         format = GL_RGBA; break;

                        default:
                                logError("Tileset::load() failed, only images with RGB and RGBA image format can be used to create a tileset!");

                                glDeleteTextures(1, &m_textureId);
                                glBindTexture(GL_TEXTURE_2D, 0);
                                m_textureId = 0;

                                stbi_image_free(imgData);
                                return false;
                                break;
                }

                // TODO (maybe): For now we don't generate mipmaps but in future we may need them (???)
                
                // Upload data to the texture
                glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);
                glBindTexture(GL_TEXTURE_2D, 0);

                // Set tileset proerties
                m_rows = rows;
                m_cols = cols;
                m_tileWidth = tileWidth;
                m_tileHeight = tileHeight;

                stbi_image_free(imgData);
                return true;
        }


        // Destroyes the opengl texture associated to the tileset
        void Tileset::unload()
        {
                if(isValid())
                {
                        glDeleteTextures(1, &m_textureId);
                        m_textureId = 0;

                        m_rows = 0;
                        m_cols = 0;
                        m_tileWidth = 0;
                        m_tileHeight = 0;
                }
        }


        // Binds the tileset texture to the opengl context
        void Tileset::activate() const
        {
                if(!isValid())
                {
                        logWarn("Tileset::activate() failed, the tileset has not been loaded correctly!");
                        return;
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_textureId);
        }


        // Unbinds the currently bound TEXTURE_2D from the opengl context
        void Tileset::deactivate() const
        {
                glBindTexture(GL_TEXTURE_2D, 0);
        }

}

