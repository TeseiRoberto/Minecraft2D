#version 330 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in float inTileId;

uniform mat4 transformMatrix;

out vec2 uv;
out float tileId;

void main()
{
        gl_Position = transformMatrix * vec4(inPos, 0.0f, 1.0f);

        uv = inUv;
        tileId = inTileId;
}
