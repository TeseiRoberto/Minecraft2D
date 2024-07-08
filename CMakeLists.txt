
cmake_minimum_required(VERSION 3.20)

project(Minecraft2D VERSION 1.0)

set (SRCS
        libs/glad/src/glad.c
        #libs/stbImage/stb_image.cpp
        src/main.cpp
        src/game.cpp
        src/graphics/shader.cpp
        src/graphics/renderer.cpp

        src/chunk.cpp
)

add_executable(minecraft2D ${SRCS})

target_include_directories(minecraft2D PRIVATE src/ libs/glad/include/ )#libs/glm/ libs/stbImage/)
target_link_libraries(minecraft2D glfw)
