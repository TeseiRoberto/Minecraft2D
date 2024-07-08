
// Contains definition of the Tileset class

#ifndef TILESET_H
#define TILESET_H

#include <string>
#include <cstdint>
#include <glad/glad.h>
#include <stb_image.h>

#include "logging.hpp"

namespace mc2d {

        class Tileset {
        public:
                Tileset();
                ~Tileset() = default;

                // Delete copy constructors
                Tileset(Tileset& other) = delete;
                Tileset(const Tileset& other) = delete;
                Tileset operator = (Tileset& other) = delete;
                Tileset operator = (const Tileset& other) = delete;

                bool                    load(const std::string& filepath, uint32_t rows, uint32_t cols, uint32_t tileWidth, uint32_t tileHeight);
                void                    unload();

                inline bool             isValid() const         { return m_textureId != 0; }
                void                    activate() const;
                void                    deactivate() const;

                inline const uint32_t   getRowsNum() const      { return m_rows; }
                inline const uint32_t   getColsNum() const      { return m_cols; }

        private:
                uint32_t        m_rows;                         // Number of rows in the tileset
                uint32_t        m_cols;                         // Number of columns in the tileset
                uint32_t        m_tileWidth;                    // The width of one tile in the tileset
                uint32_t        m_tileHeight;                   // The height of one tile in the tileset
        
                uint32_t        m_textureId;                    // The id associated to the opengl texture
        };

}

#endif // TILESET_H