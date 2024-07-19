
#include "shader.hpp"

namespace mc2d {

        // Dummy constructor, creates an empty (invalid) shader
        Shader::Shader() : m_programId(0)
        {}


        Shader::~Shader()
        {
                terminate();
        }

        // Tries to create a shader program using the given shaders source files
        // @vrtxShaderFilename: name of the file from which the vertex shader source code will be loaded
        // @fragShaderFilename: name of the file from which the fragment shader source code will be loaded
        // @returns: zero on success, non zero otherwise
        int Shader::init(const std::string& vrtxShaderFilename, const std::string& fragShaderFilename)
        {
                // If a shader program has alreaby been created then we destroy that first
                if(isValid())
                {
                        glDeleteProgram(m_programId);
                        m_programId = 0;
                }

                // Load source code for the shaders
                std::string vrtxShaderCode = loadSourceCode(vrtxShaderFilename);
                std::string fragShaderCode = loadSourceCode(fragShaderFilename);

                // Compile shaders
                uint32_t vrtxShaderId = createShader(vrtxShaderCode, GL_VERTEX_SHADER);
                uint32_t fragShaderId = createShader(fragShaderCode, GL_FRAGMENT_SHADER);

                // Link shaders into a shader program
                m_programId = createShaderProgram(vrtxShaderId, fragShaderId);

                if(vrtxShaderId != 0)
                        glDeleteShader(vrtxShaderId);

                if(fragShaderId != 0)
                        glDeleteShader(fragShaderId);

                return m_programId == 0 ? 1 : 0;
        }


        // Destroyes the shader program
        void Shader::terminate()
        {
                if(!isValid())
                        return;

                glDeleteProgram(m_programId);
                m_programId = 0;
        }


        // Set this shader program as the one to be used for rendering
        void Shader::activate() const
        {
                if(!isValid())
                {
                        logError("Shader::activate() failed, trying to use an invalid shader program!");
                        return;
                }

                glUseProgram(m_programId);
        }


        // Tries to retrive the id (location) of a uniform
        // @uniformName: the name of the uniform
        // @returns: the id (location) of the uniform associated to the given name, -1 on error
        int Shader::getUniformId(const std::string& uniformName) const
        {
                if(!isValid())
                {
                        logError("Shader::getUniformId() failed, shader has not been initialized!");
                        return -1;
                }

                if(uniformName.size() == 0)
                {
                        logError("Shader::getUniformId() failed, given uniform name is empty!");
                        return -1;
                }

                return glGetUniformLocation(m_programId, uniformName.c_str());
        }


        // Set value for the given uniform
        // @uniform: id of the uniform to be setted
        // @value: value used to set the uniform
        void Shader::setUniform(int uniformId, const glm::mat4x4& value) const
        {
                if(!isValid())
                {
                        logWarn("Shader::setUniform() failed, shader has not been initialized!");
                        return;
                }

                if(uniformId == -1)
                {
                        logWarn("Shader::setUniform() failed, given uniform id is invalid!");
                        return;
                }

                glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
        }


        // Loads the content of the file with given filename and stores it into a string
        // @filename: name of the file from which code must be loaded
        // @returns: a string that contains the source file content
        std::string Shader::loadSourceCode(const std::string& filename) const
        {
                std::ifstream srcFile(filename);
                if(!srcFile.is_open())
                {
                        logError("Shader::loadSourceCode() failed, file \"%s\" has not been found!", filename.c_str());
                        return "";
                }

                std::string currLine;
                std::string srcCode;
                while(getline(srcFile, currLine))
                {
                     srcCode += currLine + '\n';
                     currLine = "";
                }

                srcFile.close();
                return srcCode;
        }


        // Compiles a shader
        // @sourceCode: the source code to be compiled
        // @returns: id of the compiled opengl shader, 0 on error
        uint32_t Shader::createShader(const std::string& sourceCode, GLenum shaderType) const
        {
                if(sourceCode.size() == 0)
                {
                        logError("Shader::createShader() failed, shader source code is empty!");
                        return 0;
                }

                if(shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER)
                {
                        logError("Shader::createShader() failed, shader type is not \"GL_VERTEX_SHADER\" nor \"GL_FRAGMENT_SHADER\"!");
                        return 0;
                }

                int success = false;
                const char* sourceCodePtr = sourceCode.c_str();

                uint32_t shaderId = glCreateShader(shaderType);         // Create shader object
                glShaderSource(shaderId, 1, &sourceCodePtr, NULL);      // Set shader source code
                glCompileShader(shaderId);                              // Try to compile the given source code

                glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);   // Check if compilation was successfull or not
                if(!success)
                {
                        char errorMsg[256];
                        glGetShaderInfoLog(shaderId, 256, NULL, errorMsg);
                        logError("Shader::createShader() failed, shader compilation failed:\n%s", errorMsg);
                        return 0;
                }
                
                return shaderId;
        }


        // Links the givens shaders into a shader program
        // @vrtxShader: id of the vertex shader to be linked
        // @fragShader: id of the fragment shader to be linked
        // @returns: id of the linked opengl shader program, 0 on error
        uint32_t Shader::createShaderProgram(uint32_t vrtxShader, uint32_t fragShader) const
        {
                if(vrtxShader == 0 || fragShader == 0)
                {
                        logError("Shader::createShaderProgram() failed, one of the given shader has not been created correctly!");
                        return 0;
                }

                int success = false;
                uint32_t programId  = glCreateProgram();                // Create chader program object

                glAttachShader(programId, vrtxShader);                  // Attach shaders to the program
                glAttachShader(programId, fragShader);
                glLinkProgram(programId);                               // Link the program (outputs of one shader to inputs of the other one)

                glGetProgramiv(programId, GL_LINK_STATUS, &success);    // Check if linking was successfull
                if(!success)
                {
                        char errorMsg[256];
                        glGetProgramInfoLog(programId, 256, NULL, errorMsg);
                        logError("Shader::createShaderProgram() failed, shader program linking failed:\n%s", errorMsg);
                        return 0;
                }
                
                return programId;
        }

}
