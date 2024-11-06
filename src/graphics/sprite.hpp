
// Contains definition of the Sprite class

#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include <glad/glad.h>
#include <stb_image.h>

#include "log.hpp"

namespace mc2d {


        class Sprite {
        public:
                Sprite();
                ~Sprite() = default;

                // Delete copy constructors
                Sprite (Sprite& other) = delete;
                Sprite (const Sprite& other) = delete;
                Sprite operator = (Sprite& other) = delete;
                Sprite operator = (const Sprite& other) = delete;

                bool            load(const std::string& filepath);
                void            unload();

                inline bool     isInit() const                  { return m_textureId != 0; }
                void            activate() const;
                void            deactivate() const;

        private:

                bool            createTexture(unsigned char* imgData, uint32_t imgWidth, uint32_t imgHeight, uint32_t imgChannelsNum);

                uint32_t        m_textureId;            // The id associated to the opengl texture
        };

}

#endif // SPRITE_H
