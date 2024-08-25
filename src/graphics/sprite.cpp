
#include "sprite.hpp"

namespace mc2d {


        Sprite::Sprite() : m_textureId(0)
        {}


        // Loads an image and uses it to create a texture that can be used as a sprite
        // @filepath: path to the image file that must be used as a sprite
        // @returns: true if texture creation for the sprite is successfull, false otherwise
        bool Sprite::load(const std::string& filepath)
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
                        logError("Sprite::load() failed, stb_image could not load image \"%s\"!", filepath.c_str());
                        return false;
                }

                bool res = createTexture(imgData, imgWidth, imgHeight, imgChannelsNum);
                stbi_image_free(imgData);
                return res;
        }


        // Destroyes the OpenGL texture associated to the sprite
        void Sprite::unload()
        {
                if(isInit())
                {
                        glDeleteTextures(1, &m_textureId);
                        m_textureId = 0;
                }
        }


        // Binds the texture associated to the sprite to the OpenGL context
        void Sprite::activate() const
        {
                if(!isInit())
                {
                        logWarn("Sprite::activate() failed, the sprite has not been loaded correctly!");
                        return;
                }

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_textureId);
        }


        // Unbinds the currently bound GL_TEXTURE_2D from the OpenGL context
        void Sprite::deactivate() const
        {
                glBindTexture(GL_TEXTURE_2D, 0);
        }


        // Creates an OpenGL texture using the given data
        // @imgData: the image pixels from which the texture will be created
        // @imgWidth: the width of the image and of the resulting texture
        // @imgHeight: the height of the image and of the resulting texture
        // @imgChannelsNum: the number of components for each pixel of the image (only 3 (RGB) and 4 (RGBA) are supported for now)
        // @returns: true if texture creation is successfull, false otherwise
        bool Sprite::createTexture(unsigned char* imgData, uint32_t imgWidth, uint32_t imgHeight, uint32_t imgChannelsNum)
        {
                if(imgData == nullptr)
                {
                        logError("Sprite::createTexture() failed, given image data is nullptr!");
                        return false;
                }

                // Determine texture format
                GLenum format = GL_RGBA;
                switch(imgChannelsNum)
                {
                        case 3:         format = GL_RGB; break;
                        case 4:         format = GL_RGBA; break;

                        default:
                                logError("Sprite::createTexture() failed, only images with RGB and RGBA image format can be used to create a tileset!");
                                return false;
                                break;
                }

                // If there is already a texure loaded for this sprite then unload it
                if(isInit())
                        unload();

                // Creates a new OpenGL texture
                glGenTextures(1, &m_textureId);
                glBindTexture(GL_TEXTURE_2D, m_textureId);

                // Set settings for the texture
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                // Upload image data into the texture
                glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, imgData);

                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);

                return true;
        }


}

