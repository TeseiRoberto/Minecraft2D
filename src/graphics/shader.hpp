
// Contains definition of the Shader class, this is an abstraction that encapsulates an opengl shader program

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <cstdint>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "logging.hpp"

namespace mc2d {

        class Shader
        {
        public:
                Shader();
                ~Shader();

                // Delete copy constructors
                Shader(Shader& other) = delete;
                Shader(const Shader& other) = delete;
                Shader operator = (Shader& other) = delete;
                Shader operator = (const Shader& other) = delete;

                int             init(const std::string& vrtxShaderFilename, const std::string& fragShaderFilename);
                void            terminate();

                inline bool     isInit() const          { return m_programId != 0; }
                void            activate() const;
                inline void     deactivate() const      { glUseProgram(0); }

                int             getUniformId(const std::string& uniformName) const;
                void            setUniform(int uniformId, const glm::mat4x4& value) const;

        private:

                std::string     loadSourceCode(const std::string& filename) const;
                uint32_t        createShader(const std::string& sourceCode, GLenum shaderType) const;
                uint32_t        createShaderProgram(uint32_t vertxShader, uint32_t fragShader) const;

                uint32_t        m_programId;
        };

};

#endif // SHADER_H
